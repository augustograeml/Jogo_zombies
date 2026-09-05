#pragma once

#include "menu.h"
#include "../../Observers/observer_menu_fase.h"

using namespace std;

namespace Estados
{
    namespace Menus
    {
        class Menu_Fase : public Menu
        {
            private:
                Observers::Observer_Menu_Fase* pObserver;
            public:
                Menu_Fase(int id, bool jog2);
                ~Menu_Fase();

                void inicializa_valores();
                void loop_evento();
                void selecionar();
        };
    }
}