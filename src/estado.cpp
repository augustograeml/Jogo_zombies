#include "../Estados/estado.h"

namespace Estados
{
    Gerenciadores::Gerenciador_Estados* Estado::pGE(Gerenciadores::Gerenciador_Estados::get_instancia());

    Estado::Estado(int i) :  id(i)
    {
        // O chamador registra somente objetos completamente construidos.
    }

    Estado::~Estado()
    {

    }

    void Estado::setID(int num)
    {
        id = num;
    }

    int Estado::getID()
    {
        return id;
    }

}
