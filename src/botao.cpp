#include "../Estados/Menus/botao.h"

namespace Estados
{
    namespace Menu
    {
        Botao::Botao()
        {
            fonte->loadFromFile("Design/fonte/sangue_escorrendo.ttf");
            texto.setFont(*fonte);
            texto.setFillColor(sf::Color::White);
            //talvez essa linha de baixo possa cagar o codigo, ficar experto
            texto.setOutlineColor(sf::Color::Black);
            texto.setPosition(coordenada);
        }
    
        Botao::~Botao()
        {

        }

        void Botao::set_coordenadas(sf::Vector2f coord)
        {
            coordenada = coord;
        }

        sf::Text Botao::get_texto()
        {
            return texto;
        }
    }
}