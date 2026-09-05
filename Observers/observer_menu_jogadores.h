#pragma once
#include "observer.h"

namespace Estados
{
    namespace Menus
    {
        class Menu_Jogadores;
    }
}

namespace Observers
{
    class Observer_Menu_Jogadores : public Observer
    {
        private:
            Estados::Menus::Menu_Jogadores* pMenu;
        public:
            Observer_Menu_Jogadores();
            ~Observer_Menu_Jogadores();

            void atualizar(sf::Keyboard::Key k);
            void set_menu(Estados::Menus::Menu_Jogadores* pM);
    };
}