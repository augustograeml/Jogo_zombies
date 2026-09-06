#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Menus/nome.h"
#include "../Estados/Fases/fase1.h"
#include "../Persistencia/slots.h"
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
using Persistencia::Json;
using Estados::Fases::Fase1;
sf::Event tecla(sf::Keyboard::Key k) { sf::Event e{}; e.type=sf::Event::KeyPressed; e.key.code=k; return e; }
sf::Event clique(int x,int y) { sf::Event e{}; e.type=sf::Event::MouseButtonReleased; e.mouseButton.button=sf::Mouse::Left; e.mouseButton.x=x; e.mouseButton.y=y; return e; }
int main() {
    namespace fs=std::filesystem;
    const auto pasta=fs::temp_directory_path()/("zombies-menu-saves-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(pasta); fs::current_path(pasta);
    auto* ge=Gerenciadores::Gerenciador_Estados::get_instancia();
    auto* gg=Gerenciadores::Gerenciador_Grafico::get_instancia();
    auto* menu=new Estados::Menus::Menu_Principal(0); ge->adicionar_estado(menu);
    ge->adicionar_estado(new Estados::Menus::Nome(10));
    auto& slots=Persistencia::Slots::instancia();
    Fase1 modelo(6,false); auto snapshot=modelo.capturar(); slots.salvar(snapshot);
    slots.selecionar(2); auto segundo=snapshot; segundo["partida_id"]="slot-dois"; segundo["pontos"]=150; slots.salvar(segundo); slots.selecionar(1);
    auto imagem=[&](const char* nome) { gg->limpar(); menu->executar(); gg->mostrar(); sf::Texture t; t.create(1024,1024); t.update(*gg->get_Janela()); assert(t.copyToImage().saveToFile(nome)); };
    assert(!menu->saves_abertos());
    menu->tratar_evento(tecla(sf::Keyboard::Num2)); menu->tratar_evento(tecla(sf::Keyboard::R));
    assert(slots.selecionado()==1 && !menu->saves_abertos() && slots.ler()==snapshot);
    imagem("menu-inicial.png");
    menu->tratar_evento(tecla(sf::Keyboard::Down)); menu->tratar_evento(tecla(sf::Keyboard::Enter));
    assert(menu->saves_abertos()); imagem("lista-partidas.png");
    menu->tratar_evento(tecla(sf::Keyboard::Enter)); assert(ge->get_estado_atual()==6);
    auto* ativa=static_cast<Fase1*>(ge->get_estado(6)); auto alterada=ativa->capturar(); alterada["pontos"]=75; ativa->restaurar(alterada);
    ge->set_estado_atual(0); assert(!menu->saves_abertos());
    menu->tratar_evento(tecla(sf::Keyboard::Enter)); assert(menu->saves_abertos());
    menu->tratar_evento(tecla(sf::Keyboard::Num2));
    assert(slots.selecionado()==2 && Persistencia::ler_json("partida.json")["pontos"]==75 && slots.ler()==segundo);
    menu->tratar_evento(tecla(sf::Keyboard::Escape)); assert(!menu->saves_abertos());
    // Clique na opcao Continuar e no cartao 2 inicia a partida correspondente.
    menu->tratar_evento(clique(475,778)); assert(menu->saves_abertos());
    menu->tratar_evento(clique(150,480)); assert(ge->get_estado_atual()==6);
    assert(static_cast<Fase1*>(ge->get_estado(6))->get_pontos()==150);
    ge->set_estado_atual(0); menu->tratar_evento(tecla(sf::Keyboard::Enter));
    menu->tratar_evento(tecla(sf::Keyboard::Num3)); assert(slots.selecionado()==3);
    menu->tratar_evento(tecla(sf::Keyboard::Enter)); assert(ge->get_estado_atual()==0 && menu->saves_abertos());
    slots.salvar(snapshot); slots.salvar(segundo);
    { std::ofstream corrompido("partida-slot3.json"); corrompido << "quebrado"; }
    menu->tratar_evento(tecla(sf::Keyboard::R)); assert(slots.ler()==snapshot && fs::exists("partida-slot3.json.corrompido-1"));
    // Partida encerrada com nomes pendentes deve retornar a tela de nomes.
    auto pendente=snapshot; pendente["finalizada"]=true; pendente["vitoria"]=false; slots.salvar(pendente);
    menu->tratar_evento(tecla(sf::Keyboard::Enter)); assert(ge->get_estado_atual()==10);
    assert(!static_cast<Fase1*>(ge->get_estado(6))->get_nomes_confirmados());
    ge->set_estado_atual(0); menu->tratar_evento(tecla(sf::Keyboard::Enter));
    menu->tratar_evento(tecla(sf::Keyboard::Enter)); assert(ge->get_estado_atual()==10);
    std::cout<<"Menu saves: inicial oculto, teclado, mouse, troca segura, recovery e nome pendente OK. Imagens: "<<pasta<<'\n';
}
