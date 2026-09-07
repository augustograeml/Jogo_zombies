#include "../Recursos/catalogo.h"
#include "../Animacao/pose.h"
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
            auto pose=estado.correndo?Animacao::Pose::Caminhada:Animacao::Pose::Repouso;
            float progresso=0;
            if(auto* gigante=dynamic_cast<Gigante*>(this)) {
                const auto& c=gigante->get_comportamento();
                if(c.acao==Logica::AcaoCorpo::Preparando) { pose=Animacao::Pose::Preparacao; progresso=c.passos/42.f; }
                else if(c.acao==Logica::AcaoCorpo::Golpe) { pose=Animacao::Pose::Ataque; progresso=c.passos/12.f; }
                else if(c.acao==Logica::AcaoCorpo::Recuperando) { pose=Animacao::Pose::Recuperacao; progresso=c.passos/60.f; }
            } else if(auto* arqueiro=dynamic_cast<Arqueiro*>(this)) {
                const auto& c=arqueiro->get_comportamento();
                if(c.acao==Logica::AcaoArqueiro::Preparando) { pose=Animacao::Pose::Preparacao; progresso=c.preparacao/30.f; }
                else if(arqueiro->get_recarga()>90) { pose=Animacao::Pose::Ataque; progresso=(100-arqueiro->get_recarga())/10.f; }
            }
            const bool preparando=pose==Animacao::Pose::Preparacao;
            const bool golpeando=pose==Animacao::Pose::Ataque;
            if(get_reacao()) { pose=Animacao::Pose::Dano; progresso=get_reacao()/12.f; }
            const auto deformacao=Animacao::deformacao(pose,progresso);
            const auto& quadros=Recursos::caminhada(tipo);
            const auto indice=estado.correndo ? estado.quadro*quadros.size()/Recursos::Configuracao::quadros_corrida : 0;
            const auto& quadro=quadros.at(indice);
            sf::Sprite visual(*quadro.textura,quadro.regiao);
            const auto caixa=corpo.getGlobalBounds();
            const float escala=caixa.height/quadro.regiao.height;
            visual.setOrigin(quadro.regiao.width/2.f,quadro.regiao.height);
            const bool arte_direita=tipo==Recursos::Animado::Zumbi;
            visual.setScale((estado.direita==arte_direita?escala:-escala)*deformacao.x,escala*deformacao.y);
            visual.setRotation((estado.direita?1:-1)*deformacao.angulo);
            visual.setPosition(caixa.left+caixa.width/2.f,caixa.top+caixa.height);
            visual.setColor(corpo.getFillColor());
            pGG->get_Janela()->draw(visual);
            auto* janela=pGG->get_Janela();
            if(preparando) {
                // Silhueta e exclamacao legiveis mesmo em escala de cinza/sem audio.
                sf::CircleShape placa(13,3); placa.setOrigin(13,13);
                placa.setPosition(caixa.left+caixa.width/2,caixa.top-20);
                placa.setFillColor(sf::Color::White); placa.setOutlineColor(sf::Color::Black);
                placa.setOutlineThickness(2); janela->draw(placa);
                sf::RectangleShape traco({3,8}); traco.setFillColor(sf::Color::Black);
                traco.setPosition(caixa.left+caixa.width/2-1.5f,caixa.top-26); janela->draw(traco);
                traco.setSize({3,3}); traco.move(0,10); janela->draw(traco);
            }
            if(golpeando) {
                for(int i=0;i<3;++i) {
                    sf::RectangleShape risco({12,3}); risco.setFillColor(sf::Color::White);
                    risco.setOutlineThickness(1); risco.setOutlineColor(sf::Color::Black);
                    risco.setPosition(caixa.left+(estado.direita?caixa.width+4:-16),caixa.top+12+i*8);
                    janela->draw(risco);
                }
            }
        }

        void Inimigo::salvar(std::ostringstream* entrada)
        {
            *entrada << Persistencia::Serializador::salvar(*this).dump();
        }
    }
}
