#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Persistencia/slots.h"
#include <cassert>
#include <chrono>
class Cena:public Estados::Fases::Fase1 {
public: Cena(int id):Fase1(id,false){}
    void marcar() {verificar_checkpoint();}
};
int main() {
    auto pasta=std::filesystem::temp_directory_path()/("zombies-checkpoint-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(pasta);std::filesystem::current_path(pasta);
    Persistencia::Slots::instancia().configurar_pasta(pasta);
    for(int modo:{6,7}) {
        Cena a(modo);auto dados=a.capturar();
        for(auto& e:dados["inimigos"])e["posicao"][0]=30000;
        a.restaurar(dados);a.marcar();auto salvo=a.capturar();assert(salvo.contains("checkpoint"));
        assert(Persistencia::Slots::instancia().ler()==salvo);
        auto derrota=salvo;derrota["passos"]=600;derrota["finalizada"]=true;
        derrota["pontos"]=900;
        for(auto& e:derrota["jogadores"]){e["vivo"]=false;e["vida"]=0;}
        a.restaurar(derrota);assert(a.pode_recuperar_checkpoint());a.recuperar_checkpoint();
        assert(!a.get_finalizada() && a.get_assistida() && a.get_pontos()==0 && a.get_passos_sessao()==600);
        auto retomada=a.capturar();Cena b(modo);b.restaurar(retomada);assert(b.capturar()==retomada);
        auto ruim=retomada;ruim["checkpoint"]["jogadores"][0]["posicao"][0]="erro";
        bool rejeitou=false;try{b.restaurar(ruim);}catch(...){rejeitou=true;}
        assert(rejeitou && b.capturar()==retomada);
        auto fim=retomada;fim["finalizada"]=true;fim["vitoria"]=true;fim["pontos"]=200;
        b.restaurar(fim);assert(!b.registrar_resultado(modo==6?std::vector<std::string>{"Ana"}:std::vector<std::string>{"Ana","Bia"}));
        assert(!std::filesystem::exists("ranking.json") && !std::filesystem::exists("ranking-pontos.json"));
    }
}
