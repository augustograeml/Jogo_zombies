#include "../Estados/Fases/fase.h"
#include "../Logica/mundo.h"
#include "../Persistencia/mundo.h"
#include "../Persistencia/aleatorio.h"
#include "../Recursos/limites.h"
#include "../Recursos/plataforma.h"
#include <cmath>
namespace Estados::Fases {
using Persistencia::Json;
void Fase::verificar_checkpoint() {
    if(sessao.passos%60!=0 || resultado.finalizada) return;
    int regiao=-1;
    for(auto it=jogadores.get_primeiro();it!=nullptr;++it) {
        auto* j=*it;const auto r=j->get_corpo()->getGlobalBounds();
        const int atual=static_cast<int>(std::max(0.f,r.left-limites.left)/4000);
        if(!j->get_vivo() || j->get_vida()<=0 || std::abs(j->getVelocidade().y)>.01f ||
           !gC.existe_solido({r.left,r.top+r.height,r.width,3}))return;
        if(regiao>=0 && regiao!=atual)return;
        regiao=atual;
        // Nao registra no meio de um combate nem sobre uma armadilha.
        for(auto e=inimigos.get_primeiro();e!=nullptr;++e) {
            const auto p=(*e)->getPosicao();
            if((*e)->get_vivo() && std::hypot(p.x-r.left,p.y-r.top)<750)return;
            auto* flechas=static_cast<Entidades::Personagens::Inimigo*>(*e)->get_projeteis();
            if(flechas)for(auto& f:*flechas) {
                const auto p=f.getPosicao();if(f.get_vivo() && std::hypot(p.x-r.left,p.y-r.top)<750)return;
            }
        }
        for(auto o=obstaculos.get_primeiro();o!=nullptr;++o)
            if(dynamic_cast<Entidades::Obstaculos::Espinho*>(*o) && (*o)->get_vivo() &&
               std::abs((*o)->getPosicao().x-r.left)<150 && std::abs((*o)->getPosicao().y-r.top)<200)return;
    }
    if(regiao<=regiao_checkpoint)return;
    auto contexto=mundo();auto novo=Persistencia::capturar_mundo(contexto);
    const auto anterior=checkpoint;const int antiga=regiao_checkpoint;
    checkpoint=std::move(novo);regiao_checkpoint=regiao;
    try{salvar();pGE->mensagem="Checkpoint da regiao "+std::to_string(regiao+1)+" salvo.";}
    catch(const std::exception& e){checkpoint=anterior;regiao_checkpoint=antiga;pGE->mensagem=std::string("Falha no checkpoint: ")+e.what();}
}
Json Fase::capturar() {
    auto contexto=mundo();auto dados=Persistencia::capturar_mundo(contexto);
    if(!checkpoint.is_null()) {dados["checkpoint"]=checkpoint;dados["regiao_checkpoint"]=regiao_checkpoint;}
    return dados;
}
void Fase::restaurar(const Json& dados) {
    auto novo=dados.value("checkpoint",Json());
    int regiao=-1;
    if(!novo.is_null()) {
        regiao=Persistencia::inteiro(dados.at("regiao_checkpoint"),0,100000);
        if(novo.contains("checkpoint") || novo.at("finalizada").get<bool>() ||
           novo.at("partida_id")!=dados.at("partida_id") || novo.at("passos").get<double>()>dados.at("passos").get<double>())
            throw std::runtime_error("Checkpoint inconsistente.");
        // Valida todo o snapshot antes de modificar qualquer entidade da partida.
        Listas::ListaEntidade j,i,o;Gerenciadores::Gerenciador_Colisoes gc;
        Logica::Sessao s;Logica::ResultadoPartida r;Logica::Pontuacao p;Logica::Eventos e;
        std::mt19937 motor;std::string id;
        Logica::Mundo teste{j,i,o,gc,s,r,p,e,motor,id,Estado::id,num_jogadores};
        const auto anterior=Persistencia::motor();
        try {Persistencia::restaurar_mundo(teste,novo);}catch(...){Persistencia::motor()=anterior;throw;}
        Persistencia::motor()=anterior;
        for(auto it=j.get_primeiro();it!=nullptr;++it)if(!(*it)->get_vivo() || (*it)->get_vida()<=0)
            throw std::runtime_error("Checkpoint sem jogadores vivos.");
    }
    auto contexto=mundo();Persistencia::restaurar_mundo(contexto,dados);
    checkpoint=std::move(novo);regiao_checkpoint=regiao;
    Recursos::encaixar_plataformas(obstaculos);limites=Recursos::limites_mundo(obstaculos);relogio.restart();
}
void Fase::recuperar_checkpoint() {
    if(!pode_recuperar_checkpoint())throw std::runtime_error("Checkpoint indisponivel.");
    const auto anterior=capturar();auto novo=checkpoint;
    novo["passos"]=sessao.passos;novo["passos_anteriores"]=sessao.passos_anteriores;novo["acumulador"]=0;
    novo["assistida"]=true;novo["checkpoint"]=checkpoint;novo["regiao_checkpoint"]=regiao_checkpoint;
    try {restaurar(novo);set_tempo_jogadores();salvar();}
    catch(...) {restaurar(anterior);throw;}
    pGE->mensagem="Checkpoint recuperado. Tentativa assistida, fora dos rankings.";
}
}
