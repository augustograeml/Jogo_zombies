#pragma once
#include <algorithm>
#include <vector>

namespace Logica {
enum class Evento { Salto, Disparo, Dano, Coleta, InimigoDerrotado, Vitoria };
struct Notificacao { Evento tipo; int valor = 0; };
// Observer: a simulacao publica fatos; som e pontuacao reagem independentemente.
class Observador {
public:
    virtual ~Observador() = default;
    virtual void receber(const Notificacao&) = 0;
};
class Eventos {
    std::vector<Observador*> observadores;
public:
    void assinar(Observador& o) {
        if (std::find(observadores.begin(), observadores.end(), &o) == observadores.end()) observadores.push_back(&o);
    }
    void remover(Observador& o) { observadores.erase(std::remove(observadores.begin(), observadores.end(), &o), observadores.end()); }
    void publicar(Notificacao e) { for (auto* o : observadores) o->receber(e); }
};
class Pontuacao : public Observador {
    int total = 0;
public:
    int pontos() const { return total; }
    void restaurar(int p) { total = p; }
    void receber(const Notificacao& e) override {
        if (e.tipo == Evento::Coleta || e.tipo == Evento::InimigoDerrotado || e.tipo == Evento::Vitoria)
            total = std::min(1000000000, total + e.valor);
    }
};
}
