#include "../Estados/Menus/pause.h"
namespace Estados::Menus {
Gerenciadores::Gerenciador_Grafico* Pause::pGG = Gerenciadores::Gerenciador_Grafico::get_instancia();
Pause::Pause(int id) : Estado(id), fonte(new sf::Font), imagem(new sf::Texture) { inicializa_valores(); }
Pause::~Pause() { delete fonte; delete imagem; }
void Pause::inicializa_valores() {
    imagem->loadFromFile("Design/imagens/pause.png");
    fonte->loadFromFile("Design/fonte/sangue_escorrendo.ttf");
    opcoes = {"Pausa", "Continuar", "Menu"};
    coordenadas = {{340, 185}, {390, 462}, {470, 605}};
    tamanhos = {150, 50, 50};
    textos.resize(3); pos = 1;
    for (std::size_t i = 0; i < textos.size(); ++i) {
        textos[i].setFont(*fonte); textos[i].setString(opcoes[i]);
        textos[i].setCharacterSize(tamanhos[i]); textos[i].setPosition(coordenadas[i]);
        textos[i].setOutlineColor(sf::Color::Black);
    }
    textos[0].setOutlineThickness(20); textos[1].setOutlineThickness(4);
}
void Pause::tratar_evento(const sf::Event& evento) {
    if (evento.type != sf::Event::KeyPressed) return;
    if (evento.key.code == sf::Keyboard::S) { pGE->salvar_partida(); return; }
    if (evento.key.code == sf::Keyboard::Up || evento.key.code == sf::Keyboard::Down) {
        textos[pos].setOutlineThickness(0); pos = pos == 1 ? 2 : 1; textos[pos].setOutlineThickness(4);
    } else if (evento.key.code == sf::Keyboard::Escape || evento.key.code == sf::Keyboard::Enter) {
        pGE->mensagem.clear();
        pGE->set_estado_atual(evento.key.code == sf::Keyboard::Escape || pos == 1 ? pGE->get_fase() : 0);
    }
}
void Pause::mostrar_menu() {
    pGG->resetarCamera(); pGG->desenharTextura(imagem);
    for (const auto& texto : textos) pGG->get_Janela()->draw(texto);
}
void Pause::executar() { mostrar_menu(); }
void Pause::loop_evento() {}
}
