#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Interface {
std::string formatar_tempo(double segundos);
float altura_painel();
// O recorde e consultado ao entrar na fase; o desenho nao acessa o disco.
class PainelPartida {
    sf::Font fonte;
    int fase = 1, jogadores = 1;
    std::optional<double> recorde;
    std::string titulares;
    bool erro_recorde = false;
    long long recorde_pontos = 0;
public:
    PainelPartida();
    void atualizar_recorde(int numero_fase, int quantidade);
    void desenhar(sf::RenderWindow& janela, double segundos, int pontos=0, const std::vector<int>& vidas={}) const;
};
}
