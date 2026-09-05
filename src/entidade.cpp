//codigo reaproveitado do Peteco

#include "../Entidades/entidade.h"
#include <iostream>
#include <map>
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
    void Entidade::setSkin(const std::string filename)
    {
        // Compartilhar a textura evita reler a mesma imagem para cada bloco do mapa.
        static std::map<std::string, std::weak_ptr<sf::Texture>> cache;
        Textura = cache[filename].lock();
        if (!Textura) {
            Textura = std::make_shared<sf::Texture>();
            if (!Textura->loadFromFile(filename))
                throw std::runtime_error("Imagem nao encontrada: " + filename);
            cache[filename] = Textura;
        }
        corpo.setTexture(Textura.get());
    }

}
