//codigo reaproveitado do Monitor Giovane

#pragma once

#include "../Gerenciadores/gerenciador_eventos.h"
#include "../Gerenciadores/gerenciador_estados.h"
#include "../Gerenciadores/gerenciador_grafico.h"

namespace Observers
{
    class Observer
    {
        protected:
            static Gerenciadores::Gerenciador_Eventos* pGer_Eventos;
            static Gerenciadores::Gerenciador_Estados* pGer_Estados;
            static Gerenciadores::Gerenciador_Grafico* pGer_Grafico;
        public:
            Observer();
            virtual ~Observer();
            virtual void atualizar(sf::Keyboard::Key k) = 0;
    };

}