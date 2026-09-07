#pragma once
#include <SFML/Graphics.hpp>
namespace Interface {
struct Preferencias {
    float escala=1;
    bool legivel=false, contraste=false;
    static Preferencias& instancia();
    void salvar() const;
};
const sf::Font& fonte_legivel();
// Ajusta texto sem alterar coordenadas de navegacao e limita sua largura.
void aplicar_texto(sf::Text& texto, float largura_maxima);
void configurar(int opcao);
}
