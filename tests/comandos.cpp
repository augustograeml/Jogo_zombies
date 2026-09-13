#include "../Estados/Fases/fase1.h"
#include <cassert>
int main() {
    Estados::Fases::Fase1 a(7,false),b(7,true);
    b.restaurar(a.capturar());
    Logica::Comandos c{};c[0].direita=true;c[1].esquerda=true;
    for(int i=0;i<12;++i) {a.simular_passo(c);b.simular_passo(c);}
    assert(a.capturar()==b.capturar());
    auto j=a.capturar()["jogadores"];
    assert(j[0]["velocidade"][0].get<float>()>0 && j[1]["velocidade"][0].get<float>()<0);
    auto inicial=a.capturar();b.restaurar(inicial);
    a.simular_passo();b.simular_passo({});assert(a.capturar()==b.capturar());
}
