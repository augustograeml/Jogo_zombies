#include "../Entidades/Obstaculos/espinho.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Espinho::Espinho(sf::Vector2f pos) : Obstaculo(pos, true, false ,false,false,false), dano(5)
        {
            this->setSkin("Design/imagens/espinho.png");
            this->set_vida(55);
        }

        Espinho::~Espinho()
        {

        }

        void Espinho::executar()
        {

        }

        void Espinho::mover()
        {

        }


        void Espinho::atualizar()
        {

        }
        void Espinho::espinhar(Entidades::Personagens::Jogador* pJ)
        {
            pJ->set_vida(pJ->get_vida() - get_dano());
        }
        void Espinho::colidir(Entidade* pE, int a)
        {
            Entidades::Personagens::Jogador* aux = static_cast<Entidades::Personagens::Jogador*> (pE);
            espinhar(aux);
        }
    }
}