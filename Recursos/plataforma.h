#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
namespace Listas { class ListaEntidade; }

namespace Recursos {
// Mascara visual: bits indicam vizinhos acima, direita, abaixo e esquerda.
// Recalculada ao criar/carregar a fase; nao integra o estado fisico ou o save.
void encaixar_plataformas(Listas::ListaEntidade& blocos);
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
