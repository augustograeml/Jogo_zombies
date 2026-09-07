#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Menus/menu_jogadores.h"
#include "../Estados/Menus/menu_fase.h"
#include "../Interface/preferencias.h"
#include <cassert>
#include <iostream>
int main(){
    auto* g=Gerenciadores::Gerenciador_Grafico::get_instancia(); auto* j=g->get_Janela();
    Estados::Menus::Menu_Principal principal(0);
    Estados::Menus::Menu_Jogadores jogadores(1);
    Estados::Menus::Menu_Fase fases(2,false);
    int n=0;
    for(auto* menu:{static_cast<Estados::Menus::Menu*>(&principal),static_cast<Estados::Menus::Menu*>(&jogadores),static_cast<Estados::Menus::Menu*>(&fases)}) {
        j->clear(); menu->mostrar_menu(); j->display();
        sf::Texture t; t.create(1024,1024); t.update(*j); t.copyToImage().saveToFile("/tmp/zombies-design-"+std::to_string(n++)+".png");
        const auto a=menu->area_opcao(1),b=menu->area_opcao(2); assert(!a.intersects(b));
        menu->baixo(2); Interface::Preferencias::instancia().escala=1.3f;
        j->clear();menu->mostrar_menu();j->display(); Interface::Preferencias::instancia().escala=1;
    }
    std::cout<<"Botoes legiveis: tres menus, opcoes separadas e escala ampliada OK\n";
}
