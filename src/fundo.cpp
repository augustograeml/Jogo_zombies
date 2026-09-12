#include "../Recursos/catalogo.h"
namespace Recursos {
sf::IntRect area_cenario(const sf::Texture& textura) {
    // Recorte central 5:3 corresponde ao fundo de 2000x1200, sem esticar arte.
    const auto t=textura.getSize();
    int w=t.x,h=t.y;
    if(w*3>h*5) w=h*5/3; else h=w*3/5;
    return {static_cast<int>((t.x-w)/2),static_cast<int>((t.y-h)/2),w,h};
}
}
