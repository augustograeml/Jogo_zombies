#pragma once
namespace Persistencia { class Serializador; }
#include "inimigo.h"
#include "../../Logica/corpo_a_corpo.h"

namespace Entidades
{
    namespace Personagens
    {
        class Gigante : public Inimigo
        {
        friend class Persistencia::Serializador;
            private:
                Logica::EstadoCorpo comportamento;
                std::vector<Logica::Alvo> alvos;
                bool ja_inc;
                double tempo_pulo;
                int numero_salvo_gigantes;
            public:
                Gigante(sf::Vector2f pos = sf::Vector2f(0.f, 0.f), sf::Vector2f vel = sf::Vector2f(0.f, 0.f));
                ~Gigante();

                void executar();
                void perceber(const std::vector<Logica::Alvo>& jogadores) override { alvos=jogadores; }
                const Logica::EstadoCorpo& get_comportamento() const { return comportamento; }
                void restaurar_comportamento(Logica::EstadoCorpo e) { comportamento=e; }
                void mover();
                void atualizar();
                void colidir(Entidade* pE, int a);
                void atacar(Entidade* jg);
                std::vector<Projetil>* get_projeteis() {return nullptr;}
                void salvar(std::ostringstream* entrada);
              
        };
    }
}