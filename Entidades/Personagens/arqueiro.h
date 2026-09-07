#pragma once
namespace Persistencia { class Serializador; }
#include "inimigo.h"
#include "../projetil.h"
#include "../../Logica/comportamento.h"
#define TEMPO_RECARGA Recursos::Configuracao::recarga_arqueiro

namespace Entidades
{
    namespace Personagens
    {
        class Arqueiro : public Inimigo
        {
        friend class Persistencia::Serializador;
            private:
            std::vector<Projetil> vetor_projeteis;
            int recarregar;
            bool atirando;
            int numero_salvo_arqueiros;
            Logica::EstadoArqueiro comportamento;
            std::vector<Logica::Alvo> alvos;
            bool disparou = false; // Evento apenas do passo corrente; nao e estado persistente.

            public:
                Arqueiro(sf::Vector2f pos = sf::Vector2f(0.f, 0.f), sf::Vector2f vel = sf::Vector2f(0.f, 0.f));
                ~Arqueiro();

                void executar();
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
                void atirar();
                void perceber(const std::vector<Logica::Alvo>& jogadores) { alvos = jogadores; }
                int get_recarga() const { return recarregar; }
                bool disparou_no_passo() const { return disparou; }
                const Logica::EstadoArqueiro& get_comportamento() const { return comportamento; }
                void restaurar_comportamento(Logica::EstadoArqueiro e) { comportamento = e; }

                std::vector<Projetil>* get_projeteis() {return &vetor_projeteis;}
                void atacar(Entidade* jg);
                void salvar(std::ostringstream* entrada);
               
        };
    }
}
