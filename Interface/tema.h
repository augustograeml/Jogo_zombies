#pragma once
#include "preferencias.h"
namespace Interface::Tema {
inline sf::Color fundo() { return Preferencias::instancia().contraste?sf::Color::Black:sf::Color(16,23,21,238); }
inline sf::Color tinta() { return sf::Color(229,233,223); }
inline sf::Color secundaria() { return sf::Color(181,191,181); }
inline sf::Color acento() { return Preferencias::instancia().contraste?sf::Color::White:sf::Color(145,167,122); }
inline void painel(sf::RenderTarget& alvo,sf::FloatRect area) {
    sf::RectangleShape p({area.width,area.height}); p.setPosition(area.left,area.top);
    p.setFillColor(fundo()); p.setOutlineColor({80,96,82}); p.setOutlineThickness(-1); alvo.draw(p);
}
inline void estilo_titulo(sf::Text& t) {
    t.setFont(fonte_titulo()); t.setFillColor(tinta()); t.setLetterSpacing(1.12f);
    t.setOutlineColor({10,16,14,210}); t.setOutlineThickness(1.f);
}
inline void titulo(sf::RenderTarget& alvo,const std::string& valor,float y,unsigned tamanho=72) {
    sf::Text t(valor,fonte_titulo(),tamanho); estilo_titulo(t);
    const auto r=t.getLocalBounds(); const float escala=std::min(1.f,840/std::max(1.f,r.width));
    t.setOrigin(r.left+r.width/2,r.top); t.setScale(escala,escala); t.setPosition(512,y);
    auto sombra=t; sombra.move(2,3); sombra.setFillColor({5,10,8,190}); alvo.draw(sombra); alvo.draw(t);
    sf::RectangleShape linha({80,3}); linha.setPosition(472,y+r.height*escala+22); linha.setFillColor(acento()); alvo.draw(linha);
}
inline void botao(sf::RenderTarget& alvo,sf::FloatRect area,const std::string& valor,bool ativo,unsigned tamanho=20) {
    sf::RectangleShape b({area.width,area.height}); b.setPosition(area.left,area.top);
    const bool contraste=Preferencias::instancia().contraste;
    b.setFillColor(ativo?(contraste?sf::Color::White:sf::Color(39,52,35,250)):fundo()); b.setOutlineColor(ativo?acento():sf::Color(80,96,82)); b.setOutlineThickness(-1); alvo.draw(b);
    if(ativo) { sf::RectangleShape marca({4,area.height-14}); marca.setPosition(area.left+7,area.top+7); marca.setFillColor(contraste?sf::Color::Black:acento()); alvo.draw(marca); }
    sf::Text t(valor,fonte_interface(),tamanho+2); t.setLetterSpacing(1.08f); t.setFillColor(ativo?(contraste?sf::Color::Black:sf::Color(199,218,174)):tinta());
    const auto r=t.getLocalBounds(); const float s=std::min(Preferencias::instancia().escala,(area.width-28)/std::max(1.f,r.width));
    t.setOrigin(r.left+r.width/2,r.top+r.height/2); t.setScale(s,s); t.setPosition(area.left+area.width/2,area.top+area.height/2); alvo.draw(t);
}
}
