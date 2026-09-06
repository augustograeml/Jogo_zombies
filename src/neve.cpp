#include "../Entidades/Obstaculos/neve.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Neve::Neve(Vector2f pos): Obstaculo(pos, false,false,false,true,false), escorrega(true)
        {
            this->setSkin("Design/imagens/plataforma.png");
            this->set_vida(1);
        }

        Neve::~Neve()
        {

        }

        void Neve::executar()
        {
            
        }

        void Neve::mover()
        {
            
        }

        void Neve::atualizar()
        {
            
        }
        void Neve::escorregar(Entidades::Personagens::Jogador* pJ)
        {
            if(get_escorrega())
            {
                pJ->pousar(true);
            }
        }
        void Neve::colidir(Entidade* pE, int a)
        {
            Entidades::Personagens::Jogador* aux = static_cast<Entidades::Personagens::Jogador*> (pE);
            escorregar(aux);
        }
    }
}