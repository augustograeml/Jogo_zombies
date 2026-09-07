#include "../Interface/preferencias.h"
#include "../Audio/efeitos.h"
#include "../Persistencia/arquivo.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>
int main() {
    const auto origem=std::filesystem::current_path();
    auto temporario=std::filesystem::temp_directory_path()/("zombies-interface-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(temporario); std::filesystem::current_path(temporario);
    // Arquivo antigo de audio permanece valido: categorias comecam em 100%.
    Persistencia::escrever_json("preferencias.json",{{"volume",60},{"mudo",false}});
    auto& audio=Audio::Preferencias::instancia();
    assert(audio.volume_evento(Logica::Evento::Salto)==60);
    Audio::configurar_categoria(Audio::Categoria::Combate,-50);
    assert(audio.volume_evento(Logica::Evento::ImpactoInimigo)==30);
    assert(audio.volume_evento(Logica::Evento::Vitoria)==60);
    Audio::configurar_categoria(Audio::Categoria::Resultado,-200);
    assert(audio.volume_evento(Logica::Evento::Derrota)==0);
    assert(Persistencia::ler_json("preferencias.json").at("categorias").at(1)==50);
    auto& p=Interface::Preferencias::instancia();
    Interface::configurar(0); assert(p.escala>1.1f);
    Interface::configurar(0); assert(p.escala==1.3f);
    Interface::configurar(0); assert(p.escala==1.f);
    Interface::configurar(1); Interface::configurar(2);
    const auto j=Persistencia::ler_json("interface.json");
    assert(j.at("legivel")==true && j.at("contraste")==true);
    // Falha de persistencia nao deixa uma preferencia aplicada apenas em memoria.
    std::filesystem::remove("interface.json"); std::filesystem::create_directory("interface.json");
    bool falhou=false; try { Interface::configurar(1); } catch(...) { falhou=true; }
    assert(falhou && p.legivel);
    Audio::desligar();
    std::filesystem::current_path(origem); std::filesystem::remove_all(temporario);
    std::cout<<"Preferencias, compatibilidade, categorias e falha atomica: OK\n";
}
