#include "../Fisica/aabb.h"
#include "../Logica/movimento.h"
#include <cassert>
#include <iostream>
int main() {
    {
        Logica::EstadoMovimento e;
        assert(!Logica::solicitar_salto(e,true,false));
        assert(!Logica::solicitar_salto(e,false,false));
        assert(Logica::solicitar_salto(e,false,true));
        assert(!Logica::solicitar_salto(e,false,true));
        Logica::EstadoMovimento buffer;
        assert(!Logica::solicitar_salto(buffer,false,true));
        assert(Logica::solicitar_salto(buffer,true,false));
        Logica::EstadoMovimento expirado;
        Logica::solicitar_salto(expirado,true,false);
        for(int n=0;n<7;++n) Logica::solicitar_salto(expirado,false,false);
        assert(!Logica::solicitar_salto(expirado,false,true));
    }

    using Fisica::resolver;
    assert(resolver({10,0,50,50},{0,50,100,20},0).lado==4);
    auto c=resolver({10,10,50,50},{0,50,100,20},1);
    assert(c.lado==4 && c.dy==-10);
    c=resolver({90,55,50,50},{0,50,100,100},0);
    assert(c.lado==1 && c.dx==10);
    c=resolver({10,65,50,50},{0,50,100,20},-1);
    assert(c.lado==2 && c.dy==5);
    assert(resolver({100,0,50,50},{0,50,100,20},0).lado==0);
    assert(resolver({10,0,50,50},{0,50,100,20},-1).lado==0);
    Logica::EstadoMovimento queda;
    Logica::acompanhar_queda(queda,100,false);
    Logica::acompanhar_queda(queda,50,false); // Apice real, mesmo que o salto comece abaixo.
    assert(Logica::dano_queda(queda,350,10)==10); // Exige mais de seis blocos.
    assert(Logica::dano_queda(queda,700,10)==10); // Bonus consumido.
    Logica::acompanhar_queda(queda,50,false);
    assert(Logica::dano_queda(queda,351,10)==20);
    Logica::acompanhar_queda(queda,50,false);
    assert(Logica::dano_queda(queda,551,10)==30);
    Logica::acompanhar_queda(queda,50,false);
    Logica::acompanhar_queda(queda,600,true); // Pousar descarta a queda.
    assert(Logica::dano_queda(queda,601,10)==10);
    Logica::Velocidade gelo{4,0}, normal{4,0};
    for(int i=0;i<20;++i) {
        gelo=Logica::mover(gelo,true,false,false,false,false,true);
        normal=Logica::mover(normal,true,false,false,false,false,false);
    }
    assert(gelo.x>3 && normal.x<.01f);
    for(int i=0;i<200;++i) gelo=Logica::mover(gelo,true,false,false,false,false,true);
    assert(gelo.x==0); // Desliza mais, mas ainda permite parar completamente.
    Logica::Velocidade inversao{4,0}, comum{4,0};
    float distancia=0;Logica::Velocidade solto{4,0};
    for(int i=0;i<40;++i) {
        inversao=Logica::mover(inversao,true,true,false,false,false,true);
        comum=Logica::mover(comum,true,true,false,false,false,false);
    }
    assert(inversao.x>1 && std::abs(comum.x)<.01f);
    for(int i=0;i<210;++i) { solto=Logica::mover(solto,true,false,false,false,false,true);distancia+=solto.x; }
    assert(solto.x==0 && distancia>380 && distancia<410);
    assert(Logica::mover({4,0},true,false,false,true,false,true).y==-6);
    std::cout << "AABB, apoio, dano de queda e freio no gelo passaram sem janela.\n";
}
