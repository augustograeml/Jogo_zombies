#pragma once
#include "../Recursos/catalogo.h"
#include "../Recursos/escala.h"
#include <algorithm>
#include <stdexcept>
namespace Testes {
struct Patamar { float x,fim,y; };
inline bool piso(char c) { return c=='0' || c=='7'; }
inline std::vector<Patamar> patamares(const std::vector<std::string>& mapa) {
    std::vector<Patamar> saida;
    for(std::size_t y=0;y<mapa.size();++y) {
        int inicio=-1;
        for(std::size_t x=0;x<=mapa[y].size();++x) {
            const bool topo=x<mapa[y].size() && piso(mapa[y][x]) &&
                (y==0 || x>=mapa[y-1].size() || !piso(mapa[y-1][x]));
            if(topo && inicio<0) inicio=x;
            if(!topo && inicio>=0) { saida.push_back({inicio*50.f,x*50.f,y*50.f});inicio=-1; }
        }
    }
    std::sort(saida.begin(),saida.end(),[](auto a,auto b){return a.x<b.x;});
    return saida;
}
inline std::vector<Patamar> principais(const std::vector<std::string>& mapa) {
    auto saida=patamares(mapa);
    saida.erase(std::remove_if(saida.begin(),saida.end(),[](auto p){return p.fim-p.x<350;}),saida.end());
    return saida;
}
inline void validar_passagens(const std::vector<std::string>& mapa) {
    std::size_t largura=0;for(const auto& l:mapa) largura=std::max(largura,l.size());
    for(std::size_t x=0;x<largura;++x) {
        int anterior=-1;
        for(std::size_t y=0;y<mapa.size();++y) if(x<mapa[y].size() && piso(mapa[y][x])) {
            if(anterior>=0 && int(y)>anterior+1 &&
                (y-anterior-1)*Recursos::Escala::bloco<Recursos::Escala::passagem_minima)
                throw std::runtime_error("Passagem baixa na coluna "+std::to_string(x)+", linha "+std::to_string(y));
            anterior=y;
        }
    }
}
}
