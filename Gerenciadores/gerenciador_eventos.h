//codigo reaproveitado do Peteco

#pragma once
#include "gerenciador_grafico.h"
#include <SFML/Graphics.hpp>
#include <list>

using namespace sf;
using namespace std;

namespace Observers
{
    class Observer;
}

namespace Gerenciadores
{
    class Gerenciador_Eventos
    {
        private:
            Gerenciador_Grafico* pGrafico;

            Gerenciador_Eventos();
            Event evento;

            list<Observers::Observer*> lista_observers;
            list<Observers::Observer*>::iterator it;
        public:
            ~Gerenciador_Eventos();
            void encerrar();
            Gerenciador_Eventos(const Gerenciador_Eventos&) = delete;
            Gerenciador_Eventos& operator=(const Gerenciador_Eventos&) = delete;
            void executar();
            bool processar_evento(const sf::Event& evento);
            static Gerenciador_Eventos* get_instancia();

            void anexar(Observers::Observer* obs);
            void remover(Observers::Observer* obs);
            void notificar(sf::Keyboard::Key k);
    };

}
