#include "../Estados/Fases/construtor_cenario.h"
#include "../Estados/Fases/fase.h"
#include "../Recursos/catalogo.h"
namespace Estados::Fases {
namespace {
// Marcador representa a celula imediatamente acima do apoio, nao o topo da cabeca.
Entidades::Entidade* assentar(Entidades::Entidade* e,sf::Vector2f celula) {
    const auto t=e->get_corpo()->getSize();
    e->Entidades::Entidade::setPosicao({celula.x+(TAM-t.x)/2,celula.y+TAM-t.y});
    return e;
}
}
void ConstrutorCenario::inimigos(const std::string& caminho, Listas::ListaEntidade& inimigos) {
    int y = 0;
    for (const auto& linha : Recursos::validar_mapa(caminho)) {
        int x = 0;
        for (char tipo : linha) {
            const sf::Vector2f pos(x++ * TAM, y * TAM);
            if (tipo == '3') inimigos.incluir(assentar(new Entidades::Personagens::Zumbi(pos, {0, 0}),pos));
            if (tipo == '4') inimigos.incluir(assentar(new Entidades::Personagens::Arqueiro(pos, {0, 0}),pos));
            if (tipo == '9') inimigos.incluir(assentar(new Entidades::Personagens::Gigante(pos, {0, 0}),pos));
        }
        ++y;
    }
}
void ConstrutorCenario::obstaculos(const std::string& caminho, Listas::ListaEntidade& obstaculos) {
    int y = 0;
    for (const auto& linha : Recursos::validar_mapa(caminho)) {
        int x = 0;
        for (char tipo : linha) {
            const sf::Vector2f pos(x++ * TAM, y * TAM);
            using namespace Entidades::Obstaculos;
            switch (tipo) {
                case '0': obstaculos.incluir(new Neve(pos)); break;
                case '5': obstaculos.incluir(assentar(new Espinho(pos),pos)); break;
                case '6': obstaculos.incluir(assentar(new Coracao(pos),pos)); break;
                case '7': obstaculos.incluir(new Musgo(pos)); break;
                case '8': obstaculos.incluir(assentar(new Caixa(pos),pos)); break;
                default: break;
            }
        }
        ++y;
    }
}
}
