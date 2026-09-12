#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

namespace Recursos {
// Tres trechos contiguos, seguidos pelo espelho da faixa inteira: as pontas
// repetidas se encontram sem salto. A escolha e espacial, sem usar o RNG do jogo.
inline sf::IntRect trecho_plataforma(sf::Vector2f posicao) {
    const int coluna=static_cast<int>(std::floor(posicao.x/50.f));
    const int linha=static_cast<int>(std::floor(posicao.y/50.f));
    const int indice=((coluna+linha*2)%6+6)%6;
    return indice<3?sf::IntRect(indice*256,0,256,256)
                   :sf::IntRect((6-indice)*256,0,-256,256);
}
}
