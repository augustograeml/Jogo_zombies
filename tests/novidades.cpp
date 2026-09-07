#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Menus/nome.h"
#include "../Estados/Menus/pause.h"
#include "../Estados/Menus/ranking.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/slots.h"
#include "../Persistencia/pontos.h"
#include "../Recursos/catalogo.h"
#include "../Audio/efeitos.h"
#include <cassert>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
namespace fs=std::filesystem;
using Persistencia::Json;
using Persistencia::Serializador;
using Estados::Fases::Fase1;
class FaseComEventos : public Fase1 {
public:
    FaseComEventos() : Fase1(6,false) {}
    void observar(Logica::Observador& o) { eventos.assinar(o); }
};
void testar_eventos_combate() {
    struct Contagem : Logica::Observador {
        std::array<int,8> eventos{};
        void receber(const Logica::Notificacao& e) override { ++eventos.at(static_cast<unsigned>(e.tipo)); }
    } contagem;
    FaseComEventos fase; fase.observar(contagem);
    auto base=fase.capturar(); base["obstaculos"]=Json::array();
    Entidades::Personagens::Gigante gigante({100,150},{0,0});
    base["inimigos"]=Json::array({Serializador::salvar(gigante)});
    auto& jogador=base["jogadores"][0]; jogador["posicao"]={100,100}; jogador["velocidade"]={0,3};
    jogador["nochao"]=false; jogador["extra"]["movimento"]={{"origem_queda",100},{"queda_ativa",true},{"gelo",false}};
    fase.restaurar(base); fase.simular_passo();
    assert(contagem.eventos[static_cast<unsigned>(Logica::Evento::ImpactoInimigo)]==1);
    auto fatal=base; fatal["inimigos"][0]["vida"]=10;
    fase.restaurar(fatal); fase.simular_passo();
    assert(contagem.eventos[static_cast<unsigned>(Logica::Evento::ImpactoInimigo)]==2);
    assert(contagem.eventos[static_cast<unsigned>(Logica::Evento::InimigoDerrotado)]==1);
    assert(contagem.eventos[static_cast<unsigned>(Logica::Evento::Vitoria)]==1);
    const auto contados=contagem.eventos; fase.restaurar(fase.capturar()); fase.simular_passo(); assert(contagem.eventos==contados);
    base["jogadores"][0]["vida"]=0; fase.restaurar(base); fase.simular_passo();
    assert(contagem.eventos[static_cast<unsigned>(Logica::Evento::Derrota)]==1);
    Gerenciadores::Gerenciador_Estados::get_instancia()->set_estado_atual(0);
}
sf::Event tecla(sf::Keyboard::Key t) { sf::Event e{}; e.type=sf::Event::KeyPressed; e.key.code=t; return e; }
int main() {
    try {
        const auto pasta=fs::temp_directory_path()/("zombies-novidades-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(pasta); fs::current_path(pasta); // Recursos devem funcionar fora da pasta do jogo.
        auto* ge=Gerenciadores::Gerenciador_Estados::get_instancia();
        ge->adicionar_estado(new Estados::Menus::Menu_Principal(0));
        ge->adicionar_estado(new Estados::Menus::Nome(10));
        ge->adicionar_estado(new Estados::Menus::Pause(5));
        ge->adicionar_estado(new Estados::Menus::Ranking(4));
        testar_eventos_combate();
        Entidades::Personagens::Jogador jogador({80,50},{0,0},false);
        const auto tamanho=jogador.getTamanho();
        for(int i=0;i<9;++i) { jogador.set_nochao(true); jogador.mover_com_controles(false,true,false,false); }
        assert(jogador.get_animacao().correndo && jogador.get_animacao().quadro>0);
        assert(jogador.receber_dano(3) && !jogador.receber_dano(3) && jogador.get_vida()==17);
        jogador.restaurar_movimento({-300,true,true});
        const auto salvo=Serializador::salvar(jogador); auto copia=Serializador::carregar(salvo);
        assert(Serializador::salvar(*copia)==salvo && copia->get_protecao()==45);
        for(int i=0;i<45;++i) copia->atualizar_protecao();
        assert(copia->receber_dano(2)); copia->curar(100); assert(copia->get_vida()==20);
        jogador.setVelocidade({-1,0}); jogador.set_nochao(true); jogador.mover_com_controles(true,false,false,false);
        assert(!jogador.get_animacao().direita && jogador.getTamanho()==tamanho);
        auto antigo=salvo; antigo.erase("protecao"); antigo["extra"].erase("animacao"); antigo["extra"].erase("movimento");
        auto legado=Serializador::carregar(antigo); assert(legado->get_protecao()==0);
        auto* jlegado=static_cast<Entidades::Personagens::Jogador*>(legado.get()); assert(!jlegado->get_animacao().correndo && !jlegado->get_movimento().queda_ativa);
        Entidades::Personagens::Arqueiro arqueiro({400,100},{0,0});
        const std::vector<Logica::Alvo> alvos{{1,100,100,true},{2,300,100,false}};
        for(int i=0;i<15;++i) { arqueiro.perceber(alvos); arqueiro.set_nochao(true); arqueiro.executar(); }
        assert(arqueiro.get_comportamento().preparacao==15 && arqueiro.get_projeteis()->empty());
        auto arq_copia=Serializador::carregar(Serializador::salvar(arqueiro));
        auto* retomado=static_cast<Entidades::Personagens::Arqueiro*>(arq_copia.get());
        for(int i=0;i<15;++i) {
            arqueiro.perceber(alvos); retomado->perceber(alvos); arqueiro.set_nochao(true); retomado->set_nochao(true);
            arqueiro.executar(); retomado->executar();
            assert(Serializador::salvar(arqueiro)==Serializador::salvar(*retomado));
        }
        assert(arqueiro.get_projeteis()->size()==1 && arqueiro.get_projeteis()->front().getVelocidade().x<0);
        Fase1 original(6,false), otimizada(6,true); auto estado=original.capturar();
        // Comparacao sobre o mesmo mapa e RNG, mudando somente a selecao de candidatos.
        original.usar_grade(false); otimizada.restaurar(estado);
        double custo_a=0,custo_b=0,sim=0; std::uint64_t pares=0,locais=0;
        for(int i=0;i<180;++i) {
            original.simular_passo(); otimizada.simular_passo();
            assert(original.capturar()==otimizada.capturar());
            custo_a+=original.medidas_colisao().microssegundos; custo_b+=otimizada.medidas_colisao().microssegundos;
            sim+=otimizada.custo_simulacao(); pares+=original.medidas_colisao().testes_obstaculos;
            locais+=otimizada.medidas_colisao().testes_obstaculos;
        }
        assert(locais<pares);
        std::cout<<"Mapa fase1, 180 passos: "<<pares<<" -> "<<locais<<" candidatos; colisao media "<<custo_a/180<<" -> "<<custo_b/180<<" us; simulacao "<<sim/180<<" us\n";
        auto& slots=Persistencia::Slots::instancia();
        slots.definir_validador([](const Json& j){ Fase1 validacao(6,true); validacao.restaurar(j); });
        slots.salvar(estado); auto novo=estado; novo["pontos"]=125; slots.salvar(novo);
        auto ruim=novo; ruim["jogadores"][0]["extra"]["animacao"]["quadro"]=99;
        Persistencia::escrever_json("partida.json",ruim);
        assert(!slots.listar()[0].valido && slots.listar()[0].backup_disponivel);
        assert(slots.recuperar()==estado && fs::exists("partida.json.corrompido-1"));
        slots.selecionar(2); slots.salvar(novo); slots.selecionar(1); assert(slots.ler()==estado);
        slots.definir_validador({});
        // Inimigo e coracao ja consumidos nao concedem novos pontos ao carregar.
        auto versao1=estado; versao1["versao"]=1; versao1.erase("pontos");
        for(auto& j:versao1["jogadores"]) { j.erase("protecao"); j["extra"].erase("animacao"); }
        Fase1 migrada(6,true); migrada.restaurar(versao1);
        assert(migrada.capturar()["versao"]==2 && migrada.get_pontos()==0);
        auto rodada=estado; rodada["pontos"]=125;
        rodada["obstaculos"]=Json::array();
        for(auto& e:rodada["inimigos"]) { e["vivo"]=false; if(e["tipo"]=="arqueiro") e["extra"]["projeteis"]=Json::array(); }
        auto fase=std::make_unique<Fase1>(6,true); fase->restaurar(rodada);
        ge->adicionar_estado(fase.get()); fase.release(); ge->set_estado_atual(6);
        auto* ativa=static_cast<Fase1*>(ge->get_estado(6)); ativa->simular_passo();
        assert(ativa->get_pontos()==625 && ativa->get_vitoria());
        ativa->registrar_resultado({"Teste"}); ativa->registrar_resultado({"Teste"});
        auto resultados=Persistencia::RepositorioPontos().consultar(1,1); assert(resultados.size()==1 && resultados[0].pontos==625);
        ge->set_estado_atual(0);
        auto derrota=std::make_unique<Fase1>(6,true); rodada["pontos"]=25; rodada["partida_id"]="derrota-com-pontos";
        for(auto& j:rodada["jogadores"]) j["vida"]=0;
        derrota->restaurar(rodada); ge->adicionar_estado(derrota.get()); derrota.release(); ge->set_estado_atual(6);
        ativa=static_cast<Fase1*>(ge->get_estado(6)); ativa->simular_passo(); assert(!ativa->get_vitoria());
        ativa->registrar_resultado({"Derrota"}); assert(Persistencia::RepositorioPontos().consultar(1,1).size()==2);
        ge->set_estado_atual(0); ge->set_fase(-1);
        auto* menu=static_cast<Estados::Menus::Menu_Principal*>(ge->get_estado(0));
        assert(!menu->saves_abertos());
        menu->tratar_evento(tecla(sf::Keyboard::Num2)); assert(slots.selecionado()==1);
        menu->tratar_evento(tecla(sf::Keyboard::Down)); menu->tratar_evento(tecla(sf::Keyboard::Enter));
        assert(menu->saves_abertos());
        menu->tratar_evento(tecla(sf::Keyboard::Num2)); assert(slots.selecionado()==2);
        menu->fase_salva(); assert(ge->get_estado_atual()==6 && static_cast<Fase1*>(ge->get_estado(6))->get_pontos()==125);
        auto* gg=Gerenciadores::Gerenciador_Grafico::get_instancia(); gg->limpar(); ge->executar(); gg->mostrar();
        sf::Texture imagem; imagem.create(1024,1024); imagem.update(*gg->get_Janela()); imagem.copyToImage().saveToFile("painel-novo.png");
        std::cout<<"Desenho de referencia: "<<static_cast<Fase1*>(ge->get_estado(6))->custo_desenho()<<" us\n";
        ge->set_estado_atual(5); Audio::configurar(-10,false); Audio::configurar(0,true);
        assert(Audio::Preferencias::instancia().mudo && Persistencia::ler_json("preferencias.json")["volume"]==50);
        Audio::habilitar(); Audio::publicar({Logica::Evento::Salto}); Audio::configurar(0,true);
        Audio::publicar({Logica::Evento::Vitoria}); Audio::interromper();
        std::cout<<"Novidades: animacao, dano, grade, slots, recovery, pontos, HUD e audio OK. Artefatos: "<<pasta<<'\n';
        ge->encerrar(); gg->encerrar();
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
}
