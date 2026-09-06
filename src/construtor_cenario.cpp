#include "../Estados/Fases/construtor_cenario.h"
#include "../Estados/Fases/fase.h"
#include "../Recursos/catalogo.h"
namespace Estados::Fases {
void ConstrutorCenario::inimigos(const std::string& caminho, Listas::ListaEntidade& inimigos) {
    int y = 0;
    for (const auto& linha : Recursos::validar_mapa(caminho)) {
        int x = 0;
        for (char tipo : linha) {
            const sf::Vector2f pos(x++ * TAM, y * TAM);
            if (tipo == '3') inimigos.incluir(new Entidades::Personagens::Zumbi(pos, {0, 0}));
            if (tipo == '4') inimigos.incluir(new Entidades::Personagens::Arqueiro(pos, {0, 0}));
            if (tipo == '9') inimigos.incluir(new Entidades::Personagens::Gigante(pos, {0, 0}));
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
                case '5': obstaculos.incluir(new Espinho(pos)); break;
                case '6': obstaculos.incluir(new Coracao(pos)); break;
                case '7': obstaculos.incluir(new Musgo(pos)); break;
                case '8': obstaculos.incluir(new Caixa(pos)); break;
                default: break;
            }
        }
        ++y;
    }
}
}
