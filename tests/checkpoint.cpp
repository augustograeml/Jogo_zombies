#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Persistencia/slots.h"
#include <cassert>
#include <chrono>
#include <fstream>
template<class Base> class Cena:public Base {
public: Cena(int id):Base(id,false){}
    void marcar() {this->verificar_checkpoint();}
};
template<class Base> void testar(int modo) {
        Cena<Base> a(modo);auto dados=a.capturar();
        auto perigo=dados;perigo["inimigos"][0]["posicao"]=Persistencia::Json::array({150,850});
        a.restaurar(perigo);a.marcar();assert(!a.capturar().contains("checkpoint"));
        for(auto& e:dados["inimigos"])e["posicao"][0]=30000;
        a.restaurar(dados);a.marcar();auto salvo=a.capturar();assert(salvo.contains("checkpoint"));
        assert(Persistencia::Slots::instancia().ler()==salvo);
        auto derrota=salvo;derrota["passos"]=600;derrota["finalizada"]=true;
        derrota["pontos"]=900;
        for(auto& e:derrota["jogadores"]){e["vivo"]=false;e["vida"]=0;}
        a.restaurar(derrota);assert(a.pode_recuperar_checkpoint());a.recuperar_checkpoint();
        assert(!a.get_finalizada() && a.get_assistida() && a.get_pontos()==0 && a.get_passos_sessao()==600);
        auto retomada=a.capturar();Cena<Base> b(modo);b.restaurar(retomada);assert(b.capturar()==retomada);
        auto ruim=retomada;ruim["checkpoint"]["jogadores"][0]["posicao"][0]="erro";
        bool rejeitou=false;try{b.restaurar(ruim);}catch(...){rejeitou=true;}
        assert(rejeitou && b.capturar()==retomada);
        a.restaurar(derrota);const auto antes_falha=a.capturar();
        std::ofstream("destino-invalido")<<"arquivo, nao pasta";
        auto& slots=Persistencia::Slots::instancia();slots.configurar_pasta("destino-invalido");
        rejeitou=false;try{a.recuperar_checkpoint();}catch(...){rejeitou=true;}
        slots.configurar_pasta(std::filesystem::current_path());
        assert(rejeitou && a.capturar()==antes_falha);
        auto fim=retomada;fim["finalizada"]=true;fim["vitoria"]=true;fim["pontos"]=200;
        b.restaurar(fim);assert(!b.registrar_resultado((modo==6 || modo==8)?std::vector<std::string>{"Ana"}:std::vector<std::string>{"Ana","Bia"}));
        assert(!std::filesystem::exists("ranking.json") && !std::filesystem::exists("ranking-pontos.json"));
    }
int main() {
    auto pasta=std::filesystem::temp_directory_path()/("zombies-checkpoint-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(pasta);std::filesystem::current_path(pasta);
    Persistencia::Slots::instancia().configurar_pasta(pasta);
    for(int modo:{6,7}) testar<Estados::Fases::Fase1>(modo);
    for(int modo:{8,9}) testar<Estados::Fases::Fase2>(modo);
}
