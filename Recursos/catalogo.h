#pragma once
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Recursos {
// ZOMBIES_RECURSOS pode indicar a pasta do projeto ou diretamente Design.
// Sem a variavel, procura Design no diretorio atual e junto ao executavel.
std::filesystem::path caminho(const std::string& relativo);
std::shared_ptr<sf::Texture> textura(const std::string& relativo);
struct Quadro {
    std::shared_ptr<sf::Texture> textura;
    sf::IntRect regiao; // Limites dos pixels visiveis: alinhamento pelos pes.
};
const std::vector<Quadro>& corrida();
// Legenda: espaco=vazio; 0=neve; 1/2=marcadores; 3=zumbi; 4=arqueiro;
// 5=espinho; 6=coracao; 7=musgo; 8=caixa; 9=gigante.
std::vector<std::string> validar_mapa(const std::string& relativo);
}
