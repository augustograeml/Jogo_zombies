#pragma once
#include "Estados/Menus/menu_principal.h"
#include "Estados/Menus/ranking.h"
#include "Estados/Menus/pause.h"
#include "Estados/Menus/nome.h"
#include "Estados/Menus/menu_fase.h"
#include "Estados/Menus/menu_jogadores.h"
#include "Gerenciadores/gerenciador_grafico.h"
#include "Gerenciadores/gerenciador_estados.h"
#include "Gerenciadores/gerenciador_eventos.h"
class Jogo {
    Gerenciadores::Gerenciador_Grafico* pG;
    Gerenciadores::Gerenciador_Estados* pE;
    Gerenciadores::Gerenciador_Eventos* pEv;
public:
    Jogo();
    ~Jogo();
    void Executar();
    void reseta_fase();
};
