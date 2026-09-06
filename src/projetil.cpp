#include "../Entidades/projetil.h"
#include "../Persistencia/entidades.h"
namespace Entidades {
Projetil::Projetil(sf::Vector2f pos, bool dir) : Entidade(pos), dano(2), direcao(dir) {
    corpo.setSize({20, 5}); corpo.setFillColor(sf::Color::Red);
    velocidade = {dir ? 12.f : -12.f, 0.f};
}
Projetil::~Projetil() = default;
void Projetil::executar() { if (vivo) mover(); }
void Projetil::atualizar() { corpo.move(velocidade); }
void Projetil::mover() {
    if (nochao) { morrer(); return; }
    corpo.move(sf::Vector2f(0.f, 0.1f) + velocidade);
    const auto pos = corpo.getPosition();
    if (pos.x < -200 || pos.x > 4000 || pos.y < -2000 || pos.y > 2500) morrer();
}
void Projetil::atirar() {}
void Projetil::salvar(std::ostringstream* saida) { *saida << Persistencia::Serializador::salvar(*this).dump(); }
void Projetil::danar(Entidade* alvo) { if (alvo) alvo->receber_dano(dano); }
void Projetil::colidir(Entidade* alvo, int lado) {
    if (vivo && lado) { danar(alvo); morrer(); }
}
}
