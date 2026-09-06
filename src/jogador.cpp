#include "../Logica/movimento.h"
#include <algorithm>
#include <cmath>
#include "../Persistencia/entidades.h"
#include "../Entidades/Personagens/jogador.h"
#include "../Recursos/catalogo.h"
#include "../Recursos/configuracao.h"
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
                Textura = Recursos::corrida().front().textura;
                corpo.setTexture(Textura.get());
            }
            else
            {
                corpo.setFillColor(sf::Color::Green);
                Textura = Recursos::textura("imagens/luigiDireita.png");
                corpo.setTexture(Textura.get());
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

        void Jogador::desenhar()
        {
            // O sprite tem transformacao propria: trocar arte nunca altera a colisao.
            const auto& estado = animacao.obter();
            sf::Sprite visual;
            if (!jogador2) {
                const auto& quadro = Recursos::corrida().at(estado.correndo ? estado.quadro : 9);
                visual.setTexture(*quadro.textura);
                visual.setTextureRect(quadro.regiao);
            } else visual.setTexture(*Textura);
            const auto area = visual.getLocalBounds();
            const auto caixa = corpo.getGlobalBounds();
            const float escala = caixa.height / area.height;
            visual.setOrigin(area.width / 2.f, area.height);
            visual.setScale(estado.direita ? escala : -escala, escala);
            visual.setPosition(caixa.left + caixa.width / 2.f, caixa.top + caixa.height);
            // Preserva a sinalizacao existente de saude; a geometria continua independente.
            visual.setColor(get_protecao() && (get_protecao()/5)%2 ? sf::Color(255,100,100,130) : sf::Color::White);
            pGG->get_Janela()->draw(visual);
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
            const auto nova=Logica::mover({velocidade.x,velocidade.y},nochao,esquerda,direita,pular,descer);
            velocidade={nova.x,nova.y};
            nochao = false;
            leu_fase = true;
            atualizar();
            animacao.avancar(velocidade.x);
        }

        void Jogador::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
