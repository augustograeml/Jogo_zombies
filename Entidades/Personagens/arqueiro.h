#pragma once
namespace Persistencia { class Serializador; }
#include "inimigo.h"
#include "../projetil.h"
#define TEMPO_RECARGA 100

namespace Entidades
{
    namespace Personagens
    {
        class Arqueiro : public Inimigo
        {
        friend class Persistencia::Serializador;
            private:
            std::vector<Projetil> vetor_projeteis;
            int recarregar;
            bool atirando;
            int numero_salvo_arqueiros;

            public:
                Arqueiro(sf::Vector2f pos = sf::Vector2f(0.f, 0.f), sf::Vector2f vel = sf::Vector2f(0.f, 0.f));
                ~Arqueiro();

                void executar();
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
                void atirar();

                std::vector<Projetil>* get_projeteis() {return &vetor_projeteis;}
                void atacar(Entidade* jg);
                void salvar(std::ostringstream* entrada);
               
        };
    }
}