#include "../Persistencia/entidades.h"
#include <algorithm>
#include "../Entidades/Personagens/arqueiro.h"

namespace Entidades
{
    namespace Personagens
    {
        Arqueiro::Arqueiro(sf::Vector2f pos, sf::Vector2f vel) : Inimigo(pos, vel), vetor_projeteis(), numero_salvo_arqueiros(0)
        {
            this->setSkin("Design/imagens/zumbi_atirador.png");
            atirando = false;
            direcao = 0;
            recarregar = 0;
            set_forca(5);

            // depois mexer com o setvida desse molecote aqui
        }

        Arqueiro::~Arqueiro()
        {
            pjogador = nullptr;
        }

        void Arqueiro::atualizar()
        {
            corpo.setPosition(corpo.getPosition() + velocidade);
        }

        void Arqueiro::mover()
        {
            if (!nochao)
                velocidade += Vector2f(0, 0.1);
            else
            {
                if (direcao)
                    velocidade = Vector2f(-0.1f, 0.f);
                else
                    velocidade = Vector2f(0.1f, 0.f);
            }
            atualizar();
            nochao = false;
        }

        void Arqueiro::executar()
        {
            disparou = false;
            if (vivo)
            {
                const bool disparar = Logica::decidir_arqueiro(comportamento,
                    getPosicao().x, getPosicao().y, alvos, recarregar > 0);
                if (comportamento.acao == Logica::AcaoArqueiro::Preparando) {
                    velocidade.x = 0;
                    if (!nochao) velocidade.y += 0.1f; else velocidade.y = 0;
                    atualizar(); nochao = false;
                    corpo.setFillColor(sf::Color(255, 160, 100));
                } else { mover(); corpo.setFillColor(sf::Color::White); }
                if (recarregar > 0) --recarregar;
                if (disparar) { direcao = comportamento.direita; atirar(); }
            }
            for (auto& flecha : vetor_projeteis) flecha.executar();
            vetor_projeteis.erase(std::remove_if(vetor_projeteis.begin(), vetor_projeteis.end(),
                [](Projetil& flecha) { return !flecha.get_vivo(); }), vetor_projeteis.end());
        }

        void Arqueiro::atacar(Entidade *jg)
        {
            jg->receber_dano(forca);
        }
        
        void Arqueiro::colidir(Entidade *pE, int a)
        {
            if (a == 1 || a == 3)
            {
                if(a == 1)
                    direcao = 0;
                else
                    direcao = 1;

                atacar(pE);
                mudar_direcao();
            }
            else if (a == 4)
                receber_dano(20);
            else
                atacar(pE);
        }


        void Arqueiro::atirar()
        {
            if (recarregar == 0)
            {
                Projetil novo_projetil({10, 5}, direcao);
                novo_projetil.setPosicao(sf::Vector2f(this->getPosicao().x + 20.f, this->getPosicao().y + 15.f));
                atirando = false;
                vetor_projeteis.push_back(novo_projetil);
                disparou = true;
                atirando = false;
                recarregar = TEMPO_RECARGA;
            }
            else
            {
                recarregar--;
            }
        }

        void Arqueiro::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
