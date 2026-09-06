#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

namespace Interface {
std::string formatar_tempo(double segundos);
// O recorde e consultado ao entrar na fase; o desenho nao acessa o disco.
class PainelPartida {
    sf::Font fonte;
    int fase = 1, jogadores = 1;
    std::optional<double> recorde;
    std::string titulares;
    bool erro_recorde = false;
public:
    PainelPartida();
    void atualizar_recorde(int numero_fase, int quantidade);
    void desenhar(sf::RenderWindow& janela, double segundos) const;
};
}
