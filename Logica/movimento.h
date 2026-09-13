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
    unsigned tolerancia_salto=0, comando_salto=0;
    bool pulo_pressionado=false;
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
// Janelas de 100 ms, consumidas no salto; nao permitem um segundo salto no ar.
inline bool solicitar_salto(EstadoMovimento& e,bool chao,bool pressionado) {
    if(chao) e.tolerancia_salto=6;
    if(pressionado && !e.pulo_pressionado) e.comando_salto=6;
    e.pulo_pressionado=pressionado;
    const bool saltar=(chao && pressionado) || (e.comando_salto && e.tolerancia_salto);
    if(saltar) {e.comando_salto=0;e.tolerancia_salto=0;}
    else {if(e.comando_salto)--e.comando_salto;if(e.tolerancia_salto)--e.tolerancia_salto;}
    return saltar;
}
// Regra de movimento compartilhada pelo jogo e pelos testes sem SFML.
inline Velocidade mover(Velocidade v, bool chao, bool esquerda, bool direita, bool pular, bool descer, bool gelo = false) {
    using namespace Recursos::Configuracao;
    const int sentido=static_cast<int>(direita)-static_cast<int>(esquerda);
    v.x=std::clamp(v.x+sentido*(gelo && chao ? aceleracao_gelo : aceleracao_jogador),-velocidade_maxima_jogador,velocidade_maxima_jogador);
    if (!sentido) {
        const float freio = gelo && chao ? freio_gelo : freio_jogador;
        if(std::abs(v.x)<=freio) v.x=0;
        else v.x+=v.x>0?-freio:freio;
    }
    v.y=chao?0:v.y+gravidade;
    if(pular && chao) v.y=-impulso_salto;
    if(descer) v.y+=gravidade;
    return v;
}
}
