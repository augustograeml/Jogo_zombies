#include "../Observers/observer_ranking.h"
#include "../Estados/Menus/ranking.h"
#include <iostream>

namespace Observers
{
    Observer_Ranking::Observer_Ranking() : Observer(), pMenu(nullptr)
    {
        pGer_Eventos->anexar(this);
    }

    Observer_Ranking::~Observer_Ranking()
    {
        pMenu = nullptr;
    }

    void Observer_Ranking::set_menu(Estados::Menus::Ranking* pM)
    {
        pMenu = pM;
    }

    void Observer_Ranking::atualizar(sf::Keyboard::Key k)
    {
        if(!pMenu)
            return;
        if(pGer_Estados->get_estado_atual() != 4)
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