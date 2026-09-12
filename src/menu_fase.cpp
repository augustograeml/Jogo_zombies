#include "../Recursos/catalogo.h"
#include "../Estados/Menus/menu_fase.h"
#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include <memory>
namespace Estados::Menus {
Menu_Fase::Menu_Fase(int id, bool dois) : Menu(id), pObserver(nullptr) { set_jogador2(dois); inicializa_valores(); }
Menu_Fase::~Menu_Fase() = default;
void Menu_Fase::inicializa_valores() {
    imagem->loadFromFile(Recursos::caminho("Design/imagens/menu2fases.png").string());
    fonte->loadFromFile(Recursos::caminho("Design/fonte/Teko-Bold.otf").string());
    opcoes = {"Fases", "Fase 1", "Fase 2", "Sair"};
    coordenadas = {{270, 40}, {360, 604}, {600, 604}, {493, 650}};
    tamanhos = {200, 20, 20, 20};
    textos.resize(opcoes.size());
    for (std::size_t i = 0; i < textos.size(); ++i) {
        textos[i].setFont(*fonte); textos[i].setString(opcoes[i]);
        textos[i].setCharacterSize(tamanhos[i]); textos[i].setPosition(coordenadas[i]);
        textos[i].setOutlineColor(sf::Color::Black);
    }
    textos[0].setOutlineThickness(20); textos[1].setOutlineThickness(4);
}
void Menu_Fase::selecionar() {
    if (pos == 3) { pGE->set_estado_atual(Estados::Tela::Jogadores); return; }
    const int estado = (pos == 1 ? 6 : 8) + (jogador2 ? 1 : 0);
    std::unique_ptr<Fases::Fase> fase;
    if (pos == 1) fase = std::make_unique<Fases::Fase1>(estado, false);
    else fase = std::make_unique<Fases::Fase2>(estado, false);
    // O primeiro checkpoint tambem substitui uma partida anterior concluida.
    fase->salvar();
    pGE->adicionar_estado(fase.get()); fase.release();
    pGE->set_estado_atual(estado);
}
void Menu_Fase::loop_evento() {}
}
