#pragma once
#include "../estado.h"
#include "../../Gerenciadores/gerenciador_grafico.h"
#include <SFML/Graphics.hpp>
#include <vector>
namespace Estados::Menus {
class Ranking : public Estado {
    sf::Texture imagem;
    struct Linha { std::string nomes,tempo,pontos; bool vitoria; };
    std::vector<Linha> linhas;
    bool erro=false;
    int fase = 1, jogadores = 1;
    bool por_pontos = false;
    void atualizar();
public:
    explicit Ranking(int id);
    void executar() override;
    void ao_entrar() override;
    void tratar_evento(const sf::Event& evento) override;
};
}
