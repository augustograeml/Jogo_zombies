#include "../Recursos/catalogo.h"
namespace Recursos {
sf::IntRect area_cenario(const sf::Texture& textura) {
    // Os fundos originais incluem margens brancas de exportacao. Selecionar
    // apenas a arte ao desenhar preserva os arquivos e a geometria do mundo.
    const auto imagem=textura.copyToImage(); const auto tam=imagem.getSize();
    unsigned x0=tam.x,y0=tam.y,x1=0,y1=0; bool achou=false;
    for(unsigned y=0;y<tam.y;++y) for(unsigned x=0;x<tam.x;++x) {
        const auto c=imagem.getPixel(x,y);
        if(c.a && (c.r<250 || c.g<250 || c.b<250)) {
            achou=true; x0=std::min(x0,x); x1=std::max(x1,x); y0=std::min(y0,y); y1=std::max(y1,y);
        }
    }
    return achou?sf::IntRect(x0,y0,x1-x0+1,y1-y0+1):sf::IntRect(0,0,tam.x,tam.y);
}
}
