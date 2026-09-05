//codigo reaproveitado do Canal Terminal Root

#pragma once
#include "../estado.h"
#include "../../Gerenciadores/gerenciador_grafico.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

namespace Estados
{
    namespace Menus
    {
        class Menu : public Estado
        {
            protected:
                static Gerenciadores::Gerenciador_Grafico* pGG;
                int pos;
                //o deselecionar é necessario porque como o loop pra rodar o jogo eh rapido pode acontecer de uma opcao ser selecionada duplicada no menu
                bool selecionado, deselecionado, jogador2;

                sf::Font* fonte;
                sf::Texture* imagem;
                sf::Event evento;
                
                vector<string> opcoes;
                vector<sf::Vector2f> coordenadas;
                vector<sf::Text> textos;
                //tamanhos para fonte pq cada palavra tem um tamanho de fonte diferente
                vector<size_t> tamanhos; 
            public:
                Menu(int id);
                ~Menu();

                //vai ser chamado pra quando for escolher um ou dois jogadores no menu

                virtual void inicializa_valores() = 0;
                virtual void loop_evento() = 0;

                void set_jogador2(bool j);
                bool get_jogador2() const;

                void cima();
                void baixo(int i);
                virtual void selecionar() = 0;

                void mostrar_menu();
                void executar();
                void tratar_evento(const sf::Event& evento) override;
        };
    }
}
