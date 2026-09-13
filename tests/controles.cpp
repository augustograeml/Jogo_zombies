#include "../Interface/controles.h"
#include <filesystem>
#include <chrono>
#include <cassert>
int main() {
    auto pasta=std::filesystem::temp_directory_path()/("zombies-controles-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(pasta);std::filesystem::current_path(pasta);
    Interface::ConfiguracaoControles c;c.validar();
    bool rejeitou=false;try{c.atribuir(0,0,sf::Keyboard::D);}catch(...){rejeitou=true;}
    assert(rejeitou && c.teclas[0][0]==sf::Keyboard::A);
    c.atribuir(0,0,sf::Keyboard::J);auto l=Interface::ConfiguracaoControles::carregar();
    assert(l.teclas==c.teclas && l.personalizado[0]);
    auto a=Interface::combinar_controle({},24,0,false);assert(!a.esquerda && !a.direita);
    a=Interface::combinar_controle({},-80,80,true);assert(a.esquerda && a.descer && a.pular);
    Interface::MenuControles menu;sf::Event e{};e.type=sf::Event::KeyPressed;e.key.code=sf::Keyboard::Enter;
    assert(menu.tratar(e));e.key.code=sf::Keyboard::Escape;assert(menu.tratar(e));assert(!menu.tratar(e));
}
