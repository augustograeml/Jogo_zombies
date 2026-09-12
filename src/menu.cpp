#include "../Estados/Menus/menu.h"
#include "../Interface/preferencias.h"
#include "../Interface/tema.h"
namespace Estados::Menus {
Gerenciadores::Gerenciador_Grafico* Menu::pGG = Gerenciadores::Gerenciador_Grafico::get_instancia();
Menu::Menu(int id) : Estado(id), pos(1), selecionado(false), deselecionado(false), jogador2(false),
                    fonte(new sf::Font), imagem(new sf::Texture) {}
Menu::~Menu() { delete fonte; delete imagem; }
void Menu::mostrar_menu() {
    pGG->resetarCamera();
    pGG->desenharTextura(imagem);
    if(Interface::Preferencias::instancia().contraste) { sf::RectangleShape fundo({1024,1024}); fundo.setFillColor(sf::Color::Black); pGG->get_Janela()->draw(fundo); }
    auto& janela=*pGG->get_Janela();
    Interface::Tema::titulo(janela,id==0?"ZOMBIES++":id==1?"JOGADORES":"ESCOLHA A FASE",110,id==0?132:76);
    for(std::size_t i=1;i<textos.size();++i)
        Interface::Tema::botao(janela,area_opcao(i),opcoes[i],i==static_cast<std::size_t>(pos));
}

sf::FloatRect Menu::area_opcao(std::size_t i) const {
    if(id==0) return {400,690+62.f*(i-1),224,46};
    if(i==1) return {310,600,170,46};
    if(i==2) return {550,600,170,46};
    return {427,666,170,46};
}

bool Menu::get_jogador2() const { return jogador2; }
void Menu::set_jogador2(bool valor) { jogador2 = valor; }
void Menu::executar() { mostrar_menu(); }
void Menu::baixo(int limite) {
    if (pos < limite) { textos[pos++].setOutlineThickness(0); textos[pos].setOutlineThickness(4); }
}
void Menu::cima() {
    if (pos > 1) { textos[pos--].setOutlineThickness(0); textos[pos].setOutlineThickness(4); }
}
void Menu::tratar_evento(const sf::Event& evento) {
    if (evento.type != sf::Event::KeyPressed) return;
    const auto tecla = evento.key.code;
    if (tecla == sf::Keyboard::Down || tecla == sf::Keyboard::Right) baixo(static_cast<int>(textos.size()) - 1);
    else if (tecla == sf::Keyboard::Up || tecla == sf::Keyboard::Left) cima();
    else if (tecla == sf::Keyboard::Enter) { pGE->mensagem.clear(); selecionar(); }
    else if (tecla == sf::Keyboard::Escape) {
        if (id == 0) { if (pGE->salvar_partida()) pGG->fecharJanela(); }
        else pGE->set_estado_atual(id == 2 || id == 3 ? 1 : 0);
    }
}
}
