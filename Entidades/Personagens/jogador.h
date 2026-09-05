#pragma once
namespace Persistencia { class Serializador; }
#include <SFML/Graphics.hpp>
#include "personagem.h"
#include "../../Gerenciadores/gerenciador_estados.h"

namespace Entidades
{
    namespace Personagens
    {
        class Jogador : public Personagem
        {
        friend class Persistencia::Serializador;
        protected:
            static Gerenciadores::Gerenciador_Estados* PGEstados;
            bool jogador2, leu_fase;
            float poder;
            std::string nome;
            double tempo;


        public:
            Jogador(sf::Vector2f pos, sf::Vector2f vel, bool jog2);
            ~Jogador();

            void atualizar();
            void executar();
            void colidir(Entidade* pE, int a);

            void set_tempo(double x) {tempo = x;}
            double get_tempo() {return tempo;}
            bool eh_jogador2() const {return jogador2;}

            void set_nome(std::string a)  {nome = a;}
            string get_nome(){return nome;}

            void tela_pause();

            void mover();
            void mover_jog2();
            void atacar(Entidade* jg);
            void salvar(std::ostringstream* entrada);
           
        };

    }

}
