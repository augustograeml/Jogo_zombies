#include <algorithm>
#include <cmath>
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
            mover_com_controles(sf::Keyboard::isKeyPressed(sf::Keyboard::A),
                               sf::Keyboard::isKeyPressed(sf::Keyboard::D),
                               sf::Keyboard::isKeyPressed(sf::Keyboard::W),
                               sf::Keyboard::isKeyPressed(sf::Keyboard::S));
        }

        void Jogador::mover_jog2()
        {
            mover_com_controles(sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
                               sf::Keyboard::isKeyPressed(sf::Keyboard::Right),
                               sf::Keyboard::isKeyPressed(sf::Keyboard::Up),
                               sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
        }

        void Jogador::mover_com_controles(bool esquerda, bool direita, bool pular, bool descer)
        {
            constexpr float aceleracao = 0.1f, freio = 0.2f, limite = 4.f;
            const int sentido = static_cast<int>(direita) - static_cast<int>(esquerda);
            if (sentido) velocidade.x = std::clamp(velocidade.x + sentido * aceleracao, -limite, limite);
            else {
                // Freio finito: atinge zero, sem deslizar indefinidamente.
                velocidade.x = std::clamp(velocidade.x, -limite, limite);
                if (std::abs(velocidade.x) <= freio) velocidade.x = 0;
                else velocidade.x += velocidade.x > 0 ? -freio : freio;
            }
            if (!nochao) velocidade.y += 0.1f;
            else velocidade.y = 0;
            if (pular && nochao) velocidade.y = -6.f;
            if (descer) velocidade.y += 0.1f;
            nochao = false;
            leu_fase = true;
            atualizar();
        }

        void Jogador::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
