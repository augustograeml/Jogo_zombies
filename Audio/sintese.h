#pragma once
#include "../Logica/eventos.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
namespace Audio {
// Efeitos distintos e deterministas; este gerador nao usa o RNG da partida.
inline std::vector<std::int16_t> sintetizar(Logica::Evento evento, unsigned taxa=22050) {
    const auto tipo=static_cast<unsigned>(evento);
    constexpr double duracoes[]={.16,.11,.24,.30,.32,.80,.12,.90};
    const double duracao=duracoes[std::min(tipo,7u)];
    const auto tamanho=static_cast<unsigned>(taxa*duracao);
    std::vector<std::int16_t> amostras(tamanho);
    std::uint32_t ruido=173;
    double fase=0;
    for(unsigned i=0;i<tamanho;++i) {
        const double t=static_cast<double>(i)/taxa, progresso=t/duracao;
        ruido=1664525u*ruido+1013904223u;
        const double branco=static_cast<double>(ruido&65535)/32767.5-1.;
        double frequencia=440, onda=0;
        switch(evento) {
            case Logica::Evento::Salto: frequencia=240+650*progresso; break;
            case Logica::Evento::Disparo: frequencia=1500-1100*progresso; break;
            case Logica::Evento::Dano: frequencia=150-80*progresso; break;
            case Logica::Evento::Coleta: frequencia=progresso<.5?1046.5:1568; break;
            case Logica::Evento::InimigoDerrotado: frequencia=250-200*progresso; break;
            case Logica::Evento::Vitoria: {
                constexpr double notas[]={523.25,659.25,783.99,1046.5};
                frequencia=notas[std::min(3u,static_cast<unsigned>(progresso*4))]; break;
            }
            case Logica::Evento::ImpactoInimigo: frequencia=200*std::exp(-18*t)+45; break;
            case Logica::Evento::Derrota: {
                constexpr double notas[]={392,311.13,261.63};
                frequencia=notas[std::min(2u,static_cast<unsigned>(progresso*3))]; break;
            }
        }
        fase+=6.28318530718*frequencia/taxa;
        onda=std::sin(fase);
        if(evento==Logica::Evento::Disparo) onda=.2*onda+.8*branco;
        if(evento==Logica::Evento::ImpactoInimigo) onda=.8*onda+.2*branco;
        if(evento==Logica::Evento::InimigoDerrotado) onda=.4*onda+.6*branco;
        if(evento==Logica::Evento::Dano) onda=onda>0?.65:-.65;
        if(evento==Logica::Evento::Derrota) onda=.6*onda+.4*std::sin(fase*.75);
        const double entrada=std::min(1.,t/.004);
        const double saida=std::min(1.,(duracao-t)/.02);
        double envelope=entrada*saida*(1.-.45*progresso);
        if(evento==Logica::Evento::ImpactoInimigo) envelope*=std::exp(-22*t);
        amostras[i]=static_cast<std::int16_t>(9500*envelope*onda);
    }
    return amostras;
}
}
