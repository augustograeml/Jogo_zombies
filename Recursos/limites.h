#pragma once
#include "../Listas/listaEntidades.h"
#include "../Entidades/Obstaculos/neve.h"
#include "../Entidades/Obstaculos/musgo.h"
#include "escala.h"
namespace Recursos {
// Deriva da geometria persistida, inclusive ao retomar mapas de versoes anteriores.
inline sf::FloatRect limites_mundo(Listas::ListaEntidade& obstaculos) {
    float esquerda=0,topo=0,direita=0,baixo=0;
    for(auto it=obstaculos.get_primeiro();it!=nullptr;++it) {
        if(!dynamic_cast<Entidades::Obstaculos::Neve*>(*it) &&
           !dynamic_cast<Entidades::Obstaculos::Musgo*>(*it)) continue;
        const auto r=(*it)->get_corpo()->getGlobalBounds();
        esquerda=std::min(esquerda,r.left); topo=std::min(topo,r.top);
        direita=std::max(direita,r.left+r.width); baixo=std::max(baixo,r.top+r.height);
    }
    if(direita==0 && baixo==0) return {0,0,2000,2000}; // Fixtures/partidas antigas sem terreno.
    return {esquerda,topo,std::max(Escala::bloco,direita-esquerda),std::max(Escala::bloco,baixo-topo)};
}
}
