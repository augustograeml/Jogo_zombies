#pragma once

namespace Recursos::Configuracao {
// Todos os tempos de jogabilidade usam passos de simulacao, nunca quadros desenhados.
inline constexpr unsigned passos_por_segundo = 60;
inline constexpr float aceleracao_jogador = 0.1f;
inline constexpr float freio_jogador = 0.2f;
inline constexpr float velocidade_maxima_jogador = 4.f;
inline constexpr float gravidade = 0.1f;
inline constexpr float impulso_salto = 6.f;
inline constexpr unsigned passos_quadro_corrida = 2;
inline constexpr unsigned quadros_corrida = 27;
inline constexpr float altura_personagem = 50.f;
inline constexpr unsigned protecao_dano = 45;
inline constexpr int recarga_arqueiro = 100;
inline constexpr unsigned preparacao_arqueiro = 30;
inline constexpr float alcance_arqueiro = 650.f;
}
