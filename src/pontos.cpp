#include "../Persistencia/pontos.h"
#include <algorithm>
#include <stdexcept>
namespace Persistencia {
namespace {
void validar(const ResultadoPontos& r) {
    if (r.id.empty() || r.id.size() > 128) throw std::runtime_error("Identificador do resultado invalido.");
    if (r.fase < 1 || r.fase > 2 || r.jogadores < 1 || r.jogadores > 2 ||
        r.nomes.size() != static_cast<std::size_t>(r.jogadores)) throw std::runtime_error("Categoria do ranking invalida.");
    for (const auto& n : r.nomes) nome_valido(n);
    if (r.pontos < 0 || r.pontos > Pontuacao::maximo) throw std::runtime_error("Pontuacao invalida.");
    numero(Json(r.segundos), 0, 2e12);
}
}
std::vector<ResultadoPontos> RepositorioPontos::ler() const {
    if (!std::filesystem::exists(caminho)) return {};
    const auto j = ler_json(caminho);
    if (j.at("formato") != "zombies-ranking-pontos" || inteiro(j.at("versao"), 1, 1) != 1 ||
        !j.at("resultados").is_array() || j.at("resultados").size() > 10000)
        throw std::runtime_error("Ranking de pontos invalido.");
    std::vector<ResultadoPontos> resultados;
    for (const auto& item : j.at("resultados")) {
        ResultadoPontos r;
        r.id = item.at("id").get<std::string>();
        r.fase = inteiro(item.at("fase"), 1, 2); r.jogadores = inteiro(item.at("jogadores"), 1, 2);
        r.nomes = item.at("nomes").get<std::vector<std::string>>();
        if (!item.at("pontos").is_number_integer()) throw std::runtime_error("Pontos devem ser inteiros.");
        r.pontos = static_cast<std::int64_t>(numero(item.at("pontos"), 0, Pontuacao::maximo));
        r.segundos = numero(item.at("segundos"), 0, 2e12); r.vitoria = item.at("vitoria").get<bool>();
        validar(r); resultados.push_back(std::move(r));
    }
    return resultados;
}
std::vector<ResultadoPontos> RepositorioPontos::consultar(int fase, int jogadores) const {
    return consultar(fase, jogadores, CriterioPontos{});
}
std::vector<ResultadoPontos> RepositorioPontos::consultar(int fase, int jogadores, const CriterioResultado& criterio) const {
    if (fase < 1 || fase > 2 || jogadores < 1 || jogadores > 2) throw std::runtime_error("Categoria invalida.");
    auto resultados = ler();
    resultados.erase(std::remove_if(resultados.begin(), resultados.end(), [&](const ResultadoPontos& r) {
        return r.fase != fase || r.jogadores != jogadores || r.pontos <= 0;
    }), resultados.end());
    std::stable_sort(resultados.begin(), resultados.end(), [&](const ResultadoPontos& a, const ResultadoPontos& b) {
        return criterio.precede(a, b);
    });
    if (resultados.size() > 8) resultados.resize(8);
    return resultados;
}
void RepositorioPontos::registrar(const ResultadoPontos& resultado) {
    validar(resultado);
    if (!resultado.pontos) return;
    auto resultados = ler();
    if (std::any_of(resultados.begin(), resultados.end(), [&](const ResultadoPontos& r) { return r.id == resultado.id; })) return;
    if (resultados.size() >= 10000) throw std::runtime_error("Ranking atingiu o limite de registros.");
    auto limpo = resultado;
    for (auto& n : limpo.nomes) n = nome_valido(n);
    resultados.push_back(std::move(limpo));
    Json lista = Json::array();
    for (const auto& r : resultados)
        lista.push_back({{"id", r.id}, {"fase", r.fase}, {"jogadores", r.jogadores}, {"nomes", r.nomes},
                         {"pontos", r.pontos}, {"segundos", r.segundos}, {"vitoria", r.vitoria}});
    escrever_json(caminho, {{"formato", "zombies-ranking-pontos"}, {"versao", 1}, {"resultados", lista}});
}
}
