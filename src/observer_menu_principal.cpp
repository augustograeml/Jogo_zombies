#include "../Observers/observer_menu_principal.h"
#include "../Estados/Menus/menu_principal.h"

namespace Observers
{
    Observer_Menu_Principal::Observer_Menu_Principal() : pMenu(nullptr)
    {
        pGer_Eventos->anexar(this);
    }

    Observer_Menu_Principal::~Observer_Menu_Principal()
    {
        pMenu = nullptr;
    }

    void Observer_Menu_Principal::atualizar(sf::Keyboard::Key k)
    {
        if (!pMenu)
            return;
        if (pGer_Estados->get_estado_atual() != 0)
            return;

        cout << "estado atual menu principal: " << pGer_Estados->get_estado_atual() << endl;

        /*if (k == sf::Keyboard::Down)
            pMenu->baixo(4);

        else if (k == sf::Keyboard::Up)
        {
            pMenu->cima();
        }
        else if (k == sf::Keyboard::Enter)
        {
            pMenu->selecionar();
        }*/
    }
    /*else if(k == sf::Keyboard::Escape && pGer_Estados->get_estado_atual() <)
            pGer_Estados->set_estado_atual*/

    void Observer_Menu_Principal::set_menu(Estados::Menus::Menu_Principal *pM)
    {
        pMenu = pM;
    }
}