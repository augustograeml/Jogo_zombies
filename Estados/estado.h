#pragma once
#include "../Gerenciadores/gerenciador_estados.h"
#include <SFML/Window/Event.hpp>

namespace Estados
{
    class Estado
    {
        protected:
            int id;
            static Gerenciadores::Gerenciador_Estados* pGE;
        public:
            Estado(int id = -1);
            virtual ~Estado();

            void setID(int num);
            int getID();

            virtual void executar() = 0;
            virtual void tratar_evento(const sf::Event&) {}
            virtual void ao_entrar() {}
    };
}
