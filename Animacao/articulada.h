#pragma once
#include "corrida.h"
#include <SFML/Graphics.hpp>
namespace Animacao {
// Malha continua: pesos de membros preservam costuras, sem recortar a imagem original.
void desenhar_articulada(sf::RenderTarget& alvo, const sf::Texture& textura,
                        sf::FloatRect caixa, const Estado& estado, sf::Color cor);
}
