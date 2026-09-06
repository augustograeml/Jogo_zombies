#pragma once
#include "arquivo.h"
namespace Persistencia {
// A versao 2 acrescenta pontos, animacao, protecao e IA. A geometria e o RNG
// da versao 1 permanecem intactos; campos de entidade ausentes recebem defaults
// documentados no Serializador. Versoes futuras nunca sao adivinhadas.
inline Json migrar_partida(const Json& origem) {
    inteiro(origem.at("versao"),1,2);
    Json dados=origem;
    dados["versao"]=2;
    if(!dados.contains("pontos")) dados["pontos"]=0;
    if(!dados.contains("passos_anteriores")) dados["passos_anteriores"]=0;
    if(!dados.contains("nomes_confirmados")) dados["nomes_confirmados"]=dados.at("ranking_registrado");
    return dados;
}
}
