#include "../Persistencia/slots.h"
#include "../Persistencia/pontos.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <chrono>
int main() {
    using namespace Persistencia;
    auto pasta = std::filesystem::temp_directory_path() / ("zombies-persistencia-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(pasta);
    try {
        auto& slots = Slots::instancia(); slots.configurar_pasta(pasta);
        Json j{{"formato","zombies-partida"},{"versao",1},{"estado",6},{"fase",1},{"numero_jogadores",1},
               {"passos",60},{"acumulador",0.},{"partida_id","teste"},{"finalizada",false},
               {"vitoria",false},{"ranking_registrado",false},{"aleatorio","teste"},
               {"jogadores",Json::array({Json::object()})},{"inimigos",Json::array()},{"obstaculos",Json::array()}};
        slots.salvar(j); assert(slots.ler() == j); assert(slots.listar()[0].segundos == 1.);
        j["passos"] = 120; slots.salvar(j);
        assert(slots.listar()[0].backup_disponivel);
        { std::ofstream f(slots.caminho_atual()); f << "{ quebrado"; }
        assert(!slots.listar()[0].valido);
        bool falhou = false; try { slots.salvar(j); } catch (...) { falhou = true; } assert(falhou);
        assert(slots.recuperar().at("passos") == 60);
        assert(std::filesystem::exists(pasta / "partida.json.corrompido-1"));
        slots.selecionar(2); slots.salvar(j); assert(slots.listar()[1].segundos == 2.);
        slots.selecionar(1); assert(slots.ler().at("passos") == 60);
        // Falha na preparacao do backup nao pode truncar o checkpoint atual.
        std::filesystem::create_directory(pasta / "partida.json.bak.tmp");
        falhou = false; try { slots.salvar(j); } catch (...) { falhou = true; }
        assert(falhou && slots.ler().at("passos") == 60);
        std::filesystem::remove(pasta / "partida.json.bak.tmp");
        // Falha no arquivo principal conserva tanto o principal quanto o backup validado.
        std::filesystem::create_directory(pasta / "partida.json.tmp");
        falhou = false; try { slots.salvar(j); } catch (...) { falhou = true; }
        assert(falhou && slots.ler().at("passos") == 60);
        assert(ler_json(pasta / "partida.json.bak").at("passos") == 60);
        std::filesystem::remove(pasta / "partida.json.tmp");
        RepositorioPontos pontos(pasta / "ranking-pontos.json");
        pontos.registrar({"a",1,1,{" Alice "},100,10,false});
        pontos.registrar({"a",1,1,{"Alice"},1000,1,true});
        pontos.registrar({"b",1,1,{"Bob"},150,20,true});
        pontos.registrar({"c",1,1,{"Carol"},150,15,false});
        pontos.registrar({"d",1,1,{"David"},0,1,false});
        pontos.registrar({"e",2,1,{"Eve"},500,1,true});
        auto r = pontos.consultar(1,1); assert(r.size() == 3 && r[0].id == "c" && r[1].id == "b" && r[2].nomes[0] == "Alice");
        assert(pontos.consultar(1,1,CriterioTempo{})[0].id == "a");
        for (int i=0;i<10;++i) pontos.registrar({"extra"+std::to_string(i),1,1,{"X"},10,10,false});
        assert(pontos.consultar(1,1).size() == 8);
        pontos.registrar({"dupla",1,2,{"A","B"},200,10,false});
        assert(pontos.consultar(1,2).size() == 1);
        const auto ranking_antes = ler_json(pasta / "ranking-pontos.json");
        std::filesystem::create_directory(pasta / "ranking-pontos.json.tmp");
        falhou = false;
        try { pontos.registrar({"nova",1,1,{"N"},900,1,true}); } catch (...) { falhou = true; }
        assert(falhou && ler_json(pasta / "ranking-pontos.json") == ranking_antes);
        std::filesystem::remove(pasta / "ranking-pontos.json.tmp");
        pontos.registrar({"nova",1,1,{"N"},900,1,true});
        pontos.registrar({"nova",1,1,{"N"},900,1,true});
        assert(ler_json(pasta / "ranking-pontos.json").at("resultados").size() == ranking_antes.at("resultados").size()+1);
        slots.definir_validador([](const Json&) { throw std::runtime_error("invalido semanticamente"); });
        assert(!slots.listar()[0].valido && !slots.listar()[0].backup_disponivel);
        slots.definir_validador({}); slots.configurar_pasta({}); slots.selecionar(1);
        std::filesystem::remove_all(pasta);
        std::cout << "Persistencia extra: slots, backup, recuperacao, categorias, idempotencia e estrategias OK\n";
    } catch (...) { std::filesystem::remove_all(pasta); throw; }
}
