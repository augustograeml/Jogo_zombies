#include "../Interface/camera.h"
#include <cassert>
#include <iostream>
static bool contem(const sf::View& v,sf::Vector2f p) {
    const auto canto=v.getCenter()-v.getSize()/2.f;
    return sf::FloatRect(canto,v.getSize()).contains(p);
}
int main() {
    Interface::CameraDupla c; const sf::FloatRect mapa(0,0,4000,4000);
    auto v=c.atualizar({{50,50},{200,50}},mapa,190,{1024,1024});
    assert(v.size()==1 && contem(v[0],{50,50}) && contem(v[0],{200,50}));
    v=c.atualizar({{50,50},{3900,50}},mapa,190,{1600,900});
    assert(v.size()==2 && contem(v[0],{50,50}) && contem(v[1],{3900,50}));
    assert(v[0].getViewport().left+v[0].getViewport().width<=v[1].getViewport().left+.001f);
    v=c.atualizar({{1000,1000},{1650,1000}},mapa,190,{900,1600}); assert(v.size()==2);
    v=c.atualizar({{1000,1000},{1500,1000}},mapa,190,{900,1600}); assert(v.size()==1);
    v=c.atualizar({{100,50},{100,3900}},mapa,247,{900,1600});
    assert(v.size()==2 && contem(v[0],{100,50}) && contem(v[1],{100,3900}));
    assert(v[0].getViewport().top+v[0].getViewport().height<=v[1].getViewport().top+.001f);
    v=c.atualizar({{3900,3900}},mapa,247,{900,1600}); assert(v.size()==1 && contem(v[0],{3900,3900}));
    for(auto pixels:{sf::Vector2u(1600,900),sf::Vector2u(900,1600),sf::Vector2u(640,480)}) {
        auto ui=Interface::vista_interface(pixels); auto r=ui.getViewport();
        assert(std::abs(r.width*pixels.x-r.height*pixels.y)<.01f);
    }
    std::cout << "Camera: separacao horizontal/vertical, bordas, jogador unico, histerese e proporcoes OK\n";
}
