#include "../Estados/Fases/fase1.h"
#include "../Persistencia/entidades.h"
#include "../Recursos/catalogo.h"
#include <cassert>
#include <chrono>
#include <iostream>
using namespace Entidades::Personagens;
class FaseControles:public Estados::Fases::Fase1 {
public:
    FaseControles(int id,bool carregar):Fase1(id,carregar) {}
    bool controle_solo() { return static_cast<Jogador*>(*jogadores.get_primeiro())->get_controles_solo(); }
};
int main() {
    auto* gg=Gerenciadores::Gerenciador_Grafico::get_instancia();
    auto* janela=gg->get_Janela(); gg->resetarCamera(); janela->clear(sf::Color(26,35,49));
    sf::Font fonte; fonte.loadFromFile(Recursos::caminho("Design/fonte/fonte_simas.ttf").string());
    const char* nomes[]={"Luigi", "Zumbi", "Arqueiro", "Gigante"};
    for(int linha=0;linha<4;++linha) {
        sf::Text titulo(nomes[linha],fonte,24); titulo.setPosition(20,linha*240+10); janela->draw(titulo);
        for(int coluna=0;coluna<5;++coluna) {
            std::unique_ptr<Entidades::Entidade> e;
            if(linha==0) e=std::make_unique<Jogador>(sf::Vector2f(0,0),sf::Vector2f(0,0),true);
            else if(linha==1) e=std::make_unique<Zumbi>(sf::Vector2f(0,0),sf::Vector2f(0,0));
            else if(linha==2) e=std::make_unique<Arqueiro>();
            else e=std::make_unique<Gigante>(sf::Vector2f(0,0),sf::Vector2f(0,0));
            e->get_corpo()->setSize({150,150}); e->setPosicao({20.f+200*coluna,60.f+240*linha});
            Animacao::Estado a{true,static_cast<unsigned>(coluna*5),1,true};
            if(linha==0) static_cast<Jogador*>(e.get())->restaurar_animacao(a);
            else static_cast<Inimigo*>(e.get())->restaurar_animacao(a);
            const auto salvo=Persistencia::Serializador::salvar(*e);
            e->desenhar(); assert(Persistencia::Serializador::salvar(*e)==salvo);
            const auto restaurado=Persistencia::Serializador::carregar(salvo);
            assert(Persistencia::Serializador::salvar(*restaurado)==salvo);
            const auto posicao=e->getPosicao();
            if(linha>0) {
                auto* inimigo=static_cast<Inimigo*>(e.get()); inimigo->setVelocidade({-1,0}); inimigo->avancar_animacao();
                assert(!inimigo->get_animacao().direita && e->getPosicao()==posicao);
                inimigo->setVelocidade({0,0}); inimigo->avancar_animacao(); assert(!inimigo->get_animacao().correndo);
            }
        }
    }
    janela->display(); sf::Texture captura; captura.create(1024,1024); captura.update(*janela);
    const auto imagem=captura.copyToImage();
    for(int linha=0;linha<4;++linha) {
        bool diferente=false;
        for(unsigned y=60+240*linha;y<210+240*linha;++y) for(unsigned x=20;x<170;++x)
            diferente|=imagem.getPixel(x,y)!=imagem.getPixel(x+200,y);
        assert(diferente); // Cada personagem muda a pose, nao apenas sua posicao.
    }
    const auto arquivo=std::filesystem::temp_directory_path()/("zombies-caminhada-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+".png");
    imagem.saveToFile(arquivo.string());
    FaseControles solo(6,false),dupla(7,false),reaberto(6,true);
    assert(solo.controle_solo() && !dupla.controle_solo());
    reaberto.restaurar(solo.capturar()); assert(reaberto.controle_solo());
    std::cout<<"Caminhada: poses distintas, direcao, parada, desenho sem mutacao, snapshots e controles por modo OK. Imagem: "<<arquivo<<'\n';
    gg->encerrar();
}
