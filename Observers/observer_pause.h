#pragma once
#include "observer.h"

namespace Estados
{
    namespace Menus
    {
        class Pause;
    }
}

namespace Observers
{
    class Observer_Pause : public Observer
    {
        private:
            Estados::Menus::Pause* pMenu;
        public:
            Observer_Pause();
            ~Observer_Pause();

            void atualizar(sf::Keyboard::Key k);
            void set_menu(Estados::Menus::Pause* pM);

            
    };
}