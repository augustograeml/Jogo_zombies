//codigo reaproveitado do Peteco

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
class Ente;

#define LARGURA_TELA 1024.0  
#define ALTURA_TELA 1024.0

#define FONTE "../Design/Fonte/fonte_simas.ttf"

namespace Gerenciadores
{
    class Gerenciador_Grafico
    {
        private:
            mutable std::unique_ptr<sf::RenderWindow> janela;
            sf::View camera;
            //singleton
            Gerenciador_Grafico();

        public:
            ~Gerenciador_Grafico();
            Gerenciador_Grafico(const Gerenciador_Grafico&) = delete;
            Gerenciador_Grafico& operator=(const Gerenciador_Grafico&) = delete;
            bool inicializado() const { return static_cast<bool>(janela); }
            void encerrar();

            void desenharEnte(Ente* pE);
            void desenharTextura(sf::Texture* pT);
            void desenharFundo(sf::RectangleShape* pR);
            void desenharTexto(sf::Text* pT);
            bool get_JanelaAberta() const;
            static Gerenciador_Grafico* get_instancia();
            void mostrar();
            void limpar();
            void fecharJanela();
            sf::View get_view_interface() const;
            void resetarCamera();
            void centralizarCamera(sf::Vector2f p );//Entidades:Personagens:Jogador* pJ1, Entidades:Personagens:Jogador* pJ2 );
            sf::RenderWindow* get_Janela() const;
            //sf::Font* get_fonte();
    };

}