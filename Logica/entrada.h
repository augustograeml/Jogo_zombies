#pragma once
namespace Logica {
struct Teclas {
    bool a=false,d=false,w=false,s=false;
    bool esquerda=false,direita=false,cima=false,baixo=false,espaco=false;
};
struct Controle { bool esquerda, direita, pular, descer; };
inline Controle controles(const Teclas& t, bool segundo, bool solo) {
    if(segundo) return {t.esquerda,t.direita,t.cima,t.baixo};
    return {t.a || (solo && t.esquerda), t.d || (solo && t.direita),
            t.w || (solo && (t.cima || t.espaco)), t.s || (solo && t.baixo)};
}
}
