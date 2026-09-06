#include "../Persistencia/aleatorio.h"
#include "../Entidades/Obstaculos/coracao.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Coracao::Coracao(sf::Vector2f pos): Obstaculo(pos,false, true, false,false,false), cura(20)
        {
            this->setSkin("Design/imagens/saude.png");
            int dificil = Persistencia::sortear(10);
            if(dificil == 7)
                cura = 5;
        }

        Coracao::~Coracao()
        {

        }

        void Coracao::executar()
        {
            
        }

        void Coracao::mover()
        {
            
        }

        void Coracao::atualizar()
        {
            
        }


        void Coracao::curar(Entidades::Personagens::Jogador* pJ)
        {
            pJ->curar(cura);
        }
        void Coracao::colidir(Entidade* pE, int a)
        {
            Entidades::Personagens::Jogador* aux = static_cast<Entidades::Personagens::Jogador*> (pE);
            curar(aux);
            morrer();
        }
    }
}
