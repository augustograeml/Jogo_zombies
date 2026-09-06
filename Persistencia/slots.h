#pragma once
#include "arquivo.h"
#include <functional>
#include <vector>
namespace Persistencia {
struct ResumoSlot {
    int numero = 1;
    bool existe = false, valido = false, backup_disponivel = false;
    int fase = 0, jogadores = 0;
    double segundos = 0;
    bool finalizada = false;
    std::string erro;
};
// Singleton intencional: todos os menus e a partida compartilham o slot selecionado.
class Slots {
    int atual = 1;
    std::filesystem::path pasta;
    std::function<void(const Json&)> validador;
    Slots() = default;
    void validar(const Json&) const;
    std::filesystem::path caminho(int) const;
public:
    Slots(const Slots&) = delete;
    Slots& operator=(const Slots&) = delete;
    static Slots& instancia();
    void selecionar(int numero);
    int selecionado() const { return atual; }
    std::filesystem::path caminho_atual() const { return caminho(atual); }
    void configurar_pasta(std::filesystem::path destino) { pasta = std::move(destino); }
    void definir_validador(std::function<void(const Json&)> funcao) { validador = std::move(funcao); }
    void salvar(const Json& dados);
    Json ler() const;
    // Acao explicita: conserva o arquivo danificado com sufixo .corrompido-N.
    Json recuperar();
    std::vector<ResumoSlot> listar() const;
};
}
