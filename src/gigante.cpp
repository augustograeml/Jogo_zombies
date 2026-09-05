#include "../Persistencia/entidades.h"
#include "../Persistencia/aleatorio.h"
#include "../Entidades/Personagens/gigante.h"

namespace Entidades
{
    namespace Personagens
    {
        Gigante::Gigante(sf::Vector2f pos, sf::Vector2f vel) : Inimigo(pos, vel),
        ja_inc(false), tempo_pulo(0), numero_salvo_gigantes(0)
        {
            int i = Persistencia::sortear(10);
            if (i == 2)
                maldade = true;

            if (maldade)
                set_forca(10);
            else
                set_forca(4);

            this->setSkin("Design/imagens/zumbi_gigante.png");
            this->set_vida(100);
        }

        Gigante::~Gigante()
        {
        }

        void Gigante::atualizar()
        {
            corpo.setPosition(corpo.getPosition() + velocidade);
        }

        void Gigante::mover()
        {
            // velocidade += Vector2f(Persistencia::sortear(10) - 5, (float) (nochao ? - (Persistencia::sortear(5)) : 0));
            if (maldade && !(ja_inc))
            {
                velocidade.x += 0.2;
                ja_inc = true;
            }
            if (vida > 0)
            {
                if (!nochao)
                    velocidade += Vector2f(0, 0.1);
                else
                {
                    if (direcao)
                        velocidade = Vector2f(0.1f, 0.f);
                    else
                        velocidade = Vector2f(-0.1f, 0.f);
                }

                nochao = false;
                atualizar();
            }
            else
                morrer();
        }

        void Gigante::executar()
        {
            if (vivo)
                mover();
        }
        void Gigante::colidir(Entidade *pE, int a)
        {
            if (a == 1 || a == 3)
            {
                atacar(pE);
                mudar_direcao();
            }
            else if (a == 4)
            {
                this->set_vida(this->get_vida() - 10);
            }
            else
            {
                atacar(pE);
            }
        }


        void Gigante::atacar(Entidade *jg)
        {
            jg->set_vida(jg->get_vida() - forca);
        }

        void Gigante::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
