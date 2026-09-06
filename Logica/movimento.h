#pragma once
#include "../Recursos/configuracao.h"
#include <algorithm>
#include <cmath>
namespace Logica {
struct Velocidade { float x, y; };
// Regra de movimento compartilhada pelo jogo e pelos testes sem SFML.
inline Velocidade mover(Velocidade v, bool chao, bool esquerda, bool direita, bool pular, bool descer) {
    using namespace Recursos::Configuracao;
    const int sentido=static_cast<int>(direita)-static_cast<int>(esquerda);
    v.x=std::clamp(v.x+sentido*aceleracao_jogador,-velocidade_maxima_jogador,velocidade_maxima_jogador);
    if (!sentido) {
        if(std::abs(v.x)<=freio_jogador) v.x=0;
        else v.x+=v.x>0?-freio_jogador:freio_jogador;
    }
    v.y=chao?0:v.y+gravidade;
    if(pular && chao) v.y=-impulso_salto;
    if(descer) v.y+=gravidade;
    return v;
}
}
