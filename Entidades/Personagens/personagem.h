#pragma once
namespace Persistencia { class Serializador; }
#include "../entidade.h"

namespace Entidades
{
    namespace Personagens
    {
        class Personagem : public Entidade
        {
        friend class Persistencia::Serializador;
            protected:
                int forca;
            public:
                Personagem(sf::Vector2f pos, sf::Vector2f vel, int f = 5);
                ~Personagem();

                void set_forca(int forc);
                int get_forca();
                
                virtual void executar() = 0;
                virtual void colidir(Entidade* pE, int a) = 0;

                virtual void salvar(std::ostringstream* entrada) = 0;
                
                virtual void mover() = 0;
                virtual void atualizar() = 0;
                virtual void atacar(Entidade* jg) = 0;
                
        };
    }
}