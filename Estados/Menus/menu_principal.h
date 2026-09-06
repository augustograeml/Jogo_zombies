#pragma once

#include "menu.h"
#include "../../Observers/observer_menu_principal.h"
using namespace std;

namespace Estados
{
    namespace Menus
    {
        class Menu_Principal : public Menu
        {
            private:
                bool jacriado;
                bool escolhendo_partida=false;
                void escolher_slot(int numero);
                sf::Font fonte_slots;
                std::vector<std::string> resumos;
                void atualizar_slots();
                Observers::Observer_Menu_Principal* pObserver;
            public:
                Menu_Principal(int id);
                ~Menu_Principal();

                void inicializa_valores();
                void loop_evento();
                void selecionar();
                void fase_salva();
                bool saves_abertos() const { return escolhendo_partida; }
                void ao_entrar() override;
                void executar() override;
                void tratar_evento(const sf::Event&) override;
        };
    }
}
