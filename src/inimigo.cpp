#include "../Recursos/catalogo.h"
#include "../Entidades/Personagens/arqueiro.h"
#include "../Entidades/Personagens/gigante.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/aleatorio.h"
#include "../Entidades/Personagens/inimigo.h"
#include <stdlib.h>
#include <math.h>

namespace Entidades
{
    namespace Personagens
    {
        Inimigo::Inimigo(sf::Vector2f pos, sf::Vector2f vel) : Personagem(pos, vel), pjogador(nullptr), maldade(false), direcao(false)
        {
            int m = Persistencia::sortear(6);
            if(m == 1)
                maldade = true;
        }

        Inimigo::~Inimigo()
        {
            pjogador = nullptr;
        }

        sf::Vector2f Inimigo::getPosicao()
        {
            return (corpo.getPosition());
        }

        void Inimigo::setPosicao(sf::Vector2f& p)
        {
            this->posicao = p;
        }

        void Inimigo::update(sf::Vector2f& posicao_jogador)
        {
            sf::Vector2f direcao = posicao_jogador - getPosicao();
            float comprimento = sqrt(direcao.x * direcao.x + direcao.y * direcao.y);
            direcao /= comprimento;
        }

        void Inimigo::mudar_direcao()
        {
            direcao = !direcao;
        }

        void Inimigo::desenhar() {
            const auto tipo=dynamic_cast<Gigante*>(this)?Recursos::Animado::Gigante:
                dynamic_cast<Arqueiro*>(this)?Recursos::Animado::Arqueiro:Recursos::Animado::Zumbi;
            const auto& estado=animacao.obter();
            const auto& quadros=Recursos::caminhada(tipo);
            const auto indice=estado.correndo ? estado.quadro*quadros.size()/Recursos::Configuracao::quadros_corrida : 0;
            const auto& quadro=quadros.at(indice);
            sf::Sprite visual(*quadro.textura,quadro.regiao);
            const auto caixa=corpo.getGlobalBounds();
            const float escala=caixa.height/quadro.regiao.height;
            visual.setOrigin(quadro.regiao.width/2.f,quadro.regiao.height);
            const bool arte_direita=tipo==Recursos::Animado::Zumbi;
            visual.setScale(estado.direita==arte_direita?escala:-escala,escala);
            visual.setPosition(caixa.left+caixa.width/2.f,caixa.top+caixa.height);
            visual.setColor(corpo.getFillColor());
            pGG->get_Janela()->draw(visual);
        }

        void Inimigo::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
