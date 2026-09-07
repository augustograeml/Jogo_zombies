#pragma once
#include <algorithm>
#include <cmath>
namespace Animacao {
enum class Pose { Repouso, Caminhada, Subida, Queda, Dano, Preparacao, Ataque, Recuperacao };
struct Deformacao { float x=1, y=1, angulo=0; };
inline Pose pose_jogador(float vx,float vy,unsigned reacao) {
    if(reacao) return Pose::Dano;
    if(vy<-.1f) return Pose::Subida;
    if(vy>.1f) return Pose::Queda;
    return vx!=0?Pose::Caminhada:Pose::Repouso;
}
// Apenas transformacao visual; parametros provem de estados salvos da simulacao.
inline Deformacao deformacao(Pose pose,float progresso=0) {
    const float t=std::clamp(progresso,0.f,1.f);
    switch(pose) {
    case Pose::Subida: return {1.08f,.88f,-5};
    case Pose::Queda: return {.92f,1.06f,3};
    case Pose::Dano: return {1.05f,.92f,-15*t};
    case Pose::Preparacao: return {1+.12f*t,1-.12f*t,-10*t};
    case Pose::Ataque: return {1.15f,.9f,22*std::sin(t*3.14159265f)};
    case Pose::Recuperacao: return {1+.08f*(1-t),1-.08f*(1-t),0};
    default: return {};
    }
}
}
