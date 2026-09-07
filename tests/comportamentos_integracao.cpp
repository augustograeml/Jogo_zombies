#include "../Persistencia/entidades.h"
#include "../Entidades/Personagens/gigante.h"
#include "../Entidades/Personagens/zumbi.h"
#include <cassert>
#include <iostream>

int main() {
    using namespace Entidades::Personagens;
    using Persistencia::Serializador;
    const std::vector<Logica::Alvo> alvos{{1,40,0,true}};
    Gigante gigante;
    Jogador jogador({0,0},{0,0},false);
    const int vida=jogador.get_vida();
    gigante.atacar(&jogador); assert(jogador.get_vida()==vida);
    auto ataque=gigante.get_comportamento();
    ataque.acao=Logica::AcaoCorpo::Preparando; ataque.alvo=1;
    gigante.restaurar_comportamento(ataque);
    gigante.atacar(&jogador); assert(jogador.get_vida()==vida);
    ataque.acao=Logica::AcaoCorpo::Golpe; gigante.restaurar_comportamento(ataque);
    gigante.atacar(&jogador); assert(jogador.get_vida()<vida);
    gigante.restaurar_comportamento({});
    for(int i=0;i<20;++i) { gigante.perceber(alvos); gigante.set_nochao(true); gigante.executar(); }
    assert(gigante.get_comportamento().acao==Logica::AcaoCorpo::Preparando);
    auto salvo=Serializador::salvar(gigante);
    auto copia=Serializador::carregar(salvo);
    auto* retomado=dynamic_cast<Gigante*>(copia.get());
    assert(retomado && Serializador::salvar(*retomado)==salvo);
    for(int i=0;i<100;++i) {
        gigante.perceber(alvos); retomado->perceber(alvos);
        gigante.set_nochao(true); retomado->set_nochao(true);
        gigante.executar(); retomado->executar();
        assert(Serializador::salvar(gigante)==Serializador::salvar(*retomado));
    }
    salvo["extra"].erase("comportamento");
    auto legado=Serializador::carregar(salvo);
    assert(dynamic_cast<Gigante*>(legado.get())->get_comportamento().acao==Logica::AcaoCorpo::Patrulha);
    salvo=Serializador::salvar(gigante);
    salvo["extra"]["comportamento"]["passos"]=999;
    bool rejeitou=false;
    try { Serializador::carregar(salvo); } catch(...) { rejeitou=true; }
    assert(rejeitou);
    Zumbi zumbi;
    zumbi.perceber(alvos); zumbi.set_nochao(true); zumbi.executar();
    assert(zumbi.get_comportamento().acao==Logica::AcaoCorpo::Perseguindo);
    auto z=Serializador::carregar(Serializador::salvar(zumbi));
    assert(Serializador::salvar(*z)==Serializador::salvar(zumbi));
    zumbi.perceber({{1,40,0,false}}); zumbi.executar();
    assert(zumbi.get_comportamento().alvo==-1);
    std::cout << "Comportamentos: snapshot, migracao e retomada de 100 passos OK\n";
}
