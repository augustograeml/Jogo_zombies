#include "../Recursos/catalogo.h"
#include "../Estados/Fases/fase2.h"
#include "../Persistencia/aleatorio.h"
namespace Estados::Fases {
Fase2::Fase2(int id, bool carregar) : Fase(id, carregar) {
    if (id != 8 && id != 9) throw std::runtime_error("Identificador de fase 2 invalido.");
    if (!Textura.loadFromFile(Recursos::caminho("Design/imagens/cenario_op22.png").string()))
        throw std::runtime_error("Imagem da fase 2 nao encontrada.");
    shape.setSize({2000.f, 1200.f});
    shape.setScale({1.2f, 1.f});
    shape.setTexture(&Textura);
    shape.setTextureRect(Recursos::area_cenario(Textura));
    if (!carregar) {
        criar_cenario("Design/cenario/cenario_fase2.txt");
        criar_inimigos("Design/cenario/cenario_fase2.txt");
    }
    motor_fase = Persistencia::motor();
}
}
