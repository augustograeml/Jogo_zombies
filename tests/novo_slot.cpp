#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Menus/menu_jogadores.h"
#include "../Estados/Fases/fase1.h"
#include "../Persistencia/slots.h"
#include <cassert>
#include <chrono>
#include <iostream>

static sf::Event tecla(sf::Keyboard::Key k) {
    sf::Event e{}; e.type=sf::Event::KeyPressed; e.key.code=k; return e;
}
int main() {
    namespace fs=std::filesystem;
    const auto pasta=fs::temp_directory_path()/("zombies-novo-slot-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(pasta); fs::current_path(pasta);
    auto* ge=Gerenciadores::Gerenciador_Estados::get_instancia();
    auto* menu=new Estados::Menus::Menu_Principal(0);
    ge->adicionar_estado(menu); ge->adicionar_estado(new Estados::Menus::Menu_Jogadores(1));
    auto& slots=Persistencia::Slots::instancia();
    Estados::Fases::Fase1 modelo(6,false); auto primeiro=modelo.capturar();
    slots.salvar(primeiro); auto anterior=primeiro; anterior["pontos"]=100;
    slots.salvar(anterior);
    menu->tratar_evento(tecla(sf::Keyboard::Enter));
    assert(menu->destinos_abertos() && menu->destino_selecionado()==2 && !menu->saves_abertos());
    assert(slots.selecionado()==1);
    menu->tratar_evento(tecla(sf::Keyboard::Escape));
    assert(!menu->destinos_abertos() && slots.ler()==anterior);
    menu->tratar_evento(tecla(sf::Keyboard::Enter));
    menu->tratar_evento(tecla(sf::Keyboard::Num3));
    menu->tratar_evento(tecla(sf::Keyboard::Enter));
    assert(ge->get_estado_atual()==1 && slots.selecionado()==3);
    assert(!fs::exists("partida-slot3.json"));
    assert(Persistencia::ler_json("partida.json")==anterior);
    assert(Persistencia::ler_json("partida.json.bak")==primeiro);
    ge->set_estado_atual(0); menu->tratar_evento(tecla(sf::Keyboard::Enter));
    menu->tratar_evento(tecla(sf::Keyboard::Num1)); menu->tratar_evento(tecla(sf::Keyboard::Enter));
    assert(ge->get_estado_atual()==1 && slots.selecionado()==1);
    assert(slots.ler()==anterior && Persistencia::ler_json("partida.json.bak")==primeiro);
    auto nova=primeiro; nova["partida_id"]="nova-partida"; slots.salvar(nova);
    assert(slots.ler()==nova && Persistencia::ler_json("partida.json.bak")==anterior);
    assert(!fs::exists("partida-slot2.json") && !fs::exists("partida-slot3.json"));
    std::cout<<"Novo jogo: sugestao vazia, escolha de destino, cancelamento, escolha direta e backup OK.\n";
}
