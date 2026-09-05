#pragma once
namespace Persistencia { class Serializador; }
#include "obstaculo.h"
#include "../Personagens/jogador.h"

namespace Entidades
{
    namespace Obstaculos
    {
        class Neve : public Obstaculo
        {
        friend class Persistencia::Serializador;
            private:
                bool escorrega;
            public:
                Neve(Vector2f pos = Vector2f(0.f, 0.f));
                ~Neve();

                bool get_escorrega() {return escorrega;}
                void escorregar(Entidades::Personagens::Jogador* pJ);

                void executar();
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
        };
    }
}