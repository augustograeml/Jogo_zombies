#include "../Estados/Fases/fase1.h"
#include "../Persistencia/aleatorio.h"
namespace Estados::Fases {
Fase1::Fase1(int id, bool carregar) : Fase(id, carregar) {
    if (id != 6 && id != 7) throw std::runtime_error("Identificador de fase 1 invalido.");
    if (!Textura.loadFromFile("Design/imagens/cenario_op11.png"))
        throw std::runtime_error("Imagem da fase 1 nao encontrada.");
    shape.setSize({2000.f, 1200.f});
    shape.setTexture(&Textura);
    if (!carregar) {
        criar_cenario("Design/cenario/cenario_fase1.txt");
        criar_inimigos("Design/cenario/cenario_fase1.txt");
    }
    motor_fase = Persistencia::motor();
}
}
