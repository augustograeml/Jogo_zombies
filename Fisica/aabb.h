#pragma once
namespace Fisica {
struct Caixa { float x, y, largura, altura; };
struct Contato { int lado = 0; float dx = 0, dy = 0; };
// Coordenadas representam o canto superior esquerdo; apoio inclui bordas tocando.
Contato resolver(Caixa movel, Caixa fixa, float velocidade_y);
}
