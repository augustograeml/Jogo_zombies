#pragma once
#include <cmath>
#include <string>
#include <vector>
namespace Logica {
struct PosicaoObjetivo { float x,y; };
inline std::string orientar(PosicaoObjetivo jogador,const std::vector<PosicaoObjetivo>& inimigos) {
    if(inimigos.empty()) return "Area limpa";
    if(inimigos.size()>3) return "Explore e elimine os inimigos";
    auto alvo=inimigos.front();float menor=1e30f;
    for(auto p:inimigos) { const float d=std::hypot(p.x-jogador.x,p.y-jogador.y);if(d<menor){menor=d;alvo=p;} }
    std::string direcao=std::abs(alvo.x-jogador.x)<80?"Perto":alvo.x>jogador.x?"Direita":"Esquerda";
    if(std::abs(alvo.y-jogador.y)>100) direcao+=alvo.y>jogador.y?" / abaixo":" / acima";
    return direcao;
}
}
