#pragma once
#include "observer.h"

namespace Estados
{
    namespace Menus
    {
        class Menu_Principal;
    }
}

namespace Observers
{
    class Observer_Menu_Principal : public Observer
    {
        private:
            Estados::Menus::Menu_Principal* pMenu;
        public:
            Observer_Menu_Principal();
            ~Observer_Menu_Principal();

            void atualizar(sf::Keyboard::Key k);
            void set_menu(Estados::Menus::Menu_Principal* pM);
    };
}