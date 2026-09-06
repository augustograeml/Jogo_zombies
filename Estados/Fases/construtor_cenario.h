#pragma once
#include "../../Listas/listaEntidades.h"
#include <string>
namespace Estados::Fases {
// A leitura/legenda do mapa nao pertence ao loop da partida.
class ConstrutorCenario {
public:
    static void obstaculos(const std::string& caminho, Listas::ListaEntidade& destino);
    static void inimigos(const std::string& caminho, Listas::ListaEntidade& destino);
};
}
