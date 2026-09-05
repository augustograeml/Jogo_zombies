#include "../Persistencia/entidades.h"
#include "../Persistencia/aleatorio.h"
#include "../Entidades/Personagens/zumbi.h"
#include "../Entidades/Personagens/inimigo.h"

namespace Entidades
{
    namespace Personagens
    {
        Zumbi::Zumbi(Vector2f pos, Vector2f vel) : Inimigo(pos, vel), numero_zumbi_salvos(0)
        {
            this->setSkin("Design/imagens/zumbi_bateu_morreu.png");
            direcao = 1;
            pulo = Persistencia::sortear(10);
        }

        Zumbi::~Zumbi()
        {
            pjogador = nullptr;
        }

        void Zumbi::atualizar()
        {
            corpo.setPosition(corpo.getPosition() + velocidade);
        }

        void Zumbi::mover()
        {

            if (direcao)
            {
                if (!nochao)
                    velocidade += Vector2f(0, 0.1);
                else
                {
                    velocidade = Vector2f(0.1f, 0.f);
                    if (pulo == 2)
                        velocidade = Vector2f(0.3f, -2.0f);
                    else
                        pulo = Persistencia::sortear(10);
                }

                nochao = false;
            }
            else
            {
                if (!nochao)
                    velocidade += Vector2f(0, 0.1);
                else
                {
                    velocidade = Vector2f(-0.1f, 0.f);
                    if (pulo == 2)
                        velocidade = Vector2f(-0.3f, -2.0f);
                    else
                        pulo = Persistencia::sortear(10);
                }

                nochao = false;
            }

            atualizar();
        }

        void Zumbi::executar()
        {
            if (vivo)
                mover();
        }

        void Zumbi::atacar(Entidade *jg)
        {
            jg->set_vida(jg->get_vida() - forca);
        }

        void Zumbi::colidir(Entidade *pE, int a)
        {
            if (a == 1 || a == 3)
            {
                atacar(pE);
                this->mudar_direcao();
            }
            else if (a == 4)
            {
                morrer();
            }
            else
            {
                atacar(pE);
            }
        }


        void Zumbi::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}