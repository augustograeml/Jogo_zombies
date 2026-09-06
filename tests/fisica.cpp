#include "../Fisica/aabb.h"
#include <cassert>
#include <iostream>
int main() {
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
    std::cout << "6 verificacoes de AABB e apoio passaram sem janela.\n";
}
