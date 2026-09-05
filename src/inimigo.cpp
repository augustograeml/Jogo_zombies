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

        void Inimigo::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
