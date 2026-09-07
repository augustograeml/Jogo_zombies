#include "../Logica/mundo.h"
#include "../Entidades/Personagens/jogador.h"
#include "../Entidades/Personagens/zumbi.h"
#include "../Entidades/Personagens/arqueiro.h"
#include "../Entidades/Personagens/gigante.h"
#include "../Entidades/Obstaculos/coracao.h"
#include "../Persistencia/aleatorio.h"
#include "../Persistencia/pontos.h"
namespace Logica {
FimPasso simular(Mundo& m) {
    auto& jogadores=m.jogadores; auto& inimigos=m.inimigos; auto& obstaculos=m.obstaculos;
    auto& gC=m.gC; auto& sessao=m.sessao; auto& resultado=m.resultado;
    auto& motor_fase=m.motor_fase;
    auto& eventos=m.eventos;
    if (resultado.finalizada) return FimPasso::Nenhum;
    Persistencia::motor() = motor_fase;
    std::vector<Logica::Alvo> alvos;
    struct Antes { Entidades::Entidade* entidade; int vida; bool vivo; float vy; };
    std::vector<Antes> antes;
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        auto* j = static_cast<Entidades::Personagens::Jogador*>(*it);
        j->atualizar_protecao();
        alvos.push_back({j->eh_jogador2() ? 2 : 1, j->getPosicao().x, j->getPosicao().y, j->get_vivo()});
        antes.push_back({j,j->get_vida(),j->get_vivo(),j->getVelocidade().y});
    }
    for (auto it = inimigos.get_primeiro(); it != nullptr; ++it) {
        auto* a = dynamic_cast<Entidades::Personagens::Arqueiro*>(*it);
        if (a) a->perceber(alvos);
        antes.push_back({*it,(*it)->get_vida(),(*it)->get_vivo(),0});
    }
    for (auto it = obstaculos.get_primeiro(); it != nullptr; ++it)
        if (dynamic_cast<Entidades::Obstaculos::Coracao*>(*it))
            antes.push_back({*it,(*it)->get_vida(),(*it)->get_vivo(),0});
    jogadores.executar();
    inimigos.executar();
    motor_fase = Persistencia::motor();
    gC.gerenciar_colisoes();
    for(auto it=inimigos.get_primeiro();it!=nullptr;++it)
        static_cast<Entidades::Personagens::Inimigo*>(*it)->avancar_animacao();
    // Morte por queda evita uma partida sem possibilidade de terminar.
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it)
        if ((*it)->get_vida() <= 0 || (*it)->getPosicao().y > 2000) (*it)->morrer();
    for (const auto& a : antes) {
        auto* e = a.entidade;
        if (auto* j = dynamic_cast<Entidades::Personagens::Jogador*>(e)) {
            if (j->get_vida() < a.vida) eventos.publicar({Logica::Evento::Dano});
            if (a.vy >= 0 && j->getVelocidade().y < -5) eventos.publicar({Logica::Evento::Salto});
        }
        if (auto* arq = dynamic_cast<Entidades::Personagens::Arqueiro*>(e))
            if (arq->disparou_no_passo()) eventos.publicar({Logica::Evento::Disparo});
        if (a.vivo && e->get_vida()<a.vida && dynamic_cast<Entidades::Personagens::Inimigo*>(e))
            eventos.publicar({Logica::Evento::ImpactoInimigo});
        if (a.vivo && !e->get_vivo()) {
            using namespace Persistencia::Pontuacao;
            if (dynamic_cast<Entidades::Obstaculos::Coracao*>(e)) eventos.publicar({Logica::Evento::Coleta, coleta});
            else if (dynamic_cast<Entidades::Personagens::Inimigo*>(e)) {
                const int pontos = dynamic_cast<Entidades::Personagens::Gigante*>(e) ? gigante :
                    dynamic_cast<Entidades::Personagens::Arqueiro*>(e) ? arqueiro : zumbi;
                eventos.publicar({Logica::Evento::InimigoDerrotado, pontos});
            }
        }
    }
    ++sessao.passos;
    for(auto it=jogadores.get_primeiro();it!=nullptr;++it)
        static_cast<Entidades::Personagens::Jogador*>(*it)->set_tempo(static_cast<double>(sessao.passos)/60.0);
    if(gC.get_jogadores_vivos()) return FimPasso::Derrota;
    if(gC.get_inimigos_vivos()) return FimPasso::Vitoria;
    return FimPasso::Nenhum;
}
}
