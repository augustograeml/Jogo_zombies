#include "../Recursos/configuracao.h"
#include <algorithm>
#include "../Entidades/Obstaculos/musgo.h"
#include "../Recursos/plataforma.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Musgo::Musgo(sf::Vector2f pos) : Obstaculo(pos, false,false,false,false,true), gosmento(true)
        {
            this->setSkin("Design/imagens/bloco_musgo.png");
            corpo.setTextureRect(Recursos::trecho_plataforma(pos));
            this->set_vida(1);
        }

        Musgo::~Musgo()
        {

        }

        void Musgo::mover()
        {

        }

        void Musgo::executar()
        {

        }

        void Musgo::atualizar()
        {

        } 
        void Musgo::gosmar(Entidades::Personagens::Jogador* pJ)
        {
            if(get_gosmento())
            {
                pJ->setVelocidade(Vector2f(std::clamp(pJ->getVelocidade().x, -Recursos::Configuracao::velocidade_musgo, Recursos::Configuracao::velocidade_musgo) , pJ->getVelocidade().y));
            }
        }
        void Musgo::colidir(Entidade* pE,  int a)
        {
            Entidades::Personagens::Jogador* aux = static_cast<Entidades::Personagens::Jogador*> (pE);
            gosmar(aux);
        }
    }
}
