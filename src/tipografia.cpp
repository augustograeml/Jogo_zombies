#include "../Interface/preferencias.h"
#include "../Recursos/catalogo.h"
#include <stdexcept>

namespace Interface {
namespace {
sf::Font carregar(const char* arquivo) {
    sf::Font fonte;
    if(!fonte.loadFromFile(Recursos::caminho(arquivo).string()))
        throw std::runtime_error(std::string("Fonte nao encontrada: ")+arquivo);
    return fonte;
}
}
const sf::Font& fonte_interface() {
    static const sf::Font fonte=carregar("Design/fonte/BarlowCondensed-Medium.ttf");
    return Preferencias::instancia().legivel?fonte_legivel():fonte;
}
const sf::Font& fonte_titulo() {
    static const sf::Font fonte=carregar("Design/fonte/Teko-Bold.otf");
    return Preferencias::instancia().legivel?fonte_legivel():fonte;
}
}
