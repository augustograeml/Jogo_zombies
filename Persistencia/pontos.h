#pragma once
#include "arquivo.h"
#include <cstdint>
#include <vector>
namespace Persistencia {
struct ResultadoPontos {
    std::string id;
    int fase = 1, jogadores = 1;
    std::vector<std::string> nomes;
    std::int64_t pontos = 0;
    double segundos = 0;
    bool vitoria = false;
};
// Strategy: a consulta recebe a politica de classificacao, sem duplicar armazenamento.
class CriterioResultado {
public:
    virtual ~CriterioResultado() = default;
    virtual bool precede(const ResultadoPontos&, const ResultadoPontos&) const = 0;
};
class CriterioPontos final : public CriterioResultado {
public:
    bool precede(const ResultadoPontos& a, const ResultadoPontos& b) const override {
        return a.pontos != b.pontos ? a.pontos > b.pontos : a.segundos < b.segundos;
    }
};
class CriterioTempo final : public CriterioResultado {
public:
    bool precede(const ResultadoPontos& a, const ResultadoPontos& b) const override {
        return a.segundos < b.segundos;
    }
};
class RepositorioPontos {
    std::filesystem::path caminho;
    std::vector<ResultadoPontos> ler() const;
public:
    explicit RepositorioPontos(std::filesystem::path arquivo = "ranking-pontos.json") : caminho(std::move(arquivo)) {}
    std::vector<ResultadoPontos> consultar(int fase, int jogadores) const;
    std::vector<ResultadoPontos> consultar(int fase, int jogadores, const CriterioResultado& criterio) const;
    // Uma partida tem um resultado de equipe; IDs repetidos nao somam pontos novamente.
    void registrar(const ResultadoPontos& resultado);
};
namespace Pontuacao {
constexpr int zumbi = 100, arqueiro = 150, gigante = 300, coleta = 25, conclusao = 500;
constexpr std::int64_t maximo = 1000000000;
}
}
