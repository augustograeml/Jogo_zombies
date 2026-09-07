#include "../Gerenciadores/gerenciador_eventos.h"
#include "../Gerenciadores/gerenciador_estados.h"
#include "../Observers/observer.h"
#include "../Estados/estado.h"

namespace Gerenciadores {

Gerenciador_Eventos::Gerenciador_Eventos() : pGrafico(Gerenciador_Grafico::get_instancia()) {}
Gerenciador_Eventos::~Gerenciador_Eventos() { encerrar(); }
void Gerenciador_Eventos::encerrar() {
    // O destrutor do observer se desanexa; remover antes evita invalidar iteradores.
    while (!lista_observers.empty()) {
        auto* observer = lista_observers.front();
        lista_observers.pop_front();
        delete observer;
    }
}
Gerenciador_Eventos* Gerenciador_Eventos::get_instancia() {
    static Gerenciador_Eventos instancia;
    return &instancia;
}
void Gerenciador_Eventos::executar() {
    while (pGrafico->get_Janela()->pollEvent(evento))
        if (processar_evento(evento)) break;
}
bool Gerenciador_Eventos::processar_evento(const sf::Event& evento) {
    auto* estados = Gerenciador_Estados::get_instancia();
    if (evento.type == sf::Event::Closed) {
        // Em caso de erro, mantem a janela e a partida abertas para tentar novamente.
        if (estados->salvar_partida()) pGrafico->fecharJanela();
        return true;
    }
    if(evento.type==sf::Event::Resized) { pGrafico->resetarCamera(); return false; }
    const int anterior = estados->get_estado_atual();
    try {
        if (auto* estado = estados->get_estado(anterior)) estado->tratar_evento(evento);
    } catch (const std::exception& erro) { estados->mensagem = erro.what(); }
    // Um Enter nao pode ativar tambem a proxima tela no mesmo ciclo.
    return estados->get_estado_atual() != anterior;
}
void Gerenciador_Eventos::anexar(Observers::Observer* obs) { if (obs) lista_observers.push_back(obs); }
void Gerenciador_Eventos::remover(Observers::Observer* obs) { lista_observers.remove(obs); }
void Gerenciador_Eventos::notificar(sf::Keyboard::Key tecla) {
    for (auto* obs : lista_observers) obs->atualizar(tecla);
}
}
