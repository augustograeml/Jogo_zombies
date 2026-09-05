//codigo reaproveitado do Peteco

#pragma once
#include "observer.h"

namespace Observers
{
    class Observer_Tecla : public Observer
    {
        private:
            const int numero;
        public:
            Observer_Tecla(int n = 0);
            ~Observer_Tecla();

            void atualizar(sf::Keyboard::Key k);
            void tecla_pressionada(const sf::Keyboard::Key tecla);
            void tecla_solta(const sf::Keyboard::Key tecla);
    };

}