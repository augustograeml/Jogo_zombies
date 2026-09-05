#pragma once
namespace Persistencia { class Serializador; }
#include "../entidade.h"
#include "../Personagens/jogador.h"

namespace Entidades
{
    namespace Obstaculos
    {
        class Obstaculo : public Entidade
        {
        friend class Persistencia::Serializador;
            private:
                bool danoso;
                bool curoso;
                bool atrapalhante;
                bool escorregadio;
                bool gosmento;
            public:
                Obstaculo(Vector2f pos = Vector2f(0.f, 0.f), bool dano = false, bool cura = false, bool atrapalho = false, bool neve = false, bool musgo = false);
                ~Obstaculo();

                bool get_danoso()  {return danoso;}
                bool get_curoso() {return curoso;}
                bool get_atrapalhante() {return atrapalhante;}
                bool get_escorregadio() {return escorregadio;}
                bool get_gosmento() {return gosmento;}

                virtual void executar() = 0;
                virtual void mover() = 0;
                virtual void atualizar() = 0;
                virtual void colidir(Entidade* pE, int a) = 0;
        };
    }
}