#pragma once
namespace Estados {
// Valores persistidos: nunca renumerar entradas existentes.
enum class Tela : int { Principal=0, Jogadores=1, FasesSolo=2, FasesDupla=3, Ranking=4,
    Pausa=5, Fase1Solo=6, Fase1Dupla=7, Fase2Solo=8, Fase2Dupla=9, Nome=10 };
constexpr int codigo(Tela tela) { return static_cast<int>(tela); }
constexpr bool eh_fase(int id) { return id >= codigo(Tela::Fase1Solo) && id <= codigo(Tela::Fase2Dupla); }
constexpr Tela apos_resultado(bool vitoria, int fase, int jogadores) {
    return vitoria && fase==1 && jogadores==2 ? Tela::Fase2Dupla : vitoria ? Tela::Ranking : Tela::Principal;
}
}
