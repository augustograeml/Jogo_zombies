#pragma once
namespace Persistencia { class Serializador; }
#include "inimigo.h"
#include "../../Logica/corpo_a_corpo.h"

namespace Entidades
{
    namespace Personagens
    {
        class Zumbi : public Inimigo
        {
        friend class Persistencia::Serializador;
            private:
                Logica::EstadoCorpo comportamento;
                std::vector<Logica::Alvo> alvos;
                int pulo;
                int numero_zumbi_salvos;
            public:
                Zumbi(Vector2f pos = Vector2f(0.f, 0.f), Vector2f vel = Vector2f(0.f, 0.f));
                ~Zumbi();

                void mover();
                void atualizar();
                void executar();
                void perceber(const std::vector<Logica::Alvo>& jogadores) override { alvos=jogadores; }
                const Logica::EstadoCorpo& get_comportamento() const { return comportamento; }
                void restaurar_comportamento(Logica::EstadoCorpo e) { comportamento=e; }
                void colidir(Entidade* pE, int a);
                void atacar(Entidade* jg);
                std::vector<Projetil>* get_projeteis() {return nullptr;}
                void salvar(std::ostringstream* entrada);
                
        };
    }
}