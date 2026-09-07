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
    if(!textos.empty()) { auto titulo=textos[0]; pGG->get_Janela()->draw(titulo); }
    for(std::size_t i=1;i<textos.size();++i) {
        const auto area=area_opcao(i); const bool ativo=i==static_cast<std::size_t>(pos);
        sf::RectangleShape botao({area.width,area.height}); botao.setPosition(area.left,area.top);
        botao.setFillColor(ativo?sf::Color(255,211,100):sf::Color(16,27,43));
        botao.setOutlineColor(ativo?sf::Color::White:sf::Color(111,145,163));
        botao.setOutlineThickness(ativo?3:1); pGG->get_Janela()->draw(botao);
        sf::Text texto((ativo?"> ":"")+opcoes[i],Interface::fonte_legivel(),20);
        texto.setStyle(sf::Text::Bold); texto.setFillColor(ativo?sf::Color(16,27,43):sf::Color(239,245,248));
        const auto limite=texto.getLocalBounds();
        const float escala=std::min(Interface::Preferencias::instancia().escala,(area.width-20)/std::max(1.f,limite.width));
        texto.setScale(escala,escala); texto.setOrigin(limite.left+limite.width/2,limite.top+limite.height/2);
        texto.setPosition(area.left+area.width/2,area.top+area.height/2); pGG->get_Janela()->draw(texto);
    }
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
