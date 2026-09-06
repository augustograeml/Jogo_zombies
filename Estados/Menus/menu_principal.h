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
                bool escolhendo_destino=false, confirmando_substituicao=false;
                int destino_novo=1;
                void iniciar_novo();
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
                bool destinos_abertos() const { return escolhendo_destino; }
                int destino_selecionado() const { return destino_novo; }
                bool substituicao_pendente() const { return confirmando_substituicao; }
                void ao_entrar() override;
                void executar() override;
                void tratar_evento(const sf::Event&) override;
        };
    }
}
