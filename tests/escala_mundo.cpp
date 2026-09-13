#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Estados/Fases/construtor_cenario.h"
#include "../Recursos/escala.h"
#include "../Recursos/catalogo.h"
#include "../Recursos/limites.h"
#include "../Persistencia/entidades.h"
#include "level.h"
#include <cassert>
#include <iostream>
#include <queue>
#include <filesystem>
using namespace Entidades::Personagens;
using Testes::Patamar;
// Usa as mesmas entidades, movimento por passo e resolvedor de colisoes do jogo.
// Isola inimigos do percurso, mas exige saltos sem dano dos espinhos reais.
bool salto(Patamar a,Patamar b,Listas::ListaEntidade& terreno,bool dupla) {
    if(b.y<a.y-150 || b.x>a.fim+250 || a.x>b.fim+250) return false;
    Listas::ListaEntidade jogadores,inimigos;
    auto* j=new Jogador({0,0},{0,0},false); jogadores.incluir(j);
    Jogador* luigi=nullptr;
    if(dupla) { luigi=new Jogador({0,0},{0,0},true);jogadores.incluir(luigi); }
    Gerenciadores::Gerenciador_Colisoes g;
    g.set_jogadores(&jogadores);g.set_inimigos(&inimigos);g.set_obstaculos(&terreno);
    for(float inicio=a.x+10;inicio<=a.fim-50-(dupla?40:0);inicio+=25) {
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
    assert(bounds.width==40000);
    Testes::validar_passagens(linhas);
    // Uma regressao de clearance deve ser detectada mesmo em mapas pequenos.
    bool rejeitou=false;try { Testes::validar_passagens({"0"," ","0"}); } catch(...) { rejeitou=true; }
    assert(rejeitou);Testes::validar_passagens({"0"," "," ","0"});
    Listas::ListaEntidade inimigos;
    Estados::Fases::ConstrutorCenario::inimigos(caminho,inimigos);
    unsigned gigantes=0;
    for(auto it=inimigos.get_primeiro();it!=nullptr;++it) {
        const auto r=(*it)->get_corpo()->getGlobalBounds();
        assert(r.left>350 && r.left+r.width<=bounds.width);
        if(dynamic_cast<Gigante*>(*it)) { ++gigantes;assert(r.width==Recursos::Escala::gigante.x); }
        for(auto o=terreno.get_primeiro();o!=nullptr;++o)
            if((*o)->get_vivo()) assert(!r.intersects((*o)->get_corpo()->getGlobalBounds()));
    }
    assert(inimigos.get_tamanho()>=36 && gigantes==3 && terreno.get_tamanho()>=2440);
    const auto plataformas=Testes::principais(linhas);
    assert(plataformas.size()>=60 && plataformas.front().x==0 && plataformas.back().fim==40000);
    for(std::size_t i=1;i<plataformas.size();++i) {
        assert(std::abs(plataformas[i].y-plataformas[i-1].y)<=100);
        assert(plataformas[i].x-plataformas[i-1].fim<=100);
        if(!salto(plataformas[i-1],plataformas[i],terreno,dupla)) {
            std::cerr<<"Salto bloqueado fase "<<fase<<" dupla "<<dupla<<" x="<<plataformas[i].x<<"\n";assert(false);
        }
    }
    for(const auto& p:Testes::patamares(linhas)) if(p.fim-p.x<350) {
        // Sacadas opcionais e pequenos fundos dos vaos devem ter entrada/saida.
        bool acessivel=false;
        for(const auto& a:plataformas) {
            if(p.fim<a.x-250 || p.x>a.fim+250) continue;
            if(p.fim-p.x<=100) acessivel=salto(p,a,terreno,dupla);
            else acessivel=salto(a,p,terreno,dupla);
            if(acessivel)break;
        }
        if(!acessivel) { std::cerr<<"Desvio sem acesso/retorno x="<<p.x<<" y="<<p.y<<"\n";assert(false); }
    }

    // Atravessa de fato o primeiro corredor de 100 unidades, sem pular.
    const auto sacadas=Testes::patamares(linhas);
    const auto it_sacada=std::find_if(sacadas.begin(),sacadas.end(),[](auto p){return p.fim-p.x==150;});
    assert(it_sacada!=sacadas.end());const auto sacada=*it_sacada;
    const auto it_apoio=std::find_if(plataformas.begin(),plataformas.end(),[&](auto p){return p.x<=sacada.x && p.fim>=sacada.fim;});
    assert(it_apoio!=plataformas.end());const auto apoio=*it_apoio;
    Listas::ListaEntidade passantes,vazio;std::vector<Jogador*> andando;
    for(int n=0;n<(dupla?2:1);++n) {
        auto* j=new Jogador({sacada.x-60+n*40,apoio.y-Recursos::Escala::humano},{0,0},n!=0);
        passantes.incluir(j);andando.push_back(j);
    }
    Gerenciadores::Gerenciador_Colisoes corredor;
    corredor.set_jogadores(&passantes);corredor.set_obstaculos(&terreno);corredor.set_inimigos(&vazio);
    for(int t=0;t<2500 && andando[0]->getPosicao().x<sacada.fim+10;++t) {
        for(auto* j:andando) j->mover_com_controles(false,true,false,false);
        corredor.gerenciar_colisoes();
        for(auto* j:andando) assert(std::abs(j->getPosicao().y+j->getTamanho().y-apoio.y)<.05f && j->get_vida()==20);
    }
    assert(andando[0]->getPosicao().x>=sacada.fim+10);
    std::cout<<"Fase "<<fase<<" "<<(dupla?"dupla":"solo")<<": "<<plataformas.size()<<" patamares principais alcancaveis: subida <=100, deslocamento <=250; desvios <=150.\n";
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
    assert(passagem_minima>=std::max(j.getTamanho().y,l.getTamanho().y)*1.15f);
    assert(j.get_corpo()->getSize()==jogador && l.get_corpo()->getSize()==jogador);
    assert(z.getTamanho()==zumbi && a.getTamanho()==arqueiro && g.getTamanho()==gigante);
    Entidades::Obstaculos::Caixa c;Entidades::Obstaculos::Espinho e;Entidades::Obstaculos::Coracao h;
    assert(c.getTamanho().y<humano/2 && e.getTamanho().y<humano/2 && h.getTamanho().y==coracao && h.getTamanho().y<=humano/2);
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
