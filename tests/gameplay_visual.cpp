#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Estados/Menus/pause.h"
#include "../Interface/preferencias.h"
#include <cassert>
#include <filesystem>
#include <iostream>
template<class Base> struct Cena:Base {
    Cena(int id):Base(id,false){this->ao_entrar();}
    void desenhar() {this->desenhar_partida();}
    void reduzir() {int n=0;for(auto it=this->inimigos.get_primeiro();it!=nullptr;++it)if(n++>1)(*it)->morrer();}
};
int main() {
    auto* g=Gerenciadores::Gerenciador_Grafico::get_instancia();auto& janela=*g->get_Janela();
    const auto pasta=std::filesystem::temp_directory_path()/"zombies-gameplay-visual";std::filesystem::create_directories(pasta);
    auto foto=[&](std::string nome) {janela.display();sf::Texture t;assert(t.create(janela.getSize().x,janela.getSize().y));t.update(janela);assert(t.copyToImage().saveToFile((pasta/nome).string()));};
    Cena<Estados::Fases::Fase1> neve(7);Cena<Estados::Fases::Fase2> musgo(9);
    neve.reduzir();janela.clear();neve.desenhar();foto("hud-neve.png");
    musgo.reduzir();janela.clear();musgo.desenhar();foto("hud-musgo.png");
    Estados::Menus::Pause pause(5);janela.clear();pause.mostrar_menu();foto("pausa.png");
    sf::Event e{};e.type=sf::Event::KeyPressed;e.key.code=sf::Keyboard::C;pause.tratar_evento(e);
    janela.clear();pause.mostrar_menu();foto("controles.png");
    Interface::Preferencias::instancia().escala=1.3f;janela.clear();pause.mostrar_menu();foto("controles-ampliados.png");
    std::cout<<pasta<<'\n';
}
