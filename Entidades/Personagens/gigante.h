#pragma once
namespace Persistencia { class Serializador; }
#include "inimigo.h"

namespace Entidades
{
    namespace Personagens
    {
        class Gigante : public Inimigo
        {
        friend class Persistencia::Serializador;
            private:
                bool ja_inc;
                double tempo_pulo;
                int numero_salvo_gigantes;
            public:
                Gigante(sf::Vector2f pos = sf::Vector2f(0.f, 0.f), sf::Vector2f vel = sf::Vector2f(0.f, 0.f));
                ~Gigante();

                void executar();
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
                void atacar(Entidade* jg);
                std::vector<Projetil>* get_projeteis() {return nullptr;}
                void salvar(std::ostringstream* entrada);
              
        };
    }
}