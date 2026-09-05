#include "../Persistencia/aleatorio.h"
#include "../Entidades/Obstaculos/caixa.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Caixa::Caixa(sf::Vector2f pos) : Obstaculo(pos, false, false, true, false, false), atrapalha(false)
        {
            int i = Persistencia::sortear(10);
            if(i == 2)
                atrapalha = true;

            //se o jogador pular em cima da caixa e ela for fragil ela quebra

            this->setSkin("Design/imagens/caixa.png");
            this->set_vida(20);
        }

        Caixa::~Caixa()
        {

        }

        void Caixa::executar()
        {
            
        }

        void Caixa::mover()
        {
            
        }


        void Caixa::atualizar()
        {
            
        }
        void Caixa::colidir(Entidade* pE, int a)
        {
            
        }
    }
}