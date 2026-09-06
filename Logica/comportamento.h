#pragma once
#include <cmath>
#include <vector>
#include "../Recursos/configuracao.h"
namespace Logica {
enum class AcaoArqueiro { Patrulha, Preparando, Recarga };
struct Alvo { int id; float x, y; bool vivo; };
struct EstadoArqueiro {
    AcaoArqueiro acao = AcaoArqueiro::Patrulha;
    int alvo = -1;
    unsigned preparacao = 0;
    bool direita = true;
};
// Decisao pura, sem teclado, janela, ponteiros de entidade ou relogio real.
inline bool decidir_arqueiro(EstadoArqueiro& e, float x, float y, const std::vector<Alvo>& jogadores,
                            bool recarregando, float alcance = Recursos::Configuracao::alcance_arqueiro,
                            unsigned antecipacao = Recursos::Configuracao::preparacao_arqueiro) {
    const Alvo* escolhido = nullptr;
    float distancia = alcance * alcance;
    for (const auto& j : jogadores) {
        const float d = (j.x-x)*(j.x-x) + (j.y-y)*(j.y-y);
        if (j.vivo && std::abs(j.y-y) <= 120 && d <= distancia && (!escolhido || d < distancia)) {
            escolhido = &j; distancia = d;
        }
    }
    const int anterior = e.alvo;
    e.alvo = escolhido ? escolhido->id : -1;
    if (recarregando) { e.acao = AcaoArqueiro::Recarga; e.preparacao = 0; return false; }
    if (!escolhido) { e.acao = AcaoArqueiro::Patrulha; e.preparacao = 0; return false; }
    e.direita = escolhido->x >= x;
    if (e.acao != AcaoArqueiro::Preparando || anterior != e.alvo) e.preparacao = 0;
    e.acao = AcaoArqueiro::Preparando;
    if (++e.preparacao < antecipacao) return false;
    e.preparacao = 0; e.acao = AcaoArqueiro::Recarga;
    return true;
}
}
