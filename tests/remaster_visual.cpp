#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Menus/menu_jogadores.h"
#include "../Estados/Menus/menu_fase.h"
#include "../Estados/Menus/pause.h"
#include "../Estados/Menus/ranking.h"
#include "../Estados/Menus/nome.h"
#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Gerenciadores/gerenciador_eventos.h"
#include "../Interface/preferencias.h"
#include "../Persistencia/ranking.h"
#include "../Persistencia/entidades.h"
#include "../Entidades/Obstaculos/neve.h"
#include "../Entidades/Obstaculos/musgo.h"
#include <cassert>
#include <chrono>
#include <iostream>
// Exercita telas reais em uma pasta temporaria, sem tocar nas partidas do usuario.
int main() {
    namespace fs=std::filesystem;
    const auto pasta=fs::temp_directory_path()/("zombies-remaster-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(pasta); fs::current_path(pasta);
    auto* g=Gerenciadores::Gerenciador_Grafico::get_instancia(); auto* j=g->get_Janela();
    // Cada recorte conserva a caixa fisica e e reproduzido ao carregar a partida.
    for(int x=-1;x<7;++x) {
        Entidades::Obstaculos::Neve neve({x*50.f,100});
        Entidades::Obstaculos::Musgo musgo({x*50.f,100});
        for(auto* bloco:{static_cast<Entidades::Entidade*>(&neve),static_cast<Entidades::Entidade*>(&musgo)}) {
            const auto dados=Persistencia::Serializador::salvar(*bloco);
            auto copia=Persistencia::Serializador::carregar(dados);
            assert(Persistencia::Serializador::salvar(*copia)==dados);
            assert(bloco->get_corpo()->getGlobalBounds()==copia->get_corpo()->getGlobalBounds());
            assert(bloco->get_corpo()->getTextureRect()==copia->get_corpo()->getTextureRect());
            assert(bloco->get_corpo()->getSize()==sf::Vector2f(50,50));
        }
    }
    for(const auto* fonte:{&Interface::fonte_titulo(),&Interface::fonte_interface()})
        for(sf::Uint32 c:{43u,48u,57u,0xe7u,0xe3u,0xe9u})
            assert(fonte->getGlyph(c,28,false).textureRect!=fonte->getGlyph(0,28,false).textureRect);
    auto* e=Gerenciadores::Gerenciador_Estados::get_instancia();
    e->adicionar_estado(new Estados::Menus::Menu_Principal(0)); e->adicionar_estado(new Estados::Menus::Menu_Jogadores(1));
    e->adicionar_estado(new Estados::Menus::Menu_Fase(2,false)); e->adicionar_estado(new Estados::Menus::Menu_Fase(3,true));
    e->adicionar_estado(new Estados::Menus::Ranking(4)); e->adicionar_estado(new Estados::Menus::Pause(5)); e->adicionar_estado(new Estados::Menus::Nome(10));
    auto tecla=[&](sf::Keyboard::Key k){sf::Event ev{};ev.type=sf::Event::KeyPressed;ev.key.code=k;Gerenciadores::Gerenciador_Eventos::get_instancia()->processar_evento(ev);assert(e->mensagem.find("Falha")==std::string::npos);};
    auto foto=[&](const std::string& nome){j->clear();e->executar();j->display();sf::Texture t;t.create(j->getSize().x,j->getSize().y);t.update(*j);assert(t.copyToImage().saveToFile((pasta/(nome+".png")).string()));};
    foto("01-principal"); tecla(sf::Keyboard::Enter); foto("02-novo-jogo"); tecla(sf::Keyboard::Enter); foto("03-jogadores");
    tecla(sf::Keyboard::Enter); foto("04-fases"); tecla(sf::Keyboard::Enter); assert(e->get_estado_atual()==6); foto("05-neve-solo");
    tecla(sf::Keyboard::Escape); foto("06-pausa"); tecla(sf::Keyboard::F); foto("07-preferencias"); tecla(sf::Keyboard::F);
    tecla(sf::Keyboard::Down); tecla(sf::Keyboard::Enter); tecla(sf::Keyboard::Down); tecla(sf::Keyboard::Enter); foto("08-continuar"); tecla(sf::Keyboard::Enter); assert(e->get_estado_atual()==6);
    for(int id:{7,8,9}) {
        e->set_estado_atual(0);
        Estados::Fases::Fase* fase=id<8?static_cast<Estados::Fases::Fase*>(new Estados::Fases::Fase1(id,false)):new Estados::Fases::Fase2(id,false);
        e->adicionar_estado(fase); e->set_estado_atual(id); foto("09-fase-"+std::to_string(id));
    }
    auto* fase=dynamic_cast<Estados::Fases::Fase*>(e->get_estado(9));
    auto salvo=fase->capturar();
    for(bool venceu:{false,true}) {
        e->set_estado_atual(0); auto resultado=salvo; resultado["finalizada"]=true;resultado["vitoria"]=venceu;
        fase->restaurar(resultado); e->set_fase(9); e->set_estado_atual(10); foto(venceu?"11-vitoria":"10-derrota");
    }
    for(int i=0;i<8;++i) Persistencia::RepositorioRanking().registrar({"visual-"+std::to_string(i),1,1,{"Sobrevivente "+std::to_string(i+1)},20.0+i});
    e->set_estado_atual(4);foto("12-ranking");
    Interface::Preferencias::instancia().contraste=true;Interface::Preferencias::instancia().escala=1.3f;
    e->set_estado_atual(0);foto("13-alto-contraste");
    Interface::Preferencias::instancia().contraste=false;
    j->setSize({1280,720});
    sf::Event evento{};
    while(j->pollEvent(evento)) Gerenciadores::Gerenciador_Eventos::get_instancia()->processar_evento(evento);
    g->resetarCamera();foto("14-panoramico");
    Interface::Preferencias::instancia().legivel=true;
    assert(&Interface::fonte_interface()==&Interface::fonte_legivel());
    assert(&Interface::fonte_titulo()==&Interface::fonte_legivel());
    foto("15-fonte-acessivel");
    std::cout<<"Fluxos visuais e recursos OK. Capturas: "<<pasta<<'\n';
    e->encerrar();Gerenciadores::Gerenciador_Eventos::get_instancia()->encerrar();g->encerrar();
}
