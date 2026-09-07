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
            if (vivo) {
                comportamento.direita=direcao;
                Logica::decidir_zumbi(comportamento,getPosicao().x,getPosicao().y,alvos);
                direcao=comportamento.direita;
                if(comportamento.acao==Logica::AcaoCorpo::Perseguindo) {
                    velocidade.x=direcao?0.65f:-0.65f;
                    velocidade.y=nochao?0.f:velocidade.y+0.1f;
                    nochao=false; atualizar();
                } else mover();
            }
        }

        void Zumbi::atacar(Entidade *jg)
        {
            jg->receber_dano(forca);
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
                auto impulso = pE->getVelocidade(); impulso.y = -3.f;
                pE->setVelocidade(impulso); pE->set_nochao(false);
                receber_dano(20);
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