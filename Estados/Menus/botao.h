#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

using namespace std;

namespace Estados
{
    namespace Menu
    {
        class Botao
        {
            private:
                sf::RectangleShape* botao;
                sf::Font* fonte;

                bool selecionado;

                string opcao;
                sf::Vector2f coordenada;
                sf::Text texto;
                //tamanhos para fonte pq cada palavra tem um tamanho de fonte diferente
                size_t tamanho;
            public:
                Botao();
                ~Botao();

                void set_texto(string t);
                void set_coordenadas(sf::Vector2f coord);
                sf::Text get_texto();
        };
    }
}