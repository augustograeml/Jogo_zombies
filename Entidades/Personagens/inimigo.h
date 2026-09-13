#pragma once
namespace Persistencia { class Serializador; }
#include "../entidade.h"
#include "../projetil.h"
#include "../../Logica/comportamento.h"
#include "../../Gerenciadores/gerenciador_colisoes.h"
#include "jogador.h"  

namespace Entidades
{ 
    namespace Personagens
    {
        class Inimigo : public Personagem
        {
        friend class Persistencia::Serializador;
            protected:
                Gerenciadores::Gerenciador_Colisoes* terreno_percebido=nullptr; // Consulta derivada por passo, nao persistida.
                Jogador* pjogador;
                bool maldade;
                bool direcao;
                Animacao::Corrida animacao;
                    
            public:
                Inimigo(sf::Vector2f pos, sf::Vector2f vel);
                ~Inimigo();
                const Animacao::Estado& get_animacao() const { return animacao.obter(); }
                void restaurar_animacao(Animacao::Estado salvo) { animacao.restaurar(salvo); }
                void avancar_animacao() { if(vivo) { atualizar_reacao(); animacao.avancar(velocidade.x); } }
                void desenhar() override;
                sf::Vector2f getPosicao();
                void setPosicao(sf::Vector2f& p);
                void update(sf::Vector2f& posicao_jogador);
                void mudar_direcao();

                void perceber_terreno(Gerenciadores::Gerenciador_Colisoes* terreno) { terreno_percebido=terreno; }
                void deslocar_com_apoio();
                virtual void perceber(const std::vector<Logica::Alvo>&) {}
                virtual void atualizar() = 0;
                virtual void mover() = 0;

                virtual std::vector<Projetil>* get_projeteis() {return nullptr;}

                virtual void executar() = 0;
                virtual void colidir(Entidade* pE, int a) = 0;
                virtual void atacar(Entidade* jg) = 0;

                void salvar(std::ostringstream* entrada);
            };
        }
}
