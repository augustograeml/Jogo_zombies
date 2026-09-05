#pragma once
namespace Persistencia { class Serializador; }
#include "entidade.h"

namespace Entidades
{
    class Projetil : public Entidade
    {
        friend class Persistencia::Serializador;
        private:
            int dano;
            bool direcao;
        public:
         Projetil(sf::Vector2f pos, bool dir);
         ~Projetil();

        void salvar(std::ostringstream* entrada);

        int get_dano(){return dano;}
        void mover();
        void danar(Entidade* pE);
        void atirar();
        void atualizar();
        void colidir(Entidade* pE, int a);

        void executar();
    };
}