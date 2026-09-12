#include "../Logica/mundo.h"
#include "../Interface/preferencias.h"
#include "../Persistencia/mundo.h"
#include "../Estados/Fases/construtor_cenario.h"
#include "../Persistencia/slots.h"
#include "../Persistencia/pontos.h"
#include "../Persistencia/migracao.h"
#include "../Estados/Fases/fase.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/aleatorio.h"
#include "../Persistencia/ranking.h"
#include <chrono>
#include <fstream>
#include <memory>
#include <set>

namespace Estados::Fases {
using Persistencia::Json;
Fase::Fase(int id, bool carregar) : Estado(id), ja_criado(carregar),
    jogador2(id == 7 || id == 9), num_jogadores(jogador2 ? 2 : 1) {
    eventos.assinar(pontuacao);
    eventos.assinar(som);
    gC.set_jogadores(&jogadores);
    gC.set_inimigos(&inimigos);
    gC.set_obstaculos(&obstaculos);
    std::random_device aleatorio;
    partida_id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) +
                 "-" + std::to_string(aleatorio()) + "-" + std::to_string(id);
    if (!carregar) criar_jogadores();
}
void Fase::criar_jogadores() {
    // Insercao invertida mantem jogador 1 como primeiro elemento.
    if (jogador2) jogadores.incluir(new Entidades::Personagens::Jogador({150, 50}, {0, 0}, true));
    auto* primeiro=new Entidades::Personagens::Jogador({80, 50}, {0, 0}, false);
    primeiro->set_controles_solo(num_jogadores==1);
    jogadores.incluir(primeiro);
}
void Fase::criar_inimigos(std::string caminho) { ConstrutorCenario::inimigos(caminho, inimigos); }
void Fase::criar_cenario(std::string caminho) { ConstrutorCenario::obstaculos(caminho, obstaculos); gC.invalidar_grade(); }
void Fase::ao_entrar() {
    painel.atualizar_recorde(get_numero_fase(), num_jogadores);
    relogio.restart(); // Descarta tempo no menu, em pausa e fora do processo.
    atualizar();
}
void Fase::tratar_evento(const sf::Event& evento) {
    if ((evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Escape) ||
        evento.type == sf::Event::LostFocus) pGE->set_estado_atual(Estados::Tela::Pausa);
}
void Fase::executar_comum() {
    sessao.acumular(relogio.restart().asSeconds());
    while (!resultado.finalizada && sessao.proximo()) {
        simular_passo();
    }
    if (pGE->get_estado_atual() != Estado::id) return;
    desenhar_partida();
}
void Fase::desenhar_partida() {
    const auto inicio_desenho=std::chrono::steady_clock::now();
    atualizar();
    for(const auto& vista:vistas) {
    pGG->get_Janela()->setView(vista);
    pGG->desenharFundo(&shape);
    const auto& camera=pGG->get_Janela()->getView();
    const auto canto=camera.getCenter()-camera.getSize()/2.f;
    // Margem conserva sprites que ultrapassam a caixa fisica; so o desenho e filtrado.
    const sf::FloatRect visivel(canto.x-100,canto.y-100,camera.getSize().x+200,camera.getSize().y+200);
    obstaculos.desenhar(visivel);
    jogadores.desenhar(visivel);
    inimigos.desenhar(visivel);
    for (auto it = inimigos.get_primeiro(); it != nullptr; ++it) {
        auto* projeteis = static_cast<Entidades::Personagens::Inimigo*>(*it)->get_projeteis();
        if (projeteis) for (auto& p : *projeteis) if (p.get_vivo() && visivel.intersects(p.get_corpo()->getGlobalBounds())) p.desenhar();
    }
    }
    std::vector<int> vidas;
    for(auto it=jogadores.get_primeiro();it!=nullptr;++it) vidas.push_back((*it)->get_vida());
    painel.desenhar(*pGG->get_Janela(), get_tempo_sessao(), get_pontos(), vidas);
    if(vistas.size()==2) {
        auto* janela=pGG->get_Janela(); const auto anterior=janela->getView();
        janela->setView(Interface::vista_interface(janela->getSize()));
        const auto tela=Interface::enquadramento(janela->getSize());
        for(std::size_t i=0;i<vistas.size();++i) {
            const auto area=vistas[i].getViewport();
            const float x=(area.left-tela.left)/tela.width*1024;
            const float y=(area.top-tela.top)/tela.height*1024;
            sf::RectangleShape borda({area.width/tela.width*1024,area.height/tela.height*1024});
            borda.setPosition(x,y); borda.setFillColor(sf::Color::Transparent);
            borda.setOutlineThickness(-2); borda.setOutlineColor(sf::Color::White); janela->draw(borda);
            sf::RectangleShape etiqueta({44,30}); etiqueta.setPosition(x+4,y+4);
            etiqueta.setFillColor(sf::Color::Black); janela->draw(etiqueta);
            sf::Text nome("J"+std::to_string(i+1),Interface::fonte_interface(),20);
            nome.setPosition(x+10,y+5); janela->draw(nome);
        }
        janela->setView(anterior);
    }
    micros_desenho=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-inicio_desenho).count();
}
void Fase::atualizar() {
    std::vector<sf::Vector2f> vivos;
    for(auto it=jogadores.get_primeiro();it!=nullptr;++it)
        if((*it)->get_vivo()) vivos.push_back((*it)->getPosicao()+sf::Vector2f(25,25));
    auto* janela=pGG->get_Janela();
    vistas=camera_dupla.atualizar(vivos,shape.getGlobalBounds(),Interface::altura_painel(),janela->getSize());
    janela->setView(vistas.front());
}
void Fase::set_tempo_jogadores() {
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it)
        static_cast<Entidades::Personagens::Jogador*>(*it)->set_tempo(get_tempo());
}
void Fase::fim_de_jogo() { concluir(true); }
void Fase::concluir(bool venceu) {
    if (resultado.finalizada) return;
    resultado.finalizada = true;
    resultado.vitoria = venceu;
    if (venceu) eventos.publicar({Logica::Evento::Vitoria, Persistencia::Pontuacao::conclusao});
    else eventos.publicar({Logica::Evento::Derrota});
    set_tempo_jogadores();
    try { salvar(); }
    catch (const std::exception& erro) { pGE->mensagem = std::string("Falha ao salvar resultado: ") + erro.what(); }
    pGE->set_estado_atual(Estados::Tela::Nome);
}
void Fase::salvar(const std::filesystem::path& caminho) {
    if (caminho.empty()) Persistencia::Slots::instancia().salvar(capturar());
    else Persistencia::escrever_json(caminho, capturar());
}
bool Fase::registrar_resultado(const std::vector<std::string>& nomes) {
    if (!resultado.finalizada) throw std::runtime_error("A partida ainda nao terminou.");
    if (resultado.nomes_confirmados) { salvar(); return resultado.ranking_registrado; }
    if (nomes.size() != static_cast<std::size_t>(num_jogadores))
        throw std::runtime_error("Informe o nome de cada jogador.");
    std::vector<std::string> validados;
    for (const auto& nome : nomes) validados.push_back(Persistencia::nome_valido(nome));
    // Derrotas recebem nome, mas nao competem com tempos de fases concluidas.
    if (resultado.vitoria) {
        Persistencia::RepositorioRanking().registrar({partida_id, get_numero_fase(), num_jogadores, validados, get_tempo()});
    }
    if (get_pontos() > 0) Persistencia::RepositorioPontos().registrar(
        {partida_id, get_numero_fase(), num_jogadores, validados, get_pontos(), get_tempo(), resultado.vitoria});
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        auto* jogador = static_cast<Entidades::Personagens::Jogador*>(*it);
        jogador->set_nome(validados.at(jogador->eh_jogador2() ? 1 : 0));
    }
    resultado.ranking_registrado = resultado.vitoria;
    resultado.nomes_confirmados = true;
    salvar();
    return resultado.ranking_registrado;
}
}

namespace Estados::Fases {
Logica::Mundo Fase::mundo() {
    return {jogadores,inimigos,obstaculos,gC,sessao,resultado,pontuacao,eventos,motor_fase,partida_id,Estado::id,num_jogadores};
}
void Fase::simular_passo() {
    if(resultado.finalizada) return;
    const auto inicio=std::chrono::steady_clock::now();
    auto contexto=mundo();
    const auto fim=Logica::simular(contexto);
    if(fim!=Logica::FimPasso::Nenhum) concluir(fim==Logica::FimPasso::Vitoria);
    micros_simulacao=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-inicio).count();
}
Json Fase::capturar() { auto contexto=mundo(); return Persistencia::capturar_mundo(contexto); }
void Fase::restaurar(const Json& dados) {
    auto contexto=mundo(); Persistencia::restaurar_mundo(contexto,dados); relogio.restart();
}
}
