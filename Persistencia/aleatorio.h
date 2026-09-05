#pragma once
#include <random>
#include <sstream>
#include <stdexcept>

namespace Persistencia {
// Motor serializavel: carregar nao muda as proximas decisoes dos inimigos.
inline std::mt19937& motor() {
    static std::mt19937 instancia(std::random_device{}());
    return instancia;
}
inline int sortear(int limite) { return static_cast<int>(motor()() % limite); }
inline std::string estado_aleatorio() {
    std::ostringstream saida;
    saida << motor();
    return saida.str();
}
inline std::mt19937 ler_motor(const std::string& estado) {
    if (estado.size() > 20000) throw std::runtime_error("Estado aleatorio invalido.");
    std::istringstream entrada(estado);
    std::mt19937 resultado;
    if (!(entrada >> resultado)) throw std::runtime_error("Estado aleatorio invalido.");
    entrada >> std::ws;
    if (!entrada.eof()) throw std::runtime_error("Estado aleatorio invalido.");
    return resultado;
}
}
