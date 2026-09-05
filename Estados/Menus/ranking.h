#pragma once
#include "../estado.h"
#include "../../Gerenciadores/gerenciador_grafico.h"
#include <SFML/Graphics.hpp>
#include <vector>
namespace Estados::Menus {
class Ranking : public Estado {
    sf::Font fonte;
    sf::Texture imagem;
    std::vector<sf::Text> textos;
    int fase = 1, jogadores = 1;
    void atualizar();
public:
    explicit Ranking(int id);
    void executar() override;
    void ao_entrar() override;
    void tratar_evento(const sf::Event& evento) override;
};
}
