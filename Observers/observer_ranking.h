#pragma once
#include "observer.h"

namespace Estados
{
    namespace Menus
    {
        class Ranking;
    }
}

namespace Observers
{
    class Observer_Ranking : public Observer
    {
        private:
            Estados::Menus::Ranking* pMenu;
        public:
            Observer_Ranking();
            ~Observer_Ranking();

            void atualizar(sf::Keyboard::Key k);
            void set_menu(Estados::Menus::Ranking* pM);
    };
}