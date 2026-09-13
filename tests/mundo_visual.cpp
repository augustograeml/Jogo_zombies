#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Persistencia/entidades.h"
#include "../Recursos/catalogo.h"
#include "level.h"
#include <cassert>
#include <filesystem>
#include <iostream>
using namespace Entidades::Personagens;
using namespace Entidades::Obstaculos;
namespace fs=std::filesystem;
template<class Base> class Cena:public Base {
public:
    Cena(int id):Base(id,false) { this->ao_entrar(); }
    void foto(const fs::path& arquivo) {
        auto* janela=this->pGG->get_Janela();const auto antes=this->capturar();
        janela->clear();this->desenhar_partida();janela->display();
        assert(this->capturar()==antes);
        sf::Texture t;assert(t.create(janela->getSize().x,janela->getSize().y));t.update(*janela);
        assert(t.copyToImage().saveToFile(arquivo.string()));
    }
    void camera(sf::Vector2f a,sf::Vector2f b) {
        this->camera_dupla=Interface::CameraDupla{};
        unsigned n=0;for(auto it=this->jogadores.get_primeiro();it!=nullptr;++it)
            (*it)->setPosicao(n++?b:a);
    }
    void escala() {
        this->jogadores.limpar();this->inimigos.limpar();this->obstaculos.limpar();
        auto colocar=[](Entidades::Entidade* e,float x) {
            e->setPosicao({x,850-e->getTamanho().y});return e;
        };
        this->jogadores.incluir(colocar(new Jogador({0,0},{0,0},true),250));
        this->jogadores.incluir(colocar(new Jogador({0,0},{0,0},false),130));
        this->inimigos.incluir(colocar(new Zumbi,380));
        this->inimigos.incluir(colocar(new Arqueiro,500));
        this->inimigos.incluir(colocar(new Gigante,630));
        this->obstaculos.incluir(colocar(new Caixa,780));
        this->obstaculos.incluir(colocar(new Espinho,850));
        this->obstaculos.incluir(colocar(new Coracao,920));
        for(int x=0;x<21;++x) {
            if(this->get_numero_fase()==1)this->obstaculos.incluir(new Neve({x*50.f,850}));
            else this->obstaculos.incluir(new Musgo({x*50.f,850}));
        }
        this->limites={0,0,1024,1000};
    }
};
template<class Base> void testar(int fase) {
    const fs::path pasta="/tmp/zombies-mundo-visual";
    for(bool dupla:{false,true}) {
        Cena<Base> cena((fase==1?6:8)+(dupla?1:0));
        const auto inicio=cena.capturar();
        const std::string prefixo="fase"+std::to_string(fase)+(dupla?"-dupla-":"-solo-");
        cena.foto(pasta/(prefixo+"inicio.png"));
        const auto trechos=Testes::principais(Recursos::validar_mapa("Design/cenario/cenario_fase"+std::to_string(fase)+".txt"));
        const auto meio=trechos[trechos.size()/2];
        cena.camera({meio.x+60,meio.y-80},{meio.x+180,meio.y-80});cena.foto(pasta/(prefixo+"meio.png"));
        const auto arena=trechos.back();
        cena.camera({arena.x+300,arena.y-80},{arena.x+180,arena.y-80});cena.foto(pasta/(prefixo+"arena.png"));
        cena.camera({100,-450},{39500,-450});cena.foto(pasta/(prefixo+"alto-horizontal.png"));
        cena.camera({1800,-500},{1800,920});cena.foto(pasta/(prefixo+"alto-vertical.png"));
        for(auto tamanho:{sf::Vector2u(1280,720),sf::Vector2u(720,1280)}) {
            auto* janela=Gerenciadores::Gerenciador_Grafico::get_instancia()->get_Janela();
            janela->setSize(tamanho);cena.foto(pasta/(prefixo+std::to_string(tamanho.x)+".png"));
        }
        Gerenciadores::Gerenciador_Grafico::get_instancia()->get_Janela()->setSize({1024,1024});
        cena.restaurar(inicio);assert(cena.capturar()==inicio);
    }
    Cena<Base> escala(fase==1?7:9);escala.escala();escala.foto(pasta/("escala-fase"+std::to_string(fase)+".png"));
}
int main() {
    fs::create_directories("/tmp/zombies-mundo-visual");
    // Isola quaisquer preferencias/arquivos de teste dos saves do usuario.
    fs::current_path("/tmp/zombies-mundo-visual");
    testar<Estados::Fases::Fase1>(1);testar<Estados::Fases::Fase2>(2);
    std::cout<<"Capturas reais SFML: /tmp/zombies-mundo-visual\n";
}
