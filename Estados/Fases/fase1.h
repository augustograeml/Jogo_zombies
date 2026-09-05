#pragma once
#include "fase.h"
namespace Estados::Fases {
class Fase1 : public Fase {
public:
    Fase1(int id, bool carregar);
    void executar() override { executar_comum(); }
};
}
