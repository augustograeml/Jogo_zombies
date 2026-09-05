#pragma once
namespace Persistencia { class Serializador; }
#include "obstaculo.h"

namespace Entidades
{
    namespace Obstaculos
    {
        class Caixa : public Obstaculo
        {
        friend class Persistencia::Serializador;
            private:
                bool atrapalha;
            public:
                Caixa(Vector2f pos = Vector2f(0.f, 0.f));
                ~Caixa();


                void executar();
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
        };
    }
}