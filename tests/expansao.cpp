#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/slots.h"
#include "../Logica/mundo.h"
#include "level.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <filesystem>
using Relogio=std::chrono::steady_clock;
using Persistencia::Json;
template<class Base> class CenaGrande:public Base {
public:
    CenaGrande(int id,bool carregar):Base(id,carregar) {}
    void passo() { auto m=this->mundo();Logica::simular(m); }
    sf::FloatRect bounds() const { return this->limites; }
    void separar() {
        if(this->num_jogadores==2) {
            auto it=this->jogadores.get_primeiro();++it;(*it)->setPosicao({100,820});
        }
    }
    void desenhar() { this->desenhar_partida(); }
};
template<class Base> void verificar(int id,const std::filesystem::path& pasta) {
    CenaGrande<Base> original(id,false),copia(id,true);
    auto dados=original.capturar();
    const auto mapa=Recursos::validar_mapa("Design/cenario/cenario_fase"+std::to_string(original.get_numero_fase())+".txt");
    const auto final=Testes::principais(mapa).back();
    unsigned n=0;
    for(auto& j:dados["jogadores"]) { j["posicao"]={final.x+300+n++*80,final.y-80};j["velocidade"]={0,0}; }
    dados["pontos"]=1234;dados["passos"]=36000;
    dados["inimigos"][0]["vivo"]=false;dados["inimigos"][0]["vida"]=0;
    for(auto& item:dados["obstaculos"]) if(item["tipo"]=="coracao") {item["vivo"]=false;break;}
    bool flecha=false;
    for(auto& e:dados["inimigos"]) if(e["tipo"]=="arqueiro" && e["posicao"][0].template get<float>()>30000) {
        Entidades::Projetil p({final.x+480,final.y-45},false);
        e["extra"]["projeteis"]=Json::array({Persistencia::Serializador::salvar(p)});flecha=true;break;
    }
    assert(flecha);
    original.restaurar(dados);
    auto& slots=Persistencia::Slots::instancia();slots.configurar_pasta(pasta);slots.selecionar(id%3+1);
    slots.salvar(original.capturar());copia.restaurar(slots.ler());
    assert(original.capturar()==copia.capturar() && copia.bounds().width==40000);
    const auto bytes=std::filesystem::file_size(slots.caminho_atual());assert(bytes<8*1024*1024);
    double custo=0;std::uint64_t candidatos=0,teoricos=0;
    for(int i=0;i<120;++i) {
        const auto inicio=Relogio::now();original.passo();
        custo+=std::chrono::duration<double,std::milli>(Relogio::now()-inicio).count();
        candidatos+=original.medidas_colisao().testes_obstaculos;
        teoricos+=original.medidas_colisao().pares_teoricos;
        copia.passo();
        if(i==0 || i==119) assert(original.capturar()==copia.capturar());
        if(i==0) {
            bool em_voo=false;
            const auto atual=copia.capturar();
            for(const auto& e:atual["inimigos"]) if(e["tipo"]=="arqueiro")
                for(const auto& p:e["extra"]["projeteis"])
                    if(p["posicao"][0].template get<float>()>38000) em_voo=true;
            assert(em_voo); // Uma constante antiga de 4000 nao pode apagar a flecha.
        }
    }
    auto depois=copia.capturar();
    assert(depois["passos"]==36120 && depois["jogadores"][0]["posicao"][0].template get<float>()>38000);
    assert(candidatos<teoricos/20);
    // A grade pode ser desligada para conferir equivalencia com a varredura completa.
    CenaGrande<Base> simples(id,true);simples.restaurar(dados);simples.usar_grade(false);
    copia.restaurar(dados);
    for(int i=0;i<6;++i) {simples.passo();copia.passo();}
    assert(simples.capturar()==copia.capturar());
    copia.separar();copia.ao_entrar();
    auto* janela=Gerenciadores::Gerenciador_Grafico::get_instancia()->get_Janela();janela->setFramerateLimit(0);
    janela->clear();copia.desenhar();janela->display(); // Aquecimento das texturas/atlas.
    const auto antes=copia.capturar();const auto inicio=Relogio::now();
    for(int i=0;i<30;++i) {janela->clear();copia.desenhar();janela->display();}
    const double desenho=std::chrono::duration<double,std::milli>(Relogio::now()-inicio).count()/30;
    assert(copia.capturar()==antes);
    sf::Texture pixels;assert(pixels.create(janela->getSize().x,janela->getSize().y));pixels.update(*janela);
    assert(pixels.copyToImage().saveToFile((pasta/("distante-"+std::to_string(id)+".png")).string()));
    std::cout<<"Estado "<<id<<": save "<<bytes<<" bytes, simulacao "<<custo/120<<" ms/passo, "
        <<candidatos<<"/"<<teoricos<<" candidatos, desenho "<<desenho<<" ms ("<<1000/desenho<<" FPS no ambiente de teste).\n";
}
int main() {
    const auto pasta=std::filesystem::temp_directory_path()/("zombies-expansao-"+std::to_string(Relogio::now().time_since_epoch().count()));
    std::filesystem::create_directories(pasta);std::filesystem::current_path(pasta);
    for(int id:{6,7})verificar<Estados::Fases::Fase1>(id,pasta);
    for(int id:{8,9})verificar<Estados::Fases::Fase2>(id,pasta);
    std::cout<<"Capturas e saves: "<<pasta<<'\n';
}
