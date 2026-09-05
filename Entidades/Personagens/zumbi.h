#pragma once
namespace Persistencia { class Serializador; }
#include "inimigo.h"

namespace Entidades
{
    namespace Personagens
    {
        class Zumbi : public Inimigo
        {
        friend class Persistencia::Serializador;
            private:
                int pulo;
                int numero_zumbi_salvos;
            public:
                Zumbi(Vector2f pos = Vector2f(0.f, 0.f), Vector2f vel = Vector2f(0.f, 0.f));
                ~Zumbi();

                void mover();
                void atualizar();
                void executar();
                void colidir(Entidade* pE, int a);
                void atacar(Entidade* jg);
                std::vector<Projetil>* get_projeteis() {return nullptr;}
                void salvar(std::ostringstream* entrada);
                
        };
    }
}