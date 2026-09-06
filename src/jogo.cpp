#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Audio/efeitos.h"
#include "../Persistencia/slots.h"
#include "../Persistencia/aleatorio.h"
#include "../Recursos/catalogo.h"
#include "../jogo.h"
#include <memory>

Jogo::Jogo() : pG(Gerenciadores::Gerenciador_Grafico::get_instancia()),
    pE(Gerenciadores::Gerenciador_Estados::get_instancia()),
    pEv(Gerenciadores::Gerenciador_Eventos::get_instancia()) {
    Persistencia::Slots::instancia().definir_validador([](const Persistencia::Json& j) {
        const auto motor = Persistencia::motor();
        try {
            const int id=Persistencia::inteiro(j.at("estado"),6,9);
            std::unique_ptr<Estados::Fases::Fase> f;
            if(id<8) f=std::make_unique<Estados::Fases::Fase1>(id,true);
            else f=std::make_unique<Estados::Fases::Fase2>(id,true);
            f->restaurar(j);
        } catch (...) { Persistencia::motor()=motor; throw; }
        Persistencia::motor()=motor;
    });
    try { Audio::habilitar(); } catch(const std::exception& e) { pE->mensagem=e.what(); }
    pE->adicionar_estado(new Estados::Menus::Menu_Principal(0));
    pE->adicionar_estado(new Estados::Menus::Menu_Jogadores(1));
    pE->adicionar_estado(new Estados::Menus::Menu_Fase(2, false));
    pE->adicionar_estado(new Estados::Menus::Menu_Fase(3, true));
    pE->adicionar_estado(new Estados::Menus::Ranking(4));
    pE->adicionar_estado(new Estados::Menus::Pause(5));
    pE->adicionar_estado(new Estados::Menus::Nome(10));
    Executar();
}
Jogo::~Jogo() {
    // As telas pertencem exclusivamente ao gerenciador. Graficos saem por ultimo.
    delete pE;
    delete pEv;
    delete pG;
}
void Jogo::Executar() {
    sf::Font fonte;
    fonte.loadFromFile(Recursos::caminho("Design/fonte/fonte_simas.ttf").string());
    while (pG->get_JanelaAberta()) {
        pEv->executar();
        if (!pG->get_JanelaAberta()) break;
        pG->limpar();
        pE->executar();
        if (!pE->mensagem.empty()) {
            const auto camera = pG->get_Janela()->getView();
            pG->get_Janela()->setView(pG->get_Janela()->getDefaultView());
            sf::RectangleShape fundo({1004, 70});
            fundo.setFillColor(sf::Color(0, 0, 0, 220));
            const float y = pE->get_estado_atual() >= 6 && pE->get_estado_atual() <= 9 ? 168.f : 10.f;
            fundo.setPosition(10, y);
            sf::Text aviso(pE->mensagem, fonte, 18);
            aviso.setPosition(20, y + 10);
            const float largura = aviso.getLocalBounds().width;
            if (largura > 980) aviso.setScale(980 / largura, 980 / largura);
            pG->get_Janela()->draw(fundo);
            pG->get_Janela()->draw(aviso);
            pG->get_Janela()->setView(camera);
        }
        pG->mostrar();
    }
}
void Jogo::reseta_fase() {}
