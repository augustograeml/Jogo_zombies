#include "../Persistencia/arquivo.h"
#include <fstream>
#include <cmath>
#include <stdexcept>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace Persistencia {
Json ler_json(const std::filesystem::path& caminho) {
    std::ifstream arquivo(caminho, std::ios::binary | std::ios::ate);
    if (!arquivo) throw std::runtime_error("Arquivo nao encontrado: " + caminho.string());
    const auto tamanho = arquivo.tellg();
    if (tamanho < 0 || tamanho > 8 * 1024 * 1024)
        throw std::runtime_error("Arquivo excede o limite de 8 MB.");
    arquivo.seekg(0);
    // Limita tambem a profundidade antes da construcao de objetos.
    return Json::parse(arquivo, [](int profundidade, Json::parse_event_t, Json&) {
        if (profundidade > 32) throw std::runtime_error("Arquivo muito aninhado.");
        return true;
    });
}

void escrever_json(const std::filesystem::path& caminho, const Json& dados) {
    const auto conteudo = dados.dump(2);
    if (conteudo.size() > 8 * 1024 * 1024) throw std::runtime_error("Salvamento muito grande.");
    auto temporario = caminho;
    temporario += ".tmp";
    try {
        std::ofstream arquivo(temporario, std::ios::binary | std::ios::trunc);
        if (!arquivo) throw std::runtime_error("Nao foi possivel criar o arquivo temporario.");
        arquivo << conteudo << '\n';
        arquivo.flush();
        if (!arquivo) throw std::runtime_error("Falha ao gravar os dados.");
        arquivo.close();
        if (!arquivo) throw std::runtime_error("Falha ao fechar o arquivo.");
#ifdef _WIN32
        if (!MoveFileExW(temporario.c_str(), caminho.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            throw std::runtime_error("Nao foi possivel substituir o arquivo salvo.");
#else
        std::filesystem::rename(temporario, caminho);
#endif
    } catch (...) {
        std::error_code erro;
        std::filesystem::remove(temporario, erro);
        throw;
    }
}

double numero(const Json& valor, double minimo, double maximo) {
    if (!valor.is_number()) throw std::runtime_error("Numero invalido no arquivo.");
    const double n = valor.get<double>();
    if (!std::isfinite(n) || n < minimo || n > maximo)
        throw std::runtime_error("Numero fora do intervalo permitido.");
    return n;
}
int inteiro(const Json& valor, int minimo, int maximo) {
    if (!valor.is_number_integer()) throw std::runtime_error("Inteiro invalido no arquivo.");
    return static_cast<int>(numero(valor, minimo, maximo));
}
std::string nome_valido(const std::string& nome) {
    const auto inicio = nome.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) throw std::runtime_error("Digite um nome.");
    auto resultado = nome.substr(inicio, nome.find_last_not_of(" \t\r\n") - inicio + 1);
    if (resultado.size() > 96) throw std::runtime_error("Nome muito longo.");
    for (unsigned char c : resultado)
        if (c < 32 || c == 127) throw std::runtime_error("Nome contem caracteres invalidos.");
    return resultado;
}
}
