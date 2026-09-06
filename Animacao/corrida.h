#pragma once
#include "../Recursos/configuracao.h"
#include <stdexcept>

namespace Animacao {
struct Estado {
    bool correndo = false;
    unsigned quadro = 0;
    unsigned passos = 0;
    bool direita = true;
};
// Controlador puro: nao abre janela, nao carrega recursos e so avanca na simulacao.
class Corrida {
    Estado estado;
public:
    const Estado& obter() const { return estado; }
    void restaurar(Estado salvo) {
        if (salvo.quadro >= Recursos::Configuracao::quadros_corrida ||
            salvo.passos >= Recursos::Configuracao::passos_quadro_corrida ||
            (!salvo.correndo && (salvo.quadro || salvo.passos)))
            throw std::runtime_error("Estado de animacao de corrida invalido");
        estado = salvo;
    }
    void avancar(float velocidade_horizontal) {
        if (velocidade_horizontal != 0.f) estado.direita = velocidade_horizontal > 0.f;
        estado.correndo = velocidade_horizontal != 0.f;
        if (!estado.correndo) { estado.quadro = estado.passos = 0; return; }
        if (++estado.passos == Recursos::Configuracao::passos_quadro_corrida) {
            estado.passos = 0;
            estado.quadro = (estado.quadro + 1) % Recursos::Configuracao::quadros_corrida;
        }
    }
};
}
