#include "../Gerenciadores/gerenciador_grafico.h"
#include "../Gerenciadores/gerenciador_estados.h"
#include "../Gerenciadores/gerenciador_eventos.h"
#include "../Audio/efeitos.h"
#include <cassert>
#include <iostream>
int main(int argc,char**) {
    auto* grafico=Gerenciadores::Gerenciador_Grafico::get_instancia();
    assert(!grafico->inicializado());
    auto* estados=Gerenciadores::Gerenciador_Estados::get_instancia();
    auto* eventos=Gerenciadores::Gerenciador_Eventos::get_instancia();
    assert(!grafico->inicializado());
    Audio::interromper();
    estados->encerrar(); eventos->encerrar(); grafico->encerrar();
    assert(!grafico->inicializado());
    assert(grafico==Gerenciadores::Gerenciador_Grafico::get_instancia());
    if(argc>1) {
        grafico->get_Janela(); assert(grafico->get_JanelaAberta());
        grafico->encerrar(); assert(!grafico->inicializado());
        grafico->get_Janela(); assert(grafico->get_JanelaAberta());
        grafico->encerrar();
    }
    std::cout << "Servicos: Singleton preservado, inicio sem dispositivo e encerramento idempotente OK\n";
}
