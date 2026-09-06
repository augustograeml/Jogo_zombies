#pragma once
#include "../Recursos/configuracao.h"
#include <algorithm>
#include <cmath>
namespace Logica {
struct Velocidade { float x, y; };
struct EstadoMovimento {
    float origem_queda = 0;
    bool queda_ativa = false;
    bool gelo = false;
};
inline void acompanhar_queda(EstadoMovimento& estado, float y, bool apoiado) {
    if (apoiado || !estado.queda_ativa) estado.origem_queda = y;
    else estado.origem_queda = std::min(estado.origem_queda, y);
    estado.queda_ativa = !apoiado;
}
inline int dano_queda(EstadoMovimento& estado, float y, int base) {
    const float altura = estado.queda_ativa ? y - estado.origem_queda : 0.f;
    const int multiplicador = altura > 500.f ? 3 : altura > 300.f ? 2 : 1;
    estado.origem_queda = y; estado.queda_ativa = false;
    return base * multiplicador;
}
// Regra de movimento compartilhada pelo jogo e pelos testes sem SFML.
inline Velocidade mover(Velocidade v, bool chao, bool esquerda, bool direita, bool pular, bool descer, bool gelo = false) {
    using namespace Recursos::Configuracao;
    const int sentido=static_cast<int>(direita)-static_cast<int>(esquerda);
    v.x=std::clamp(v.x+sentido*aceleracao_jogador,-velocidade_maxima_jogador,velocidade_maxima_jogador);
    if (!sentido) {
        const float freio = gelo && chao ? .035f : freio_jogador;
        if(std::abs(v.x)<=freio) v.x=0;
        else v.x+=v.x>0?-freio:freio;
    }
    v.y=chao?0:v.y+gravidade;
    if(pular && chao) v.y=-impulso_salto;
    if(descer) v.y+=gravidade;
    return v;
}
}
