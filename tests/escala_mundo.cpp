#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Estados/Fases/construtor_cenario.h"
#include "../Recursos/escala.h"
#include "../Recursos/catalogo.h"
#include "../Recursos/limites.h"
#include "../Persistencia/entidades.h"
#include <cassert>
#include <iostream>
#include <queue>
#include <filesystem>
using namespace Entidades::Personagens;
struct Patamar { float x,fim,y; };
// Usa as mesmas entidades, movimento por passo e resolvedor de colisoes do jogo.
// Isola inimigos do percurso, mas exige saltos sem dano dos espinhos reais.
bool salto(Patamar a,Patamar b,Listas::ListaEntidade& terreno,bool dupla) {
    if(b.y<a.y-100 || b.x>a.fim+250 || a.x>b.fim+250) return false;
    Listas::ListaEntidade jogadores,inimigos;
    auto* j=new Jogador({0,0},{0,0},false); jogadores.incluir(j);
    Jogador* luigi=nullptr;
    if(dupla) { luigi=new Jogador({0,0},{0,0},true);jogadores.incluir(luigi); }
    Gerenciadores::Gerenciador_Colisoes g;
    g.set_jogadores(&jogadores);g.set_inimigos(&inimigos);g.set_obstaculos(&terreno);
    for(float inicio=a.x+25;inicio<=a.fim-45-(dupla?40:0);inicio+=25) {
        const float destino=std::clamp(inicio,b.x+10,b.fim-50-(dupla?40:0));
        if(std::abs(destino-inicio)>250) continue;
        for(auto* jogador:{j,luigi}) if(jogador) {
            jogador->setPosicao({inicio+(jogador==luigi?40.f:0.f),a.y-Recursos::Escala::humano});
            jogador->setVelocidade({.2f,0});jogador->set_nochao(true);
            jogador->set_vida(20);jogador->set_vivo(true);jogador->set_protecao(0);
            jogador->restaurar_movimento({a.y-Recursos::Escala::humano,false,false});
        }
        for(int passo=0;passo<180;++passo) {
            for(auto* jogador:{j,luigi}) if(jogador) {
                const float dx=destino+(jogador==luigi?40.f:0.f)-jogador->getPosicao().x;
                const float vx=jogador->getVelocidade().x;
                // Freia antecipadamente no ar; margem interior deixa espaco para dupla.
                const float parada=vx*std::abs(vx)/.4f;
                const float erro=dx-parada;
                jogador->mover_com_controles(erro<-3,erro>3,passo==0,false);
            }
            g.gerenciar_colisoes();
            const auto r=j->get_corpo()->getGlobalBounds();
            if(passo>10 && j->get_vida()==20 && (!luigi || luigi->get_vida()==20) && std::abs(r.top+r.height-b.y)<.05f && r.left>=b.x+5 && r.left+r.width<=b.fim-5) {
                if(luigi) {
                    const auto outro=luigi->get_corpo()->getGlobalBounds();
                    if(std::abs(outro.top+outro.height-b.y)>.05f || outro.left<b.x+5 || outro.left+outro.width>b.fim-5) continue;
                }
                return true;
            }
        }
    }
    return false;
}
void mapa(int fase,bool dupla) {
    const auto caminho="Design/cenario/cenario_fase"+std::to_string(fase)+".txt";
    const auto linhas=Recursos::validar_mapa(caminho);
    Listas::ListaEntidade terreno;
    Estados::Fases::ConstrutorCenario::obstaculos(caminho,terreno);
    // Desativa apenas curas para que uma coleta nao mascare dano durante um salto.
    for(auto it=terreno.get_primeiro();it!=nullptr;++it)
        if(dynamic_cast<Entidades::Obstaculos::Coracao*>(*it)) (*it)->morrer();
    const auto bounds=Recursos::limites_mundo(terreno);
    assert(bounds.width==4000 && bounds.height==1100);
    Listas::ListaEntidade inimigos;
    Estados::Fases::ConstrutorCenario::inimigos(caminho,inimigos);
    unsigned gigantes=0;
    for(auto it=inimigos.get_primeiro();it!=nullptr;++it) {
        const auto r=(*it)->get_corpo()->getGlobalBounds();
        assert(r.left>350); // Nenhum inimigo nasce sobre os dois pontos iniciais.
        if(dynamic_cast<Gigante*>(*it)) { ++gigantes;assert(r.left>3200 && r.width==Recursos::Escala::gigante.x); }
        for(auto o=terreno.get_primeiro();o!=nullptr;++o)
            if((*o)->get_vivo()) assert(!r.intersects((*o)->get_corpo()->getGlobalBounds()));
    }
    assert(gigantes==1);
    const char material=fase==1?'0':'7';std::vector<Patamar> plataformas;
    for(std::size_t y=0;y<linhas.size();++y) {
        int inicio=-1;
        for(std::size_t x=0;x<=linhas[y].size();++x) {
            const bool superficie=x<linhas[y].size() && linhas[y][x]==material &&
                (y==0 || x>=linhas[y-1].size() || linhas[y-1][x]!=material) &&
                (y<2 || x>=linhas[y-2].size() || linhas[y-2][x]!=material);
            if(superficie && inicio<0) inicio=x;
            if(!superficie && inicio>=0) { plataformas.push_back({inicio*50.f,x*50.f,y*50.f});inicio=-1; }
        }
    }
    std::vector<bool> visitado(plataformas.size());std::queue<std::size_t> fila;
    for(std::size_t i=0;i<plataformas.size();++i)
        if(plataformas[i].x==0 && plataformas[i].y==900) {visitado[i]=true;fila.push(i);}
    while(!fila.empty()) {
        const auto atual=fila.front();fila.pop();
        for(std::size_t i=0;i<plataformas.size();++i) if(!visitado[i] && salto(plataformas[atual],plataformas[i],terreno,dupla)) {
            visitado[i]=true;fila.push(i);
        }
    }
    for(std::size_t i=0;i<plataformas.size();++i) if(!visitado[i] && plataformas[i].y<1000) {
        std::cerr<<"Inalcancavel fase "<<fase<<": "<<plataformas[i].x<<","<<plataformas[i].y<<"\n";assert(false);
    }
    std::cout<<"Fase "<<fase<<" "<<(dupla?"dupla":"solo")<<": "<<std::count(visitado.begin(),visitado.end(),true)<<" patamares alcancaveis com subida <=100 e deslocamento <=250.\n";
}
int main() {
    using namespace Recursos::Escala;
    // Oraculo de alcance baseado na regra real por passo, com saida lenta do musgo.
    for(float inicial:{.2f,4.f}) {
        Logica::Velocidade v{inicial,0};float x=0,y=0,apice=0;int passos=0;
        do { v=Logica::mover(v,passos==0,false,true,passos==0,false);
            x+=v.x;y+=v.y;apice=std::min(apice,y);++passos;
        } while(v.y<=0 || y<-100);
        assert(-apice>100/.65f && x>250/.8f);
        std::cout<<"Salto: altura "<<-apice<<", alcance para subir 100: "<<x<<"\n";
    }
    Jogador j({0,0},{0,0},false),l({0,0},{0,0},true);Zumbi z;Arqueiro a;Gigante g;
    assert(j.get_corpo()->getSize()==jogador && l.get_corpo()->getSize()==jogador);
    assert(z.getTamanho()==zumbi && a.getTamanho()==arqueiro && g.getTamanho()==gigante);
    Entidades::Obstaculos::Caixa c;Entidades::Obstaculos::Espinho e;Entidades::Obstaculos::Coracao h;
    assert(c.getTamanho().y<humano/2 && e.getTamanho().y<humano/2 && h.getTamanho().y<humano/3);
    for(auto* entidade:std::vector<Entidades::Entidade*>{&j,&l,&z,&a,&g,&c,&e,&h}) {
        auto salvo=Persistencia::Serializador::salvar(*entidade);
        auto copia=Persistencia::Serializador::carregar(salvo);
        assert(Persistencia::Serializador::salvar(*copia)==salvo);
    }
    {
        Listas::ListaEntidade jogadores,obstaculos,inimigos;
        auto* jogador=new Jogador({100,100},{0,0},false);jogadores.incluir(jogador);
        auto* cura=new Entidades::Obstaculos::Coracao({110,120});obstaculos.incluir(cura);
        Gerenciadores::Gerenciador_Colisoes colisao;
        colisao.set_jogadores(&jogadores);colisao.set_inimigos(&inimigos);colisao.set_obstaculos(&obstaculos);
        jogador->receber_dano(10);const auto pos=jogador->getPosicao();
        colisao.gerenciar_colisoes();assert(!cura->get_vivo() && jogador->get_vida()>10 && jogador->getPosicao()==pos);
        obstaculos.limpar();colisao.invalidar_grade();
        for(int tipo=0;tipo<3;++tipo) {
            Inimigo* inimigo=tipo==0?static_cast<Inimigo*>(new Zumbi({100,300},{0,0})):
                tipo==1?static_cast<Inimigo*>(new Arqueiro({100,300},{0,0})):static_cast<Inimigo*>(new Gigante({100,300},{0,0}));
            inimigos.incluir(inimigo);const int vida=inimigo->get_vida();
            jogador->setPosicao({105,222});jogador->setVelocidade({0,3});
            jogador->restaurar_movimento({222,true,false});colisao.gerenciar_colisoes();
            assert(inimigo->get_vida()==std::max(0,vida-(tipo==2?10:20)) && jogador->getVelocidade().y==-3);
            inimigos.limpar();
        }
    }
    for(int fase:{1,2}) for(bool dupla:{false,true}) mapa(fase,dupla);
}
