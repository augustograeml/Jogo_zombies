#pragma once
namespace Persistencia { class Serializador; }
#include "obstaculo.h"
#include "../Personagens/jogador.h"

namespace Entidades
{
    namespace Obstaculos
    {
        class Espinho : public Obstaculo
        {
        friend class Persistencia::Serializador;
            private:
                int dano;
            public:
                Espinho(Vector2f pos = Vector2(0.f, 0.f));
                ~Espinho();

                void espinhar(Entidades::Personagens::Jogador* pJ);
                void executar();
                int get_dano() {return dano;}
                void mover();
                void atualizar();
                void colidir(Entidade* pE,int a);
        };
    }
}