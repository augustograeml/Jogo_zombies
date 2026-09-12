#include "../Recursos/catalogo.h"
#include "../Interface/preferencias.h"
#include "../Interface/tema.h"
#include "../Estados/Menus/nome.h"
#include "../Estados/Fases/fase2.h"
#include <memory>
namespace Estados::Menus {
Nome::Nome(int id) : Menu(id) { inicializa_valores(); }
void Nome::inicializa_valores() {
    imagem->loadFromFile(Recursos::caminho("Design/imagens/pegar_nome.png").string());
    fonte->loadFromFile(Recursos::caminho("Design/fonte/BarlowSemiCondensed-Medium.ttf").string());
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
    const auto destino=Estados::apos_resultado(fase->get_vitoria(),fase->get_numero_fase(),quantidade);
    if (destino==Estados::Tela::Fase2Dupla) {
        auto proxima = std::make_unique<Fases::Fase2>(9, false);
        proxima->continuar_sessao(fase->get_passos_sessao());
        proxima->salvar();
        pGE->adicionar_estado(proxima.get()); proxima.release();
        pGE->set_estado_atual(Estados::Tela::Fase2Dupla);
    } else {
        pGE->set_estado_atual(destino);
        if (!fase->get_vitoria()) pGE->mensagem = "Nome salvo. Pontos positivos disputam o ranking de pontos (tecla P).";
    }
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
        pGE->set_estado_atual(Estados::Tela::Principal); // Resultado pendente continua disponivel em Continuar.
    }
}
void Nome::executar() {
    pGG->resetarCamera(); pGG->desenharTextura(imagem);
    if(Interface::Preferencias::instancia().contraste) {
        sf::RectangleShape fundo({1024,1024}); fundo.setFillColor(sf::Color::Black); pGG->get_Janela()->draw(fundo);
    }
    auto* fase = dynamic_cast<Fases::Fase*>(pGE->get_estado(pGE->get_fase()));
    const bool venceu = fase && fase->get_vitoria();
    Interface::Tema::painel(*pGG->get_Janela(),{80,110,864,650});
    sf::Text titulo(venceu ? "FASE CONCLUIDA" : "FIM DE JOGO", Interface::fonte_titulo(), 64);
    Interface::Tema::estilo_titulo(titulo);
    titulo.setPosition(130, 160);
    sf::Text instrucao("Nome do jogador " + std::to_string(std::min(static_cast<int>(nomes.size()) + 1, quantidade)) +
                       " (Enter confirma)", Interface::fonte_interface(), 26);
    instrucao.setPosition(130, 320);
    Interface::Tema::painel(*pGG->get_Janela(),{130,460,760,75});
    sf::Text campo(entrada + "_", Interface::fonte_interface(), 32); campo.setPosition(150, 480);
    sf::Text resultado("Pontuacao da equipe: " + std::to_string(fase ? fase->get_pontos() : 0) +
        (venceu ? " | Tempo de conclusao registrado." : " | Pontos positivos entram no ranking."), Interface::fonte_interface(), 20);
    resultado.setPosition(130, 250);
    sf::Text ajuda("Esc: menu | Continuar recupera o resultado pendente", Interface::fonte_interface(), 20);
    ajuda.setPosition(130, 650);
    for (auto* texto : {&titulo, &instrucao, &campo, &ajuda, &resultado}) {
        Interface::aplicar_texto(*texto,770);
        texto->setFillColor(Interface::Tema::tinta());
    }
    ajuda.setFillColor(Interface::Preferencias::instancia().contraste?sf::Color::White:Interface::Tema::secundaria());
    pGG->get_Janela()->draw(titulo); pGG->get_Janela()->draw(instrucao);
    pGG->get_Janela()->draw(resultado);
    pGG->get_Janela()->draw(campo); pGG->get_Janela()->draw(ajuda);
}
}
