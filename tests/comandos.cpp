#include "../Estados/Fases/fase1.h"
#include <cassert>
#include "../Persistencia/entidades.h"
#include "../Logica/objetivo.h"
int main() {
    assert(Logica::orientar({0,0},{{200,0}})=="Direita");
    assert(Logica::orientar({400,0},{{200,0}})=="Esquerda");
    assert(Logica::orientar({0,0},{} )=="Area limpa");
    assert(Logica::orientar({0,0},{{0,-200}})=="Perto / acima");
    Entidades::Personagens::Jogador salto({0,0},{0,0},false);
    salto.set_nochao(true);salto.mover_com_controles(false,false,false,false);
    const auto salvo=Persistencia::Serializador::salvar(salto);
    auto copia=Persistencia::Serializador::carregar(salvo);
    auto* retomado=dynamic_cast<Entidades::Personagens::Jogador*>(copia.get());assert(retomado);
    salto.mover_com_controles(false,false,true,false);retomado->mover_com_controles(false,false,true,false);
    assert(salto.getVelocidade().y==-6 && Persistencia::Serializador::salvar(salto)==Persistencia::Serializador::salvar(*retomado));
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
