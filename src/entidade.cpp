//codigo reaproveitado do Peteco

#include "../Entidades/entidade.h"
#include <iostream>
#include "../Recursos/catalogo.h"
#include <algorithm>
#include "../Entidades/Personagens/jogador.h"
#include <stdexcept>
#include "../Persistencia/entidades.h"

using namespace std;

namespace Entidades
{
    Entidade::Entidade(sf::Vector2f pos):
        Ente(sf::Vector2f(TAM, TAM)), pausado(false), window(nullptr), posicao(pos),
        velocidade(0.f, 0.f), vivo(true), vida(20), nochao(false)
    {
        corpo.setPosition(posicao);
    }

    Entidade::~Entidade()
    {

    }

    void Entidade::executar()
    {
            this->mover();
            this->atualizar();
    }
    void Entidade::salvar(std::ostringstream* entrada) {
        *entrada << Persistencia::Serializador::salvar(*this).dump();
    }

    void Entidade::setPosicao(Vector2f p)
    {
        corpo.setPosition(p);
    }
    void Entidade::morrer()
    {
        set_vivo(0);
    }
    bool Entidade::receber_dano(int dano, unsigned duracao) {
        if (!vivo || dano <= 0 || protecao) return false;
        vida = std::max(0, vida - dano);
        reacao_visual=12;
        if (dynamic_cast<Personagens::Jogador*>(this)) protecao = duracao;
        if (vida == 0) morrer();
        return true;
    }
    void Entidade::curar(int quantidade, int maxima) {
        if (vivo && quantidade > 0) vida = std::min(maxima, vida + quantidade);
    }
    void Entidade::setSkin(const std::string filename)
    {
        Textura = Recursos::textura(filename);
        corpo.setTexture(Textura.get());
    }

}
