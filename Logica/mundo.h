#pragma once
#include "sessao.h"
#include "eventos.h"
#include "../Listas/listaEntidades.h"
#include "../Gerenciadores/gerenciador_colisoes.h"
#include <random>
#include <string>
namespace Logica {
// Dependencias explicitas, sem acesso a telas, janela ou gerenciador de estados.
// As referencias pertencem a Fase e sao validas somente durante a operacao.
struct Mundo {
    Listas::ListaEntidade &jogadores, &inimigos, &obstaculos;
    Gerenciadores::Gerenciador_Colisoes& gC;
    Sessao& sessao;
    ResultadoPartida& resultado;
    Pontuacao& pontuacao;
    Eventos& eventos;
    std::mt19937& motor_fase;
    std::string& partida_id;
    int estado, num_jogadores;
    int numero_fase() const { return estado<8?1:2; }
};
enum class FimPasso { Nenhum, Vitoria, Derrota };
FimPasso simular(Mundo& mundo);
}
