#include "../Recursos/catalogo.h"
#include "../Recursos/configuracao.h"
#include <cstdlib>
#include <fstream>
#include <map>
#include <stdexcept>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Recursos {
namespace {
std::filesystem::path executavel() {
#ifdef _WIN32
    std::wstring buffer(32768, L'\0');
    const auto tamanho = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (tamanho && tamanho < buffer.size()) { buffer.resize(tamanho); return std::filesystem::path(buffer).parent_path(); }
#else
    std::error_code erro;
    const auto caminho = std::filesystem::read_symlink("/proc/self/exe", erro);
    if (!erro) return caminho.parent_path();
#endif
    return {};
}
std::filesystem::path procurar(std::filesystem::path inicio) {
    for (unsigned nivel = 0; !inicio.empty() && nivel < 8; ++nivel) {
        if (std::filesystem::is_directory(inicio / "Design")) return inicio / "Design";
        auto pai = inicio.parent_path();
        if (pai == inicio) break;
        inicio = std::move(pai);
    }
    return {};
}
std::filesystem::path raiz() {
    if (const char* ambiente = std::getenv("ZOMBIES_RECURSOS")) {
        std::filesystem::path base(ambiente);
        if (std::filesystem::is_directory(base / "Design")) base /= "Design";
        if (!std::filesystem::is_directory(base))
            throw std::runtime_error("ZOMBIES_RECURSOS nao aponta para uma pasta existente: " + base.string());
        return base;
    }
    auto base = procurar(std::filesystem::current_path());
    if (base.empty()) base = procurar(executavel());
    if (base.empty()) throw std::runtime_error("Pasta Design nao encontrada. Mantenha Design junto ao executavel ou defina ZOMBIES_RECURSOS.");
    return base;
}
}
std::filesystem::path caminho(const std::string& relativo) {
    std::filesystem::path pedido(relativo);
    if (pedido.is_absolute()) return pedido;
    auto texto = pedido.generic_string();
    if (texto.rfind("Design/", 0) == 0) texto.erase(0, 7);
    return raiz() / texto;
}
std::shared_ptr<sf::Texture> textura(const std::string& relativo) {
    static std::map<std::filesystem::path, std::shared_ptr<sf::Texture>> cache;
    const auto arquivo = caminho(relativo);
    const auto encontrado = cache.find(arquivo);
    if (encontrado != cache.end()) return encontrado->second;
    auto recurso = std::make_shared<sf::Texture>();
    if (!recurso->loadFromFile(arquivo.string()))
        throw std::runtime_error("Nao foi possivel carregar a imagem: " + arquivo.string());
    cache.emplace(arquivo, recurso);
    return recurso;
}
const std::vector<Quadro>& corrida() {
    // Conferidos visualmente: op1..op27 formam um ciclo; parado/andando sao outra arte.
    static const std::vector<Quadro> quadros = [] {
        std::vector<Quadro> resultado;
        for (unsigned numero = 1; numero <= Configuracao::quadros_corrida; ++numero) {
            auto recurso = textura("imagens/op" + std::to_string(numero) + ".png");
            const auto pixels = recurso->copyToImage();
            const auto tamanho = pixels.getSize();
            unsigned esquerda = tamanho.x, cima = tamanho.y, direita = 0, baixo = 0;
            for (unsigned y = 0; y < tamanho.y; ++y) for (unsigned x = 0; x < tamanho.x; ++x)
                if (pixels.getPixel(x, y).a > 8) {
                    esquerda = std::min(esquerda, x); cima = std::min(cima, y);
                    direita = std::max(direita, x); baixo = std::max(baixo, y);
                }
            if (esquerda == tamanho.x) throw std::runtime_error("Quadro de corrida inteiramente transparente: op" + std::to_string(numero));
            resultado.push_back({std::move(recurso), {static_cast<int>(esquerda), static_cast<int>(cima),
                static_cast<int>(direita - esquerda + 1), static_cast<int>(baixo - cima + 1)}});
        }
        return resultado;
    }();
    return quadros;
}
std::vector<std::string> validar_mapa(const std::string& relativo) {
    const auto arquivo = caminho(relativo);
    std::ifstream entrada(arquivo);
    if (!entrada) throw std::runtime_error("Cenario nao encontrado: " + arquivo.string());
    std::vector<std::string> linhas;
    std::string linha;
    while (std::getline(entrada, linha)) {
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.size() > 4096 || linhas.size() >= 4096) throw std::runtime_error("Cenario excede o limite de 4096 linhas/colunas: " + arquivo.string());
        for (std::size_t coluna = 0; coluna < linha.size(); ++coluna)
            if (linha[coluna] != ' ' && (linha[coluna] < '0' || linha[coluna] > '9'))
                throw std::runtime_error("Simbolo desconhecido no cenario " + arquivo.string() + ", linha " + std::to_string(linhas.size() + 1) + ", coluna " + std::to_string(coluna + 1));
        linhas.push_back(linha);
    }
    if (!entrada.eof() || linhas.empty()) throw std::runtime_error("Cenario vazio ou leitura incompleta: " + arquivo.string());
    return linhas;
}
}
