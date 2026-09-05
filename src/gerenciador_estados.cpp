#include "../Gerenciadores/gerenciador_estados.h"
#include "../Estados/estado.h"
#include "../Estados/Fases/fase.h"
#include <stdexcept>

namespace Gerenciadores {
Gerenciador_Estados* Gerenciador_Estados::instancia = nullptr;
Gerenciador_Estados::Gerenciador_Estados() : estadoAtual(0), fase(-1), estados(11, nullptr) {}
Gerenciador_Estados::~Gerenciador_Estados() { for (auto* estado : estados) delete estado; }
Gerenciador_Estados* Gerenciador_Estados::get_instancia() {
    if (!instancia) instancia = new Gerenciador_Estados;
    return instancia;
}
Estados::Estado* Gerenciador_Estados::get_estado(int id) {
    return id >= 0 && id < static_cast<int>(estados.size()) ? estados[id] : nullptr;
}
void Gerenciador_Estados::set_estado_atual(int id) {
    auto* destino = get_estado(id);
    if (!destino) throw std::runtime_error("Estado de destino nao foi criado.");
    if (id == estadoAtual) return;
    // Mesmo sem permissao de escrita, a simulacao deve parar na pausa.
    if (estadoAtual >= 6 && estadoAtual <= 9 && id == 5) salvar_partida();
    estadoAtual = id;
    if (id >= 6 && id <= 9) fase = id;
    destino->ao_entrar();
}
bool Gerenciador_Estados::salvar_partida() {
    auto* partida = dynamic_cast<Estados::Fases::Fase*>(get_estado(fase));
    if (!partida) return true;
    try {
        partida->salvar();
        mensagem = "Partida salva.";
        return true;
    } catch (const std::exception& erro) {
        mensagem = std::string("Nao foi possivel salvar: ") + erro.what();
        return false;
    }
}
void Gerenciador_Estados::deleta_estados(int id) {
    if (id == estadoAtual) throw std::runtime_error("Nao e possivel excluir o estado em execucao.");
    delete estados.at(id);
    estados.at(id) = nullptr;
}
void Gerenciador_Estados::set_fase(int id) { fase = id; }
int Gerenciador_Estados::get_estado_atual() { return estadoAtual; }
int Gerenciador_Estados::get_fase() { return fase; }
void Gerenciador_Estados::adicionar_estado(Estados::Estado* estado) {
    if (!estado) return;
    auto& anterior = estados.at(estado->getID());
    if (anterior == estado) return;
    if (anterior && estado->getID() == estadoAtual)
        throw std::runtime_error("Nao e possivel substituir o estado em execucao.");
    delete anterior;
    anterior = estado;
}
void Gerenciador_Estados::executar() {
    auto* estado = get_estado(estadoAtual);
    if (estado) estado->executar();
}
}
