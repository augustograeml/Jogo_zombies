#pragma once
#include <algorithm>
#include <cstdint>
namespace Logica {
struct ResultadoPartida {
    bool finalizada=false, vitoria=false, ranking_registrado=false, nomes_confirmados=false;
};
struct Sessao {
    std::uint64_t passos=0, passos_anteriores=0;
    double acumulador=0;
    static constexpr double intervalo=1.0/60.0;
    void acumular(double segundos) { acumulador += std::clamp(segundos, 0.0, 0.25); }
    bool proximo() {
        if(acumulador < intervalo) return false;
        acumulador -= intervalo; return true;
    }
    double tempo() const { return static_cast<double>(passos)/60.0; }
    double tempo_total() const { return static_cast<double>(passos+passos_anteriores)/60.0; }
};
}
