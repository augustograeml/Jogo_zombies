#pragma once
#include <SFML/Graphics/View.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
namespace Interface {
// Mantem coordenadas logicas quadradas sem deformar texto em janelas retangulares.
inline sf::FloatRect enquadramento(sf::Vector2u pixels) {
    if(!pixels.x || !pixels.y) return {0,0,1,1};
    const float proporcao=static_cast<float>(pixels.x)/pixels.y;
    return proporcao>1 ? sf::FloatRect((1-1/proporcao)/2,0,1/proporcao,1)
                       : sf::FloatRect(0,(1-proporcao)/2,1,proporcao);
}
inline sf::View vista_interface(sf::Vector2u pixels) {
    sf::View vista({512,512},{1024,1024}); vista.setViewport(enquadramento(pixels)); return vista;
}
class CameraDupla {
    bool dividida=false, empilhada=false;
public:
    // Histerese impede alternancia rapida quando a distancia oscila no limite.
    std::vector<sf::View> atualizar(const std::vector<sf::Vector2f>& vivos,
                                    sf::FloatRect mapa, float painel, sf::Vector2u pixels) {
        const float altura=1024-painel;
        if(vivos.size()<2) dividida=false;
        else {
            const auto distancia=vivos[1]-vivos[0];
            const float dx=std::abs(distancia.x)/1024,dy=std::abs(distancia.y)/altura;
            if(!dividida && std::max(dx,dy)>.72f) { dividida=true; empilhada=dy>dx; }
            else if(dividida && std::max(dx,dy)<.58f) dividida=false;
        }
        const auto tela=enquadramento(pixels);
        std::vector<sf::View> vistas;
        const auto adicionar=[&](sf::Vector2f centro,sf::FloatRect area) {
            const sf::Vector2f tamanho(area.width*1024,area.height*1024);
            // Fora dos limites (por exemplo queda fatal), ainda acompanha o jogador.
            const auto limitar=[](float c,float inicio,float fim,float metade) {
                if(c<inicio || c>fim) return c;
                return fim-inicio<2*metade?(inicio+fim)/2:std::clamp(c,inicio+metade,fim-metade);
            };
            centro.x=limitar(centro.x,mapa.left,mapa.left+mapa.width,tamanho.x/2);
            centro.y=limitar(centro.y,mapa.top,mapa.top+mapa.height,tamanho.y/2);
            sf::View vista(centro,tamanho);
            vista.setViewport({tela.left+area.left*tela.width,tela.top+area.top*tela.height,
                               area.width*tela.width,area.height*tela.height});
            vistas.push_back(vista);
        };
        const float topo=painel/1024, h=altura/1024;
        if(dividida) {
            if(empilhada) {
                adicionar(vivos[0],{0,topo,1,h/2}); adicionar(vivos[1],{0,topo+h/2,1,h/2});
            } else {
                adicionar(vivos[0],{0,topo,.5f,h}); adicionar(vivos[1],{.5f,topo,.5f,h});
            }
        } else {
            sf::Vector2f centro(mapa.left+mapa.width/2,mapa.top+mapa.height/2);
            if(!vivos.empty()) { centro={0,0}; for(auto p:vivos) centro+=p; centro/=static_cast<float>(vivos.size()); }
            adicionar(centro,{0,topo,1,h});
        }
        return vistas;
    }
};
}
