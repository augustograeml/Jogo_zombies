#include "../Persistencia/slots.h"
#include "../Interface/painel.h"
#include "../Recursos/catalogo.h"
#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include <memory>

namespace Estados::Menus {
Menu_Principal::Menu_Principal(int id) : Menu(id), jacriado(false), pObserver(nullptr) { inicializa_valores(); }
Menu_Principal::~Menu_Principal() = default;
void Menu_Principal::inicializa_valores() {
    fonte_slots.loadFromFile(Recursos::caminho("Design/fonte/fonte_simas.ttf").string());
    atualizar_slots();
    imagem->loadFromFile(Recursos::caminho("Design/imagens/menu_zombies++.jpg").string());
    fonte->loadFromFile(Recursos::caminho("Design/fonte/sangue_escorrendo.ttf").string());
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
void Menu_Principal::ao_entrar() { pGG->resetarCamera(); atualizar_slots(); }
void Menu_Principal::atualizar_slots() {
    resumos.clear();
    for (const auto& r : Persistencia::Slots::instancia().listar()) {
        std::string t = "Slot " + std::to_string(r.numero) + ": ";
        if (r.valido) t += "Fase " + std::to_string(r.fase) + (r.jogadores==2?" Dupla ":" Solo ") +
            Interface::formatar_tempo(r.segundos) + (r.finalizada?" - encerrada":"");
        else t += r.existe ? "arquivo invalido" : "vazio";
        if (r.backup_disponivel) t += " [R: recuperar anterior]";
        resumos.push_back(t);
    }
}
void Menu_Principal::executar() {
    mostrar_menu();
    sf::RectangleShape fundo({980,150}); fundo.setPosition(22,500); fundo.setFillColor({8,15,23,235});
    pGG->get_Janela()->draw(fundo);
    for (std::size_t i=0;i<resumos.size();++i) {
        sf::Text t(resumos[i],fonte_slots,18); t.setPosition(35,535+30*i);
        t.setFillColor(static_cast<int>(i+1)==Persistencia::Slots::instancia().selecionado()?sf::Color(107,243,211):sf::Color::White);
        pGG->get_Janela()->draw(t);
    }
    sf::Text ajuda("1 / 2 / 3: escolher slot para Novo Jogo ou Continuar",fonte_slots,18);
    ajuda.setPosition(35,507); pGG->get_Janela()->draw(ajuda);
}
void Menu_Principal::tratar_evento(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code >= sf::Keyboard::Num1 && e.key.code <= sf::Keyboard::Num3) {
            const int slot=e.key.code-sf::Keyboard::Num1+1;
            if(slot!=Persistencia::Slots::instancia().selecionado()) {
                if(!pGE->salvar_partida()) return;
                Persistencia::Slots::instancia().selecionar(slot); pGE->set_fase(-1);
            }
            pGE->mensagem.clear(); atualizar_slots(); return;
        }
        if(e.key.code==sf::Keyboard::R) {
            Persistencia::Slots::instancia().recuperar(); pGE->set_fase(-1);
            pGE->mensagem="Checkpoint anterior recuperado. Escolha Continuar."; atualizar_slots(); return;
        }
    }
    Menu::tratar_evento(e);
}
void Menu_Principal::fase_salva() {
    if (!std::filesystem::exists(Persistencia::Slots::instancia().caminho_atual())) {
        pGE->mensagem = "Nenhuma partida completa salva. Inicie um Novo Jogo.";
        return;
    }
    const auto dados = Persistencia::Slots::instancia().ler();
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
    if (pos == 1) pGE->set_estado_atual(Estados::Tela::Jogadores);
    else if (pos == 2) {
        fase_salva();
    } else if (pos == 3) pGE->set_estado_atual(Estados::Tela::Ranking);
    else if (pos == 4 && pGE->salvar_partida()) pGG->fecharJanela();
}
void Menu_Principal::loop_evento() {} // Eventos sao encaminhados pelo gerenciador.
}
