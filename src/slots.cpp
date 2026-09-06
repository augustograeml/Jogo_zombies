#include "../Persistencia/slots.h"
#include <stdexcept>
namespace Persistencia {
Slots& Slots::instancia() { static Slots unica; return unica; }
void Slots::selecionar(int numero) {
    if (numero < 1 || numero > 3) throw std::runtime_error("Slot deve estar entre 1 e 3.");
    atual = numero;
}
std::filesystem::path Slots::caminho(int numero) const {
    return pasta / (numero == 1 ? "partida.json" : "partida-slot" + std::to_string(numero) + ".json");
}
void Slots::validar(const Json& j) const {
    if (j.at("formato") != "zombies-partida") throw std::runtime_error("Formato de partida invalido.");
    inteiro(j.at("versao"), 1, 2);
    const int fase = inteiro(j.at("fase"), 1, 2), jogadores = inteiro(j.at("numero_jogadores"), 1, 2);
    if (inteiro(j.at("estado"), 6, 9) != 6 + (fase - 1) * 2 + jogadores - 1)
        throw std::runtime_error("Estado e categoria da partida inconsistentes.");
    numero(j.at("passos"), 0, 6e13);
    numero(j.value("passos_anteriores", Json(0)), 0, 6e13);
    numero(j.at("acumulador"), 0, 0.25);
    const auto id = j.at("partida_id").get<std::string>();
    if (id.empty() || id.size() > 128) throw std::runtime_error("Identificador de partida invalido.");
    j.at("finalizada").get<bool>();
    j.at("vitoria").get<bool>();
    j.at("ranking_registrado").get<bool>();
    if (!j.at("aleatorio").is_string()) throw std::runtime_error("Estado aleatorio ausente.");
    for (const auto* lista : {"jogadores", "inimigos", "obstaculos"})
        if (!j.at(lista).is_array() || j.at(lista).size() > 10000)
            throw std::runtime_error("Lista de entidades invalida.");
    if (j.at("jogadores").size() != static_cast<std::size_t>(jogadores))
        throw std::runtime_error("Quantidade de jogadores invalida.");
    if (validador) validador(j);
}
void Slots::salvar(const Json& dados) {
    validar(dados);
    const auto destino = caminho_atual();
    if (std::filesystem::exists(destino)) {
        // Nunca substitui silenciosamente um slot corrompido nem seu backup valido.
        const auto anterior = ler_json(destino);
        validar(anterior);
        escrever_json(destino.string() + ".bak", anterior);
    }
    escrever_json(destino, dados);
}
Json Slots::ler() const { auto j = ler_json(caminho_atual()); validar(j); return j; }
Json Slots::recuperar() {
    const auto destino = caminho_atual();
    const auto dados = ler_json(destino.string() + ".bak");
    validar(dados);
    if (std::filesystem::exists(destino)) {
        std::filesystem::path preservado;
        unsigned indice = 1;
        do { preservado = destino.string() + ".corrompido-" + std::to_string(indice++); }
        while (std::filesystem::exists(preservado));
        // Copiar primeiro preserva o original mesmo se a substituicao posterior falhar.
        std::filesystem::copy_file(destino, preservado);
    }
    escrever_json(destino, dados);
    return dados;
}
std::vector<ResumoSlot> Slots::listar() const {
    std::vector<ResumoSlot> lista;
    for (int i = 1; i <= 3; ++i) {
        ResumoSlot r; r.numero = i;
        try {
            r.existe = std::filesystem::exists(caminho(i));
            if (r.existe) {
                auto j = ler_json(caminho(i)); validar(j);
                r.valido = true; r.fase = j.at("fase"); r.jogadores = j.at("numero_jogadores");
                r.segundos = (numero(j.at("passos"), 0, 6e13) + numero(j.value("passos_anteriores", Json(0)), 0, 6e13)) / 60.;
                r.finalizada = j.at("finalizada");
            }
        } catch (const std::exception& e) { r.erro = e.what(); }
        try { auto j = ler_json(caminho(i).string() + ".bak"); validar(j); r.backup_disponivel = true; }
        catch (const std::exception&) {}
        lista.push_back(std::move(r));
    }
    return lista;
}
}
