#pragma once
#include <SFML/System/Vector2.hpp>
namespace Recursos::Escala {
// Unidade do mapa mantida; um humano mede 1,6 blocos.
inline constexpr float bloco=50.f;
inline constexpr float humano=1.6f*bloco;
// Caixas do tronco: maos e pes podem ultrapassar discretamente a largura.
inline const sf::Vector2f jogador{.5f*humano,humano};
inline const sf::Vector2f zumbi{.6f*humano,1.1f*humano};
inline const sf::Vector2f arqueiro{.48f*humano,1.05f*humano};
inline const sf::Vector2f gigante{1.1f*humano,1.75f*humano};
inline constexpr float caixa=.48f*humano;
inline constexpr float espinho=.4f*humano;
inline constexpr float coracao=.3f*humano;
inline const sf::Vector2f projetil{.3f*humano,.0625f*humano};
}
