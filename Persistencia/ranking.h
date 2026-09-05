#pragma once
#include "arquivo.h"
#include <vector>
namespace Persistencia {
struct Resultado {
    std::string id;
    int fase;
    int jogadores;
    std::vector<std::string> nomes;
    double segundos;
};
class RepositorioRanking {
    std::filesystem::path caminho, legados;
    std::vector<Resultado> ler() const;
public:
    explicit RepositorioRanking(std::filesystem::path arquivo = "ranking.json",
                               std::filesystem::path pastaLegada = "Design/imagens")
        : caminho(std::move(arquivo)), legados(std::move(pastaLegada)) {}
    std::vector<Resultado> consultar(int fase, int jogadores) const;
    // Id da partida torna a confirmacao idempotente, inclusive depois de recarregar.
    void registrar(const Resultado& resultado);
};
}
