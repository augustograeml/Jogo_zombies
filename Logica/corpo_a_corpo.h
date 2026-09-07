#pragma once
#include "comportamento.h"

namespace Logica {
enum class AcaoCorpo { Patrulha, Perseguindo, Preparando, Golpe, Recuperando };
struct EstadoCorpo {
    AcaoCorpo acao = AcaoCorpo::Patrulha;
    int alvo = -1;
    unsigned passos = 0;
    bool direita = true;
    float origem = 0;
    bool iniciado = false;
};
inline const Alvo* alvo_proximo(float x, float y, const std::vector<Alvo>& alvos, float alcance) {
    const Alvo* escolhido = nullptr;
    float menor = alcance;
    for (const auto& a : alvos) {
        const float d = std::abs(a.x-x);
        if(a.vivo && std::abs(a.y-y)<=90 && d<=menor && (!escolhido || d<menor)) {
            escolhido=&a; menor=d;
        }
    }
    return escolhido;
}
// O ponto de patrulha limita a perseguicao: o inimigo nao segue o jogador pelo mapa inteiro.
inline void decidir_zumbi(EstadoCorpo& e, float x, float y, const std::vector<Alvo>& alvos) {
    if(!e.iniciado) { e.origem=x; e.iniciado=true; }
    const auto* alvo=alvo_proximo(x,y,alvos,220);
    if(alvo && std::abs(alvo->x-e.origem)<=300) {
        e.acao=AcaoCorpo::Perseguindo; e.alvo=alvo->id; e.direita=alvo->x>=x;
    } else {
        e.acao=AcaoCorpo::Patrulha; e.alvo=-1;
        if(std::abs(x-e.origem)>100) e.direita=x<e.origem;
    }
}
// Preparacao imovel (0,7 s), golpe curto (0,2 s), recuperacao vulneravel (1 s).
// Durante o golpe, a direcao fica travada; afastar-se ou pular evita o contato.
inline void decidir_gigante(EstadoCorpo& e, float x, float y, const std::vector<Alvo>& alvos) {
    if(!e.iniciado) { e.origem=x; e.iniciado=true; }
    if(e.acao==AcaoCorpo::Recuperando) {
        e.alvo=-1;
        if(++e.passos>=60) { e.acao=AcaoCorpo::Patrulha; e.passos=0; }
        return;
    }
    if(e.acao==AcaoCorpo::Preparando || e.acao==AcaoCorpo::Golpe) {
        bool vivo=false;
        for(const auto& a:alvos) if(a.id==e.alvo && a.vivo) vivo=true;
        if(!vivo) { e.alvo=-1; e.acao=AcaoCorpo::Recuperando; e.passos=0; return; }
        if(++e.passos >= (e.acao==AcaoCorpo::Preparando?42u:12u)) {
            e.acao=e.acao==AcaoCorpo::Preparando?AcaoCorpo::Golpe:AcaoCorpo::Recuperando;
            e.passos=0;
            if(e.acao==AcaoCorpo::Recuperando) e.alvo=-1;
        }
        return;
    }
    const auto* alvo=alvo_proximo(x,y,alvos,100);
    if(alvo) {
        e.acao=AcaoCorpo::Preparando; e.alvo=alvo->id; e.passos=0; e.direita=alvo->x>=x;
    } else {
        e.alvo=-1;
        if(std::abs(x-e.origem)>100) e.direita=x<e.origem;
    }
}
}
