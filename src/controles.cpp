#include "../Interface/controles.h"
#include <SFML/Window/Keyboard.hpp>
namespace Interface {
Logica::Comandos ler_controles(bool solo) {
    using K=sf::Keyboard;
    const Logica::Teclas t{K::isKeyPressed(K::A),K::isKeyPressed(K::D),K::isKeyPressed(K::W),K::isKeyPressed(K::S),
        K::isKeyPressed(K::Left),K::isKeyPressed(K::Right),K::isKeyPressed(K::Up),K::isKeyPressed(K::Down),K::isKeyPressed(K::Space)};
    return {Logica::controles(t,false,solo),Logica::controles(t,true,solo)};
}
}
