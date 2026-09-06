#include "../Persistencia/pontos.h"
#include "../Recursos/catalogo.h"
#include "../Persistencia/ranking.h"
#include <algorithm>
#include <fstream>
#include <cmath>

namespace Persistencia {
namespace {
void validar(const Resultado& r) {
    if (r.id.empty() || r.id.size() > 128 || r.fase < 1 || r.fase > 2 ||
        r.jogadores < 1 || r.jogadores > 2 || r.nomes.size() != static_cast<std::size_t>(r.jogadores) ||
        !std::isfinite(r.segundos) || r.segundos <= 0 || r.segundos > 1e12)
        throw std::runtime_error("Resultado de ranking invalido.");
    for (const auto& nome : r.nomes) nome_valido(nome);
}
}
std::vector<Resultado> RepositorioRanking::ler() const {
    std::vector<Resultado> resultados;
    if (std::filesystem::exists(caminho)) {
        const auto dados = ler_json(caminho);
        if (inteiro(dados.at("versao"), 1, 1) != 1) throw std::runtime_error("Ranking incompativel.");
        const auto& lista = dados.at("resultados");
        if (!lista.is_array() || lista.size() > 10000) throw std::runtime_error("Ranking muito grande.");
        for (const auto& j : lista) {
            Resultado r{j.at("id").get<std::string>(), inteiro(j.at("fase"), 1, 2),
                        inteiro(j.at("jogadores"), 1, 2), j.at("nomes").get<std::vector<std::string>>(),
                        numero(j.at("segundos"), 0, 1e12)};
            validar(r);
            resultados.push_back(std::move(r));
        }
    } else {
        // O formato anterior nao informa o modo: seus registros entram no solo.
        for (int fase = 1; fase <= 2; ++fase) {
            auto pasta = legados;
            if (legados == std::filesystem::path("Design/imagens")) pasta=Recursos::caminho("Design/imagens");
            std::ifstream arquivo(pasta / ("rankingfase" + std::to_string(fase) + ".txt"));
            std::string linha;
            std::size_t indice = 0;
            while (std::getline(arquivo, linha)) {
                ++indice;
                if (indice > 10000) throw std::runtime_error("Ranking legado muito grande.");
                try {
                    const auto separador = linha.rfind(".....");
                    if (separador == std::string::npos) continue;
                    const auto valor = linha.substr(separador + 5);
                    std::size_t fim = 0;
                    const double tempo = std::stod(valor, &fim);
                    if (fim >= valor.size() || valor[fim] != 's' ||
                        valor.find_first_not_of(" \t\r", fim + 1) != std::string::npos) continue;
                    Resultado r{"legado-" + std::to_string(fase) + "-" + std::to_string(indice),
                                fase, 1, {nome_valido(linha.substr(0, separador))}, tempo};
                    validar(r);
                    resultados.push_back(std::move(r));
                } catch (const std::exception&) { /* Linhas antigas invalidas nao viram pontuacoes. */ }
            }
        }
    }
    return resultados;
}
std::vector<Resultado> RepositorioRanking::consultar(int fase, int jogadores) const {
    auto resultados = ler();
    resultados.erase(std::remove_if(resultados.begin(), resultados.end(), [=](const Resultado& r) {
        return r.fase != fase || r.jogadores != jogadores;
    }), resultados.end());
    // Empates mantem a ordem de registro.
    std::stable_sort(resultados.begin(), resultados.end(), [](const Resultado& a, const Resultado& b) {
        return CriterioTempo().precede({a.id,a.fase,a.jogadores,a.nomes,0,a.segundos,true},
                                      {b.id,b.fase,b.jogadores,b.nomes,0,b.segundos,true});
    });
    return resultados;
}
void RepositorioRanking::registrar(const Resultado& resultado) {
    validar(resultado);
    auto resultados = ler();
    for (const auto& r : resultados) if (r.id == resultado.id) return;
    if (resultados.size() >= 10000) throw std::runtime_error("Ranking atingiu o limite de registros.");
    resultados.push_back(resultado);
    Json dados = {{"versao", 1}, {"resultados", Json::array()}};
    for (const auto& r : resultados)
        dados["resultados"].push_back({{"id", r.id}, {"fase", r.fase}, {"jogadores", r.jogadores},
                                      {"nomes", r.nomes}, {"segundos", r.segundos}});
    escrever_json(caminho, dados);
}
}
