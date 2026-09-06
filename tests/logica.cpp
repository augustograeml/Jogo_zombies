#include "../Logica/entrada.h"
#include "../Audio/sintese.h"
#include <set>
#include "../Logica/movimento.h"
#include "../Logica/eventos.h"
#include "../Logica/comportamento.h"
#include "../Logica/sessao.h"
#include "../Estados/identificador.h"
#include <cassert>
#include <iostream>
int main() {
    Logica::Teclas teclas; teclas.esquerda=true; teclas.espaco=true;
    assert(Logica::controles(teclas,false,true).esquerda && Logica::controles(teclas,false,true).pular);
    assert(!Logica::controles(teclas,false,false).esquerda && !Logica::controles(teclas,false,false).pular);
    assert(Logica::controles(teclas,true,false).esquerda && !Logica::controles(teclas,true,false).pular);
    teclas.w=true; assert(Logica::controles(teclas,false,false).pular);
    std::set<std::uint64_t> sons;
    for(int i=0;i<8;++i) {
        const auto pcm=Audio::sintetizar(static_cast<Logica::Evento>(i));
        std::uint64_t assinatura=1469598103934665603ull; bool audivel=false;
        for(auto amostra:pcm) { assinatura=(assinatura^static_cast<std::uint16_t>(amostra))*1099511628211ull; audivel|=amostra!=0; }
        assert(audivel && pcm.front()==0); sons.insert(assinatura);
    }
    assert(sons.size()==8);
    Logica::Velocidade v{0,0};
    for(int i=0;i<100;++i) v=Logica::mover(v,true,false,true,false,false);
    assert(v.x==4);
    for(int i=0;i<30;++i) v=Logica::mover(v,true,false,false,false,false);
    assert(v.x==0 && v.y==0);
    assert(Logica::mover(v,true,false,false,true,false).y==-6);
    assert(Estados::apos_resultado(true,1,2)==Estados::Tela::Fase2Dupla);
    assert(Estados::apos_resultado(false,2,2)==Estados::Tela::Principal);
    assert(Estados::apos_resultado(true,2,1)==Estados::Tela::Ranking);
    Logica::Eventos eventos;
    Logica::Pontuacao pontos;
    struct Contador:Logica::Observador { int n=0; void receber(const Logica::Notificacao&) override { ++n; } } contador;
    eventos.assinar(pontos); eventos.assinar(contador); eventos.assinar(contador);
    eventos.publicar({Logica::Evento::Dano}); assert(pontos.pontos()==0 && contador.n==1);
    eventos.publicar({Logica::Evento::InimigoDerrotado,100}); assert(pontos.pontos()==100 && contador.n==2);
    eventos.remover(contador); eventos.publicar({Logica::Evento::Coleta,25}); assert(pontos.pontos()==125 && contador.n==2);
    pontos.restaurar(40); assert(pontos.pontos()==40);
    Logica::EstadoArqueiro arq;
    assert(!Logica::decidir_arqueiro(arq,0,0,{{1,900,0,true},{2,10,0,false}},false) && arq.alvo==-1);
    for(int i=0;i<29;++i) assert(!Logica::decidir_arqueiro(arq,0,0,{{1,300,0,true},{2,100,0,true}},false));
    assert(arq.alvo==2 && arq.preparacao==29);
    auto retomado=arq;
    assert(Logica::decidir_arqueiro(arq,0,0,{{1,300,0,true},{2,100,0,true}},false));
    assert(Logica::decidir_arqueiro(retomado,0,0,{{1,300,0,true},{2,100,0,true}},false));
    assert(!Logica::decidir_arqueiro(arq,0,0,{{1,300,0,true}},true) && arq.acao==Logica::AcaoArqueiro::Recarga);
    assert(!Logica::decidir_arqueiro(arq,0,0,{{1,-100,0,true}},false) && !arq.direita);
    assert(!Logica::decidir_arqueiro(arq,0,0,{{1,-100,0,false}},false) && arq.alvo==-1 && arq.preparacao==0);
    Logica::Sessao sessao; assert(sessao.tempo_total()==0);
    sessao.acumular(5); int n=0; while(sessao.proximo()) { ++n; ++sessao.passos; } assert(n==15);
    auto copia=sessao; assert(copia.tempo()==sessao.tempo());
    assert(Estados::eh_fase(Estados::codigo(Estados::Tela::Fase2Dupla)) && !Estados::eh_fase(5));
    std::cout << "Logica: Observer, pontuacao, alvo, antecipacao, recarga, sessao e estados OK\n";
}
