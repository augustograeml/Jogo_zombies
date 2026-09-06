//codigo reaproveitado do Peteco

#pragma once
namespace Persistencia { class Serializador; }

#include "Gerenciadores/gerenciador_grafico.h"

#include <SFML/Graphics.hpp>

class Ente
{
    friend class Persistencia::Serializador;
    protected:
        int id;
        static Gerenciadores::Gerenciador_Grafico* pGG;
        sf::RectangleShape corpo;
    public:
        Ente(sf::Vector2f tamanho = sf::Vector2f(20.f, 20.f));
        virtual ~Ente();
        virtual void executar() = 0;
        sf::RectangleShape* get_corpo();
        virtual void desenhar();
};
