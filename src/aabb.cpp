#include "../Fisica/aabb.h"
#include <algorithm>
#include <cmath>
namespace Fisica {
Contato resolver(Caixa a, Caixa b, float vy) {
    const float horizontal = std::min(a.x+a.largura,b.x+b.largura)-std::max(a.x,b.x);
    if (horizontal <= 0) return {};
    const float fundo = a.y+a.altura;
    if (vy >= 0 && std::abs(fundo-b.y) <= .02f) return {4,0,b.y-fundo};
    const float vertical = std::min(fundo,b.y+b.altura)-std::max(a.y,b.y);
    if (vertical <= 0) return {};
    const float esquerda = b.x-a.x-a.largura, direita = b.x+b.largura-a.x;
    const float cima = b.y-fundo, baixo = b.y+b.altura-a.y;
    const float dx = std::abs(esquerda)<std::abs(direita)?esquerda:direita;
    const float dy = std::abs(cima)<std::abs(baixo)?cima:baixo;
    if (std::abs(dy)<=std::abs(dx)) return {dy<0?4:2,0,dy};
    return {dx<0?3:1,dx,0};
}
}
