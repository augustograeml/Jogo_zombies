#include "../Entidades/Obstaculos/obstaculo.h"

namespace Entidades
{
    namespace Obstaculos
    {
        Obstaculo::Obstaculo(sf::Vector2f pos, bool dano, bool cura, bool atrapalho, bool neve, bool musgo) :
        Entidade(pos), danoso(dano), curoso(cura), atrapalhante(atrapalho), escorregadio(neve), gosmento(musgo)
        {

        }

        Obstaculo::~Obstaculo()
        {

        }
    }
}