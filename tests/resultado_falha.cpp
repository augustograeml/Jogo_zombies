#include "../Estados/Fases/fase1.h"
#include "../Persistencia/slots.h"
#include "../Persistencia/pontos.h"
#include "../Persistencia/ranking.h"
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
int main() {
    namespace fs = std::filesystem;
    using Persistencia::Json;
    using Estados::Fases::Fase1;
    const auto anterior = fs::current_path();
    const auto pasta = fs::temp_directory_path() / ("zombies-resultado-falha-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(pasta);
    fs::current_path(pasta);
    try {
        Fase1 fase(6,false);
        auto final = fase.capturar();
        final["finalizada"] = true; final["vitoria"] = true; final["pontos"] = 500; final["passos"] = 600;
        fase.restaurar(final);
        auto& slots = Persistencia::Slots::instancia();
        slots.definir_validador([](const Json& j) { Fase1 validacao(6,true); validacao.restaurar(j); });
        fase.salvar();
        { std::ofstream arquivo("ranking-pontos.json"); arquivo << "{arquivo quebrado"; }
        bool falhou = false;
        try { fase.registrar_resultado({"Teste"}); } catch (...) { falhou = true; }
        assert(falhou && !fase.get_nomes_confirmados() && !fase.get_ranking_registrado());
        Fase1 recarregada(6,true); recarregada.restaurar(fase.capturar());
        fase.salvar(); assert(slots.listar()[0].valido);
        const auto tempo_antes = Persistencia::ler_json("ranking.json");
        fs::remove("ranking-pontos.json");
        recarregada.registrar_resultado({"Teste"}); recarregada.registrar_resultado({"Teste"});
        assert(Persistencia::ler_json("ranking.json") == tempo_antes);
        assert(Persistencia::RepositorioPontos().consultar(1,1).size() == 1);
        assert(recarregada.get_nomes_confirmados() && recarregada.get_ranking_registrado());
        // Falha na substituicao durante recuperacao preserva arquivo atual e backup.
        const auto atual = slots.ler(); const auto backup = Persistencia::ler_json("partida.json.bak");
        fs::create_directory("partida.json.tmp");
        falhou = false; try { slots.recuperar(); } catch (...) { falhou = true; }
        assert(falhou && slots.ler() == atual && Persistencia::ler_json("partida.json.bak") == backup);
        assert(Persistencia::ler_json("partida.json.corrompido-1") == atual);
        slots.definir_validador({});
        fs::current_path(anterior); fs::remove_all(pasta);
        std::cout << "Resultado parcial: falha de pontos preserva save; retry nao duplica ranking; recovery atomico OK\n";
    } catch (const std::exception& erro) {
        Persistencia::Slots::instancia().definir_validador({});
        fs::current_path(anterior); fs::remove_all(pasta);
        std::cerr << erro.what() << '\n'; return 1;
    }
}
