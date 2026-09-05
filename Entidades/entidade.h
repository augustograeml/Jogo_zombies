//codigo reaproveitado do Peteco

#pragma once
namespace Persistencia { class Serializador; }

#include "../ente.h"

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <memory>

#define TAM 50.f
#define TAM_CHEFAO 100.f
//tamanho da nossa flecha, la elwe
#define TAM_FLECHA 10.f
#define GRAVIDADE {0, 0.1f}

using namespace sf;

namespace Entidades
{
    class Entidade : public Ente
    {
        friend class Persistencia::Serializador;
        protected:
            bool pausado;

            std::shared_ptr<sf::Texture> Textura;
            sf::RectangleShape RS;
            sf::RenderWindow *window;

            sf::Vector2f posicao;
            sf::Vector2f velocidade;
            bool vivo;
            int vida;
            bool nochao;
        public:
            Entidade(sf::Vector2f pos = sf::Vector2f(0.f,0.f));
            virtual ~Entidade();

            void executar();

            virtual void salvar(std::ostringstream* entrada);

            virtual void mover() = 0;
            virtual void atualizar() = 0;
            virtual void colidir(Entidade* pE, int a) = 0;
            
            void morrer();

            const sf::Vector2f getPosicao() {return corpo.getPosition();}
            void setPosicao(sf::Vector2f p);
            const sf::Vector2f getTamanho()   {return corpo.getSize();}

            bool get_pause() {return pausado;}
            void set_pausado(bool p) {pausado = p;}

            void set_vida(int v) {vida = v;}
            int get_vida() const {return vida;}

            bool get_vivo() {return vivo;}
            void set_vivo(bool a ){vivo = a;}

            void set_nochao(bool n) {nochao = n;}
            
            Vector2f getVelocidade() {return velocidade;}
            void setVelocidade(sf::Vector2f v) {velocidade = v;}

            void setWindow( sf::RenderWindow* window)   {this->window = window;} 
            void setSkin(const std::string filename);
    };

}
