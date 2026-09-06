#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include <memory>

namespace Estados::Menus {
Menu_Principal::Menu_Principal(int id) : Menu(id), jacriado(false), pObserver(nullptr) { inicializa_valores(); }
Menu_Principal::~Menu_Principal() = default;
void Menu_Principal::inicializa_valores() {
    imagem->loadFromFile("Design/imagens/menu_zombies++.jpg");
    fonte->loadFromFile("Design/fonte/sangue_escorrendo.ttf");
    opcoes = {"Zombies++", "Novo Jogo", "Continuar", "Ranking", "Sair"};
    coordenadas = {{130, 40}, {445, 700}, {445, 762}, {460, 823}, {480, 886}};
    tamanhos = {200, 22, 22, 22, 22};
    textos.resize(opcoes.size());
    for (std::size_t i = 0; i < textos.size(); ++i) {
        textos[i].setFont(*fonte); textos[i].setString(opcoes[i]);
        textos[i].setCharacterSize(tamanhos[i]); textos[i].setPosition(coordenadas[i]);
        textos[i].setOutlineColor(sf::Color::Black);
    }
    textos[0].setOutlineThickness(20);
    textos[1].setOutlineThickness(4);
}
void Menu_Principal::ao_entrar() { pGG->resetarCamera(); }
void Menu_Principal::fase_salva() {
    if (!std::filesystem::exists("partida.json")) {
        pGE->mensagem = "Nenhuma partida completa salva. Inicie um Novo Jogo.";
        return;
    }
    const auto dados = Persistencia::ler_json("partida.json");
    const int estado = Persistencia::inteiro(dados.at("estado"), 6, 9);
    std::unique_ptr<Fases::Fase> fase;
    if (estado < 8) fase = std::make_unique<Fases::Fase1>(estado, true);
    else fase = std::make_unique<Fases::Fase2>(estado, true);
    fase->restaurar(dados);
    if (fase->get_finalizada() && fase->get_nomes_confirmados()) {
        pGE->mensagem = "Esta partida terminou. Escolha Novo Jogo.";
        return;
    }
    const bool pendente = fase->get_finalizada();
    pGE->adicionar_estado(fase.get()); fase.release();
    pGE->set_fase(estado);
    pGE->set_estado_atual(pendente ? 10 : estado);
}
void Menu_Principal::selecionar() {
    if (pos == 1) pGE->set_estado_atual(1);
    else if (pos == 2) {
        auto* fase = dynamic_cast<Fases::Fase*>(pGE->get_estado(pGE->get_fase()));
        if (fase && !fase->get_finalizada()) pGE->set_estado_atual(pGE->get_fase());
        else if (fase && fase->get_finalizada() && !fase->get_nomes_confirmados()) pGE->set_estado_atual(10);
        else fase_salva();
    } else if (pos == 3) pGE->set_estado_atual(4);
    else if (pos == 4 && pGE->salvar_partida()) pGG->fecharJanela();
}
void Menu_Principal::loop_evento() {} // Eventos sao encaminhados pelo gerenciador.
}
