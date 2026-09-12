#include "../Persistencia/slots.h"
#include "../Interface/painel.h"
#include "../Interface/preferencias.h"
#include "../Interface/tema.h"
#include "../Recursos/catalogo.h"
#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include <memory>

namespace Estados::Menus {
Menu_Principal::Menu_Principal(int id) : Menu(id), jacriado(false), pObserver(nullptr) { inicializa_valores(); }
Menu_Principal::~Menu_Principal() = default;
void Menu_Principal::inicializa_valores() {
    fonte_slots.loadFromFile(Recursos::caminho("Design/fonte/BarlowCondensed-Medium.ttf").string());
    imagem->loadFromFile(Recursos::caminho("Design/imagens/menu_zombies++.png").string());
    fonte->loadFromFile(Recursos::caminho("Design/fonte/Teko-Bold.otf").string());
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
void Menu_Principal::ao_entrar() { pGG->resetarCamera(); escolhendo_partida=false; escolhendo_destino=false; }
void Menu_Principal::atualizar_slots() {
    resumos.clear();
    for (const auto& r : Persistencia::Slots::instancia().listar()) {
        std::string t = "Slot " + std::to_string(r.numero) + ": ";
        if (r.valido) t += "Fase " + std::to_string(r.fase) + (r.jogadores==2?" Dupla ":" Solo ") +
            Interface::formatar_tempo(r.segundos) + (r.finalizada?" - encerrada":"");
        else t += r.existe ? "arquivo invalido" : "vazio";
        if (r.backup_disponivel) t += escolhendo_destino ? " [copia anterior]" : " [R: recuperar anterior]";
        resumos.push_back(t);
    }
}
void Menu_Principal::executar() {
    if (!escolhendo_partida && !escolhendo_destino) { mostrar_menu(); return; }
    pGG->resetarCamera(); pGG->desenharTextura(imagem);
    auto* janela=pGG->get_Janela();
    sf::RectangleShape fundo({920,580}); fundo.setPosition(52,225); fundo.setFillColor(Interface::Preferencias::instancia().contraste?sf::Color::Black:Interface::Tema::fundo()); janela->draw(fundo);
    sf::Text titulo(escolhendo_destino?"NOVO JOGO / DESTINO":"CONTINUAR JOGO",Interface::fonte_titulo(),52); titulo.setPosition(95,250); Interface::Tema::estilo_titulo(titulo); Interface::aplicar_texto(titulo,820); janela->draw(titulo);
    for (std::size_t i=0;i<resumos.size();++i) {
        const bool atual=static_cast<int>(i+1)==(escolhendo_destino?destino_novo:Persistencia::Slots::instancia().selecionado());
        sf::RectangleShape cartao({840,82}); cartao.setPosition(92,350+100*i);
        cartao.setFillColor(atual?Interface::Tema::acento():Interface::Tema::fundo()); janela->draw(cartao);
        sf::Text t((atual?"> ":"  ")+resumos[i],fonte_slots,18); t.setPosition(110,376+100*i);
        t.setFillColor(atual?sf::Color(16,23,21):Interface::Tema::tinta());
        Interface::aplicar_texto(t,800);
        if(atual) t.setFillColor(sf::Color(16,23,21));
        janela->draw(t);
    }
    const std::string instrucoes = escolhendo_destino
        ? "Setas ou 1/2/3: escolher | Enter ou clique: usar este slot\nA partida anterior fica na copia de recuperacao | Esc: voltar"
        : "Setas ou 1/2/3: escolher | Enter: continuar\nR: recuperar anterior | Esc: voltar";
    sf::Text ajuda(instrucoes,fonte_slots,18);
    ajuda.setPosition(95,690); Interface::aplicar_texto(ajuda,820); janela->draw(ajuda);
}
void Menu_Principal::escolher_slot(int numero) {
    if(numero!=Persistencia::Slots::instancia().selecionado()) {
        if(!pGE->salvar_partida()) return;
        Persistencia::Slots::instancia().selecionar(numero); pGE->set_fase(-1);
    }
    pGE->mensagem.clear();
}
void Menu_Principal::tratar_evento(const sf::Event& e) {
    if (!escolhendo_partida && !escolhendo_destino) {
        if(e.type==sf::Event::MouseButtonReleased && e.mouseButton.button==sf::Mouse::Left) {
            const auto ponto=pGG->get_Janela()->mapPixelToCoords({e.mouseButton.x,e.mouseButton.y},pGG->get_view_interface());
            for(std::size_t i=1;i<textos.size();++i) {
                if(area_opcao(i).contains(ponto)) {
                textos[pos].setOutlineThickness(0); pos=static_cast<int>(i); textos[pos].setOutlineThickness(4);
                pGE->mensagem.clear(); selecionar(); return;
                }
            }
        }
        Menu::tratar_evento(e); return;
    }
    if (escolhendo_destino) {
        if (e.type==sf::Event::MouseButtonReleased && e.mouseButton.button==sf::Mouse::Left) {
            const auto ponto=pGG->get_Janela()->mapPixelToCoords({e.mouseButton.x,e.mouseButton.y},pGG->get_view_interface());
            for (int i=0;i<3;++i) if (sf::FloatRect(92,350+100*i,840,82).contains(ponto)) {
                destino_novo=i+1; iniciar_novo(); return;
            }
        }
        if (e.type!=sf::Event::KeyPressed) return;
        if (e.key.code==sf::Keyboard::Escape) {
            escolhendo_destino=false;
            pGE->mensagem.clear(); return;
        }
        if (e.key.code==sf::Keyboard::Enter) { iniciar_novo(); return; }
        int numero=destino_novo;
        if (e.key.code>=sf::Keyboard::Num1 && e.key.code<=sf::Keyboard::Num3) numero=e.key.code-sf::Keyboard::Num1+1;
        else if (e.key.code==sf::Keyboard::Up) numero=numero==1?3:numero-1;
        else if (e.key.code==sf::Keyboard::Down) numero=numero==3?1:numero+1;
        if (numero!=destino_novo) { destino_novo=numero; pGE->mensagem.clear(); }
        return;
    }
    if(e.type==sf::Event::MouseButtonReleased && e.mouseButton.button==sf::Mouse::Left) {
        const auto ponto=pGG->get_Janela()->mapPixelToCoords({e.mouseButton.x,e.mouseButton.y},pGG->get_view_interface());
        for(int i=0;i<3;++i) if(sf::FloatRect(92,350+100*i,840,82).contains(ponto)) {
            escolher_slot(i+1); if(Persistencia::Slots::instancia().selecionado()==i+1) fase_salva(); return;
        }
    }
    if (e.type != sf::Event::KeyPressed) return;
    const auto tecla=e.key.code;
    if(tecla==sf::Keyboard::Escape) { escolhendo_partida=false; pGE->mensagem.clear(); return; }
    if(tecla==sf::Keyboard::Enter) { pGE->mensagem.clear(); fase_salva(); return; }
    if(tecla>=sf::Keyboard::Num1 && tecla<=sf::Keyboard::Num3) escolher_slot(tecla-sf::Keyboard::Num1+1);
    else if(tecla==sf::Keyboard::Up || tecla==sf::Keyboard::Down) {
        int numero=Persistencia::Slots::instancia().selecionado()+(tecla==sf::Keyboard::Up?-1:1);
        escolher_slot(numero<1?3:numero>3?1:numero);
    } else if(tecla==sf::Keyboard::R) {
        Persistencia::Slots::instancia().recuperar(); pGE->set_fase(-1);
        pGE->mensagem="Checkpoint anterior recuperado. Enter para continuar."; atualizar_slots();
    }
}
void Menu_Principal::iniciar_novo() {
    auto& slots=Persistencia::Slots::instancia();
    const auto resumo=slots.listar().at(destino_novo-1);
    if (resumo.existe && !resumo.valido) {
        pGE->mensagem="Slot invalido. Recupere-o em Continuar ou escolha outro destino.";
        return;
    }
    // A escolha nao grava nem remove arquivos. O primeiro save preserva a partida anterior em .bak.
    if (!pGE->salvar_partida()) return;
    slots.selecionar(destino_novo); pGE->set_fase(-1);
    pGE->mensagem.clear(); escolhendo_destino=false;
    pGE->set_estado_atual(Estados::Tela::Jogadores);
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
    if (pos == 1) {
        if (!pGE->salvar_partida()) return;
        auto& slots=Persistencia::Slots::instancia();
        destino_novo=slots.selecionado();
        for (const auto& resumo:slots.listar()) if (!resumo.existe && !resumo.backup_disponivel) { destino_novo=resumo.numero; break; }
        escolhendo_destino=true; atualizar_slots(); pGE->mensagem.clear();
    }
    else if (pos == 2) {
        if (!pGE->salvar_partida()) return;
        pGE->mensagem.clear(); atualizar_slots(); escolhendo_partida=true;
    } else if (pos == 3) pGE->set_estado_atual(Estados::Tela::Ranking);
    else if (pos == 4 && pGE->salvar_partida()) pGG->fecharJanela();
}
void Menu_Principal::loop_evento() {} // Eventos sao encaminhados pelo gerenciador.
}
