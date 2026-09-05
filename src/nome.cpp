#include "../Estados/Menus/nome.h"
#include "../Estados/Fases/fase2.h"
#include <memory>
namespace Estados::Menus {
Nome::Nome(int id) : Menu(id) { inicializa_valores(); }
void Nome::inicializa_valores() {
    imagem->loadFromFile("Design/imagens/pegar_nome.png");
    fonte->loadFromFile("Design/fonte/fonte_simas.ttf");
}
void Nome::ao_entrar() {
    entrada.clear(); nomes.clear();
    auto* fase = dynamic_cast<Fases::Fase*>(pGE->get_estado(pGE->get_fase()));
    quantidade = fase ? fase->get_num_jogadores() : 1;
}
void Nome::selecionar() {
    auto* fase = dynamic_cast<Fases::Fase*>(pGE->get_estado(pGE->get_fase()));
    if (!fase) return;
    if (nomes.size() < static_cast<std::size_t>(quantidade)) {
        const auto utf8 = entrada.toUtf8();
        nomes.push_back(Persistencia::nome_valido(std::string(utf8.begin(), utf8.end())));
        entrada.clear();
    }
    if (nomes.size() != static_cast<std::size_t>(quantidade)) return;
    fase->registrar_resultado(nomes);
    pGE->mensagem.clear();
    // Preserva o avanco da fase 1 para a fase 2 no modo cooperativo original.
    if (fase->get_numero_fase() == 1 && quantidade == 2) {
        auto proxima = std::make_unique<Fases::Fase2>(9, false);
        proxima->salvar();
        pGE->adicionar_estado(proxima.get()); proxima.release();
        pGE->set_estado_atual(9);
    } else pGE->set_estado_atual(4);
}
void Nome::tratar_evento(const sf::Event& evento) {
    if (evento.type == sf::Event::TextEntered) {
        const auto caractere = evento.text.unicode;
        if (caractere == 8 && !entrada.isEmpty()) entrada.erase(entrada.getSize() - 1);
        else if (caractere >= 32 && caractere != 127 && caractere <= 0x10ffff &&
                 !(caractere >= 0xd800 && caractere <= 0xdfff) && entrada.getSize() < 20)
            entrada += caractere;
    } else if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Enter) {
        selecionar();
    } else if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Escape) {
        pGE->set_estado_atual(0); // Resultado pendente continua disponivel em Continuar.
    }
}
void Nome::executar() {
    pGG->resetarCamera(); pGG->desenharTextura(imagem);
    sf::Text titulo("Fase concluida!", *fonte, 45);
    titulo.setPosition(130, 160);
    sf::Text instrucao("Nome do jogador " + std::to_string(std::min(static_cast<int>(nomes.size()) + 1, quantidade)) +
                       " (Enter confirma)", *fonte, 26);
    instrucao.setPosition(130, 320);
    sf::Text campo(entrada + "_", *fonte, 32); campo.setPosition(150, 480);
    sf::Text ajuda("Esc: menu | Continuar recupera o resultado pendente", *fonte, 20);
    ajuda.setPosition(130, 650);
    for (auto* texto : {&instrucao, &campo, &ajuda}) {
        const float largura = texto->getLocalBounds().width;
        if (largura > 770) texto->setScale(770 / largura, 770 / largura);
        texto->setOutlineColor(sf::Color::Black); texto->setOutlineThickness(2);
    }
    pGG->get_Janela()->draw(titulo); pGG->get_Janela()->draw(instrucao);
    pGG->get_Janela()->draw(campo); pGG->get_Janela()->draw(ajuda);
}
}
