//codigo reaproveitado do Peteco

#include "../Observers/observer_menu_fase.h"
#include "../Estados/Menus/menu_fase.h"

namespace Observers
{
    Observer_Menu_Fase::Observer_Menu_Fase() : Observer(), pMenu(nullptr)
    {
        pGer_Eventos->anexar(this);
    }

    Observer_Menu_Fase::~Observer_Menu_Fase()
    {
        pMenu = nullptr;
    }

    void Observer_Menu_Fase::set_menu(Estados::Menus::Menu_Fase* pM)
    {
        pMenu = pM;
    }

    void Observer_Menu_Fase::atualizar(sf::Keyboard::Key k)
    {
        if(!pMenu && pGer_Estados->get_estado_atual() != 0)
            return;
        if(pGer_Estados->get_estado_atual() != 4 && pGer_Estados->get_estado_atual() != 3)
            return;
        
        cout << "estado atual menu fase: " << pGer_Estados->get_estado_atual() << endl;
        
        /*if(k == sf::Keyboard::Down || k == sf::Keyboard::Right)
        {
            //fazer o menu ir pra opcao de baixo
            pMenu->baixo(3);
        }

        else if(k == sf::Keyboard::Up || k == sf::Keyboard::Left)
        {
            //fazer o menu ir pra opcao de cima
            pMenu->cima();
        }
        else if(k == sf::Keyboard::Enter)
        {
            //fazer o menu selecioar a opcao
            pMenu->selecionar();
        }*/
    }

}