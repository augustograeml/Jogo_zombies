#pragma once
namespace Persistencia { class Serializador; }
#include "obstaculo.h"
#include "../Personagens/jogador.h"

namespace Entidades
{
    namespace Obstaculos
    {
        class Musgo : public Obstaculo
        {
        friend class Persistencia::Serializador;
            private:
                bool gosmento;
            public:
                Musgo(sf::Vector2f pos);
                ~Musgo();

                bool get_gosmento() {return gosmento;}
                void gosmar(Entidades::Personagens::Jogador* pJ);

                void executar();
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
        };
    }
}