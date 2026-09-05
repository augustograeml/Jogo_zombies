#pragma once
#include "menu.h"
#include "../../Observers/observer_menu_jogadores.h"
using namespace std;

namespace Estados
{
    namespace Menus
    {
        class Menu_Jogadores : public Menu
        {
            private:
                bool jogador2;
                Observers::Observer_Menu_Jogadores* pObserver;
            public:
                Menu_Jogadores(int id);
                ~Menu_Jogadores();

                bool get_jogador2();

                void inicializa_valores();
                void loop_evento();
                void selecionar();
        };
    }
}