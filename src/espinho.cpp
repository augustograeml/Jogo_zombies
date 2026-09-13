#include "../Recursos/escala.h"
#include "../Recursos/catalogo.h"
#include "../Entidades/Obstaculos/espinho.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Espinho::Espinho(sf::Vector2f pos) : Obstaculo(pos, true, false ,false,false,false), dano(5)
        {
            this->setSkin("Design/imagens/espinho.png");
            Recursos::dimensionar_objeto(corpo,*Textura,Recursos::Escala::espinho);
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
            pJ->receber_dano(get_dano());
        }
        void Espinho::colidir(Entidade* pE, int a)
        {
            Entidades::Personagens::Jogador* aux = static_cast<Entidades::Personagens::Jogador*> (pE);
            espinhar(aux);
        }
    }
}