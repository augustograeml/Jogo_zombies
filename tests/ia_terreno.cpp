#include "../Logica/mundo.h"
#include "../Entidades/Personagens/arqueiro.h"
#include "../Entidades/Personagens/zumbi.h"
#include "../Entidades/Obstaculos/neve.h"
#include <cassert>
int main() {
    Listas::ListaEntidade jogadores,inimigos,terreno;
    Gerenciadores::Gerenciador_Colisoes g;
    g.set_jogadores(&jogadores);g.set_inimigos(&inimigos);g.set_obstaculos(&terreno);
    auto* piso=new Entidades::Obstaculos::Neve({0,200});piso->get_corpo()->setSize({500,50});terreno.incluir(piso);
    auto* parede=new Entidades::Obstaculos::Neve({250,50});parede->get_corpo()->setSize({50,150});terreno.incluir(parede);
    auto* j=new Entidades::Personagens::Jogador({400,120},{0,0},false);jogadores.incluir(j);
    auto* a=new Entidades::Personagens::Arqueiro({100,116},{0,0});inimigos.incluir(a);
    Logica::Sessao sessao;Logica::ResultadoPartida resultado;Logica::Pontuacao pontos;Logica::Eventos eventos;
    std::mt19937 motor(1);std::string id="ia";
    Logica::Mundo m{jogadores,inimigos,terreno,g,sessao,resultado,pontos,eventos,motor,id,6,1};
    for(int i=0;i<100;++i) {Logica::simular(m);assert(!a->disparou_no_passo());}
    parede->morrer();bool disparou=false;
    for(int i=0;i<90;++i) {Logica::simular(m);disparou|=a->disparou_no_passo();}
    assert(disparou);
    Entidades::Personagens::Zumbi z({450,112},{0,0});z.perceber_terreno(&g);
    z.set_nochao(true);z.perceber({{1,600,112,true}});z.executar();
    assert(z.getPosicao().x==450); // Sonda alem da borda, antes de cair.
    for(bool grade:{false,true}) {g.ativar_grade(grade);assert(g.existe_solido({100,200,2,2}));assert(!g.existe_solido({510,200,2,2}));}
}
