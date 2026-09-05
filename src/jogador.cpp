#include "../Persistencia/entidades.h"
#include "../Entidades/Personagens/jogador.h"
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;

namespace Entidades
{
    namespace Personagens
    {
        Gerenciadores::Gerenciador_Estados *Jogador::PGEstados(Gerenciadores::Gerenciador_Estados::get_instancia());

        Jogador::Jogador(sf::Vector2f pos, sf::Vector2f vel, bool jog2) : Personagem(pos, vel),
         jogador2(jog2), leu_fase(false), poder(1), tempo(0.0)
        {
            this->set_vida(20);
            if (!jog2)
            {
                corpo.setFillColor(sf::Color::Green);
                this->setSkin("Design/imagens/op1.png");
            }
            else
            {
                corpo.setFillColor(sf::Color::Green);
                this->setSkin("Design/imagens/luigiDireita.png");
            }
           tempo = 0.0;
           nome = jog2 ? "Jogador 2" : "Jogador 1";
        }
        Jogador::~Jogador()
        {
            
        }

        void Jogador::atualizar()
        {
            corpo.setPosition(corpo.getPosition() + velocidade);
        }

        void Jogador::executar()
        {
            if (vivo)
            {
                if(vida > 0)
                {
                    corpo.setFillColor(sf::Color::Green);
                    if (vida < 10)
                    {
                    corpo.setFillColor(sf::Color::Yellow);
                    }

                    if (!jogador2)
                    mover();
                     else
                    mover_jog2();
                }
                else
                {
                    corpo.setFillColor(sf::Color::Red);
                    
                }
            }
                
             if (vida <= 0)
            {
                set_vivo(0);
            }

        }

        void Jogador::tela_pause()
        {
        }

        void Jogador::atacar(Entidade *jg)
        {
        }
        void Jogador::colidir(Entidade* pE, int a)
        {
            
        }


        void Jogador::mover()
        {
            if(!leu_fase)
            {
                PGEstados->get_fase();
                leu_fase = true;
            }
            
            if (!nochao)
                velocidade.y += 0.1f;

            else
                velocidade.y = 0.0f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                velocidade.x += 0.1f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                velocidade.x += -0.1f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && nochao)
                velocidade.y += -6.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                velocidade.y += 0.1f;

            nochao = false;

            atualizar();
        }

        void Jogador::mover_jog2()
        {
            if (!nochao)
                velocidade.y += 0.1f;

            else
                velocidade.y = 0.0f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                velocidade.x += 0.1f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                velocidade.x += -0.1f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && nochao)
                velocidade.y += -6.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                velocidade.y += 0.1f;

            // if(pausado == false)
            atualizar();

            nochao = false;
        }

        void Jogador::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
