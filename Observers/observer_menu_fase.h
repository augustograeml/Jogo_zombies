//codigo reaproveitado do Peteco

#pragma once
#include "observer.h"

namespace Estados
{
    namespace Menus
    {
        class Menu_Fase;
    }
}

namespace Observers
{
    class Observer_Menu_Fase : public Observer
    {
        private:
            Estados::Menus::Menu_Fase* pMenu;
        public:
            Observer_Menu_Fase();
            ~Observer_Menu_Fase();

            void atualizar(sf::Keyboard::Key k);
            void set_menu(Estados::Menus::Menu_Fase* pM);
    };
}

