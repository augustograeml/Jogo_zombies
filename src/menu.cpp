#include "../Estados/Menus/menu.h"
#include "../Interface/preferencias.h"
namespace Estados::Menus {
Gerenciadores::Gerenciador_Grafico* Menu::pGG = Gerenciadores::Gerenciador_Grafico::get_instancia();
Menu::Menu(int id) : Estado(id), pos(1), selecionado(false), deselecionado(false), jogador2(false),
                    fonte(new sf::Font), imagem(new sf::Texture) {}
Menu::~Menu() { delete fonte; delete imagem; }
void Menu::mostrar_menu() {
    pGG->resetarCamera();
    pGG->desenharTextura(imagem);
    if(Interface::Preferencias::instancia().contraste) { sf::RectangleShape fundo({1024,1024}); fundo.setFillColor(sf::Color::Black); pGG->get_Janela()->draw(fundo); }
    for (std::size_t i=0;i<textos.size();++i) {
        auto texto=textos[i];
        if(i==static_cast<std::size_t>(pos)) texto.setString(sf::String("> ")+texto.getString());
        Interface::aplicar_texto(texto,1000-texto.getPosition().x);
        pGG->get_Janela()->draw(texto); }
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
