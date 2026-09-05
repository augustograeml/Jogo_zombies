//codigo reaproveitado do Peteco

#include "../Observers/observer_menu_jogadores.h"
#include "../Estados/Menus/menu_jogadores.h"

namespace Observers
{
    Observer_Menu_Jogadores::Observer_Menu_Jogadores() : Observer(), pMenu(nullptr)
    {
        pGer_Eventos->anexar(this);
    }

    Observer_Menu_Jogadores::~Observer_Menu_Jogadores()
    {
        pMenu = nullptr;
    }

    void Observer_Menu_Jogadores::set_menu(Estados::Menus::Menu_Jogadores* pM)
    {
        pMenu = pM;
    }

    void Observer_Menu_Jogadores::atualizar(sf::Keyboard::Key k)
    {
        if(!pMenu)
            return;
        if(pGer_Estados->get_estado_atual() != 1)
            return;

        cout << "estado atual menu jogadores: " << pGer_Estados->get_estado_atual() << endl;
        
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