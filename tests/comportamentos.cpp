#include "../Logica/corpo_a_corpo.h"
#include <cassert>
#include <iostream>
int main() {
    using namespace Logica;
    EstadoCorpo z;
    decidir_zumbi(z,0,0,{{1,180,0,true},{2,30,0,false}});
    assert(z.acao==AcaoCorpo::Perseguindo && z.alvo==1);
    decidir_zumbi(z,210,0,{{1,350,0,true}});
    assert(z.alvo==-1 && !z.direita);
    decidir_zumbi(z,0,0,{{1,30,0,false}}); assert(z.alvo==-1);
    EstadoCorpo g;
    std::vector<Alvo> alvos{{1,50,0,true}};
    decidir_gigante(g,0,0,alvos);
    assert(g.acao==AcaoCorpo::Preparando);
    for(int i=0;i<20;++i) decidir_gigante(g,0,0,alvos);
    auto retomado=g;
    for(int i=0;i<22;++i) {
        decidir_gigante(g,0,0,alvos); decidir_gigante(retomado,0,0,alvos);
        assert(g.acao==retomado.acao && g.passos==retomado.passos);
    }
    assert(g.acao==AcaoCorpo::Golpe);
    alvos[0].x=-50;
    decidir_gigante(g,0,0,alvos); assert(g.direita); // Golpe nao segue o alvo.
    for(int i=0;i<11;++i) decidir_gigante(g,0,0,alvos);
    assert(g.acao==AcaoCorpo::Recuperando && g.alvo==-1);
    for(int i=0;i<60;++i) decidir_gigante(g,0,0,alvos);
    assert(g.acao==AcaoCorpo::Patrulha);
    decidir_gigante(g,0,0,alvos);
    alvos[0].vivo=false; decidir_gigante(g,0,0,alvos);
    assert(g.alvo==-1 && g.acao==AcaoCorpo::Recuperando);
    std::cout << "Perseguicao limitada, antecipacao, golpe e recuperacao: OK\n";
}
