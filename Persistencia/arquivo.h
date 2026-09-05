#pragma once
#include "../Estados/Fases/json.h"
#include <filesystem>
#include <string>

namespace Persistencia {
using Json = nlohmann::json;
// Leitura limitada e escrita por substituicao: falhas nao truncam o arquivo anterior.
Json ler_json(const std::filesystem::path& caminho);
void escrever_json(const std::filesystem::path& caminho, const Json& dados);
double numero(const Json& valor, double minimo, double maximo);
int inteiro(const Json& valor, int minimo, int maximo);
std::string nome_valido(const std::string& nome);
}
