#include "../Gerenciadores/gerenciador_colisoes.h"
#include "../Entidades/Personagens/jogador.h"
#include "../Entidades/Obstaculos/neve.h"
#include "../Entidades/Obstaculos/musgo.h"
#include "../Entidades/Obstaculos/caixa.h"
#include <cassert>
#include <cmath>
#include <iostream>
using Entidades::Personagens::Jogador;
using namespace Entidades::Obstaculos;
int main() {
    Listas::ListaEntidade jogadores,obstaculos,inimigos;
    Gerenciadores::Gerenciador_Colisoes g;
    g.set_jogadores(&jogadores); g.set_obstaculos(&obstaculos); g.set_inimigos(&inimigos);
    auto* j=new Jogador({25,0},{1,0},false); jogadores.incluir(j);
    obstaculos.incluir(new Neve({0,50})); obstaculos.incluir(new Neve({50,50}));
    g.gerenciar_colisoes();
    assert(std::abs(j->getVelocidade().x-1.02f)<.0001f); // Dois blocos: aplica uma vez.
    assert(j->getVelocidade().y==0 && j->getPosicao().y==0);
    for(int i=0;i<100;++i) { j->mover_com_controles(false,false,false,false); g.gerenciar_colisoes(); }
    assert(j->getVelocidade().x==0 && j->getPosicao().y==0);
    obstaculos.limpar(); g.invalidar_grade();
    obstaculos.incluir(new Musgo({0,50})); obstaculos.incluir(new Musgo({50,50}));
    j->setPosicao({25,0}); j->setVelocidade({3,0}); g.gerenciar_colisoes();
    assert(std::abs(j->getVelocidade().x-2.f)<.0001f);
    // Contato lateral nao aplica o modificador de superficie.
    j->setPosicao({-40,55}); j->setVelocidade({2,0}); g.gerenciar_colisoes();
    assert(j->getVelocidade().x==0);
    // Mesma ordem sequencial depois de deslocamento entre celulas.
    obstaculos.limpar(); g.invalidar_grade();
    auto* segundo=new Caixa({0,0}); segundo->get_corpo()->setSize({80,200}); obstaculos.incluir(segundo);
    auto* primeiro=new Caixa({102,0}); primeiro->get_corpo()->setSize({150,200}); obstaculos.incluir(primeiro);
    j->setPosicao({101,20}); j->setVelocidade({0,0}); g.ativar_grade(false); g.gerenciar_colisoes();
    const auto esperado=j->getPosicao();
    j->setPosicao({101,20}); j->setVelocidade({0,0}); g.ativar_grade(true); g.gerenciar_colisoes();
    if(j->getPosicao()!=esperado) {
        std::cerr << "Grade diverge: x="<<j->getPosicao().x<<", sequencial="<<esperado.x<<'\n'; return 1;
    }
    std::cout<<"Superficies, apoio continuo e grade com deslocamento entre celulas OK.\n";
}
