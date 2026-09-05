#pragma once
#include "fase.h"
namespace Estados::Fases {
class Fase2 : public Fase {
public:
    Fase2(int id, bool carregar);
    void executar() override { executar_comum(); }
};
}
