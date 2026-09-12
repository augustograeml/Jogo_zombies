#include "../Recursos/catalogo.h"
namespace Recursos {
const std::vector<Quadro>& caminhada_luigi() {
    static const std::vector<Quadro> quadros=[] {
        sf::Image imagem;
        if(!imagem.loadFromFile(caminho("imagens/luigi_smas_folha.png").string()))
            throw std::runtime_error("Folha de caminhada do Luigi nao encontrada.");
        // Cor de fundo das celulas na folha original; o arquivo nao e modificado.
        for(unsigned tom:{52u,84u,116u,148u}) imagem.createMaskFromColor(sf::Color(0,tom,tom));
        auto textura=std::make_shared<sf::Texture>();
        if(!textura->loadFromImage(imagem)) throw std::runtime_error("Falha ao carregar Luigi.");
        textura->setSmooth(false);
        std::vector<Quadro> q;
        // Dois contatos alternados e duas passagens, sem distorcer pernas ou tronco.
        for(int celula:{0,3,0,4,0,3,0,4}) q.push_back({textura,{20+52*celula,583,26,32}});
        return q;
    }(); return quadros;
}
}
