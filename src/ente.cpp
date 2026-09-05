//codigo reaproveitado do Peteco

#include "../ente.h"

Gerenciadores::Gerenciador_Grafico* Ente::pGG(Gerenciadores::Gerenciador_Grafico::get_instancia());

Ente::Ente(sf::Vector2f tam) : id(0), corpo(tam)
{

}

Ente::~Ente()
{

}

sf::RectangleShape* Ente::get_corpo()
{
    return &corpo;
}

void Ente::desenhar()
{
    pGG->desenharEnte(this);
}

