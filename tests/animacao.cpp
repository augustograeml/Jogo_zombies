#include "../Animacao/corrida.h"
#include <iostream>
#include <stdexcept>

int main() {
    const auto exigir = [](bool ok) { if (!ok) throw std::runtime_error("Falha na animacao"); };
    Animacao::Corrida original;
    exigir(!original.obter().correndo && original.obter().direita);
    original.avancar(-1.f);
    exigir(original.obter().correndo && !original.obter().direita && original.obter().passos == 1);
    Animacao::Corrida retomada;
    retomada.restaurar(original.obter());
    for (int i = 0; i < 100; ++i) {
        original.avancar(-1.f); retomada.avancar(-1.f);
        exigir(original.obter().quadro == retomada.obter().quadro);
        exigir(original.obter().passos == retomada.obter().passos);
    }
    original.avancar(0.f);
    exigir(!original.obter().correndo && !original.obter().direita && original.obter().quadro == 0 && original.obter().passos == 0);
    original.avancar(1.f);
    exigir(original.obter().direita);
    bool rejeitou = false;
    try { retomada.restaurar({true, 27, 0, true}); } catch (const std::runtime_error&) { rejeitou = true; }
    exigir(rejeitou);
    rejeitou = false;
    try { retomada.restaurar({false, 0, 1, true}); } catch (const std::runtime_error&) { rejeitou = true; }
    exigir(rejeitou);
    std::cout << "Animacao: parada, direcao, ciclo e retomada aprovados sem janela.\n";
}
