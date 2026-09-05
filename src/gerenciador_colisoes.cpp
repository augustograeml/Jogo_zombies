// codigo reaproveitado do Peteco

#include "../Gerenciadores/gerenciador_colisoes.h"
#include "../Entidades/Personagens/jogador.h"
#include "../Entidades/Personagens/inimigo.h"
#include "../Entidades/Personagens/arqueiro.h"
#include "../Entidades/Obstaculos/obstaculo.h"
#include "../Entidades/Obstaculos/espinho.h"
#include "../Entidades/Obstaculos/coracao.h"
#include "../Entidades/Obstaculos/caixa.h"
#include "../Entidades/Obstaculos/neve.h"
#include "../Entidades/Obstaculos/musgo.h"
#include <math.h>
#include <iostream>

using namespace std;

#define ACL 0.2

namespace Gerenciadores
{
    Gerenciador_Colisoes::Gerenciador_Colisoes() : obstaculos(nullptr),
                                                   jogadores(nullptr),
                                                   inimigos(nullptr),
                                                   zumbis(nullptr), atiradores(nullptr), gigantes(nullptr),
                                                   projeteis(nullptr),
                                                   sem_inimigos(false),
                                                   sem_jogadores(false)
    {
    }

    Gerenciador_Colisoes::~Gerenciador_Colisoes()
    {
        jogadores = nullptr;
        obstaculos = nullptr;
        inimigos = nullptr;
        projeteis = nullptr;
    }

    void Gerenciador_Colisoes::colisao_jogadores_obstaculos()
    {
        Listas::Lista<Entidades::Entidade>::Iterador obst;
        Listas::Lista<Entidades::Entidade>::Iterador jog = jogadores->get_primeiro();

        while (jog != nullptr)
        {
            obst = obstaculos->get_primeiro();
            while (obst != nullptr)
            {
                if ((*obst)->get_vivo())
                {
                    int id_colisao = colidiu(*jog, *obst);
                    if (id_colisao)
                    {
                        (*obst)->colidir(*jog, id_colisao);
                        (*jog)->colidir(*obst, id_colisao);
                    }
                }
                obst++;
            }
            jog++;
        }
    }
    void Gerenciador_Colisoes::colisao_inimigos_obstaculos()
    {
        Listas::Lista<Entidades::Entidade>::Iterador obst;
        Listas::Lista<Entidades::Entidade>::Iterador inim = inimigos->get_primeiro();
        while (inim != nullptr)
        {
            obst = obstaculos->get_primeiro();
            while (obst != nullptr)
            {
                if ((*obst)->get_vivo())
                    colidiu(*inim, *obst);

                obst++;
            }
            inim++;
        }
    }

    void Gerenciador_Colisoes::colisao_jogadores_inimigos()
    {
        Listas::Lista<Entidades::Entidade>::Iterador jog = jogadores->get_primeiro();
        Listas::Lista<Entidades::Entidade>::Iterador inim = inimigos->get_primeiro();
        while (jog != nullptr)
        {
            inim = inimigos->get_primeiro();
            while (inim != nullptr)
            {
                if ((*inim)->get_vivo())
                {
                    int j = colidiu(*jog, *inim);

                    if (j)
                        (*inim)->colidir(*jog, j);
                }
                inim++;
            }
            jog++;
        }
    }
    void Gerenciador_Colisoes::colisao_jogadores_projeteis()
    {
        for (auto inim = inimigos->get_primeiro(); inim != nullptr; ++inim) {
            auto* flechas = static_cast<Entidades::Personagens::Inimigo*>(*inim)->get_projeteis();
            if (!flechas) continue;
            for (auto& flecha : *flechas) {
                if (!flecha.get_vivo()) continue;
                for (auto jog = jogadores->get_primeiro(); jog != nullptr; ++jog) {
                    if ((*jog)->get_vivo() && (*jog)->get_corpo()->getGlobalBounds().intersects(
                            flecha.get_corpo()->getGlobalBounds())) {
                        flecha.colidir(*jog, 1);
                        break;
                    }
                }
            }
        }
    }
    void Gerenciador_Colisoes::colisao_obstaculos_projeteis()
    {
        for (auto inim = inimigos->get_primeiro(); inim != nullptr; ++inim) {
            auto* flechas = static_cast<Entidades::Personagens::Inimigo*>(*inim)->get_projeteis();
            if (!flechas) continue;
            for (auto& flecha : *flechas) {
                if (!flecha.get_vivo()) continue;
                for (auto obst = obstaculos->get_primeiro(); obst != nullptr; ++obst) {
                    if ((*obst)->get_vivo() && (*obst)->get_corpo()->getGlobalBounds().intersects(
                            flecha.get_corpo()->getGlobalBounds())) {
                        flecha.morrer();
                        break;
                    }
                }
            }
        }
    }

    bool Gerenciador_Colisoes::get_inimigos_vivos()
    {
        int ini_vivos = 0;
        Listas::Lista<Entidades::Entidade>::Iterador inimg = inimigos->get_primeiro();
        while (inimg != nullptr)
        {
            if ((*inimg)->get_vivo())
                ini_vivos++;
            inimg++;
        }
        sem_inimigos = ini_vivos == 0;

        return sem_inimigos;
    }

    bool Gerenciador_Colisoes::get_jogadores_vivos()
    {
        int jog_vivos = 0;
        Listas::Lista<Entidades::Entidade>::Iterador joga = jogadores->get_primeiro();
        while (joga != nullptr)
        {
            if ((*joga)->get_vivo())
                jog_vivos++;
            joga++;
        }
        sem_jogadores = jog_vivos == 0;

        return sem_jogadores;
    }

    void Gerenciador_Colisoes::gerenciar_colisoes()
    {
        colisao_jogadores_obstaculos();
        colisao_jogadores_inimigos();
        colisao_inimigos_obstaculos();
        colisao_jogadores_projeteis();
        colisao_obstaculos_projeteis();
        get_inimigos_vivos();
        get_jogadores_vivos();
    }

    int Gerenciador_Colisoes::colidiu(Entidades::Entidade *e1, Entidades::Entidade *e2)
    {
        if (!e1 || !e2 || !e1->get_vivo() || !e2->get_vivo()) return 0;

        sf::Vector2f pos1 = e1->getPosicao(), pos2 = e2->getPosicao(), tam1 = e1->getTamanho(), tam2 = e2->getTamanho(),
                     d(fabs(pos1.x - pos2.x) - ((tam1.x + tam2.x) / 2.f),
                       fabs(pos1.y - pos2.y) - ((tam1.y + tam2.y) / 2.f));

        if (d.x < 0 && d.y < 0)
        {
            if (d.x < d.y)
            {
                if (pos1.y <= pos2.y)
                {
                    e1->setPosicao(sf::Vector2f(e1->getPosicao().x, e2->getPosicao().y - (tam1.y + tam2.y) / 2));
                    e1->set_nochao(true);
                    e1->setVelocidade(sf::Vector2f(e1->getVelocidade().x, -e1->getVelocidade().y * ACL));
                    return 4;
                }
                else
                {
                    e1->setPosicao(sf::Vector2f(e1->getPosicao().x, e2->getPosicao().y + (tam1.y + tam2.y) / 2));
                    e1->setVelocidade(sf::Vector2f(e1->getVelocidade().x, -e1->getVelocidade().y * ACL));
                    return 2;
                }
            }
            else
            {
                if (pos1.x >= pos2.x)
                {
                    e1->setPosicao(sf::Vector2f(e2->getPosicao().x + (tam1.x + tam2.x) / 2, e1->getPosicao().y));
                    e1->setVelocidade(sf::Vector2f(-e1->getVelocidade().x * ACL, e1->getVelocidade().y));
                    return 1;
                }
                else
                {
                    e1->setPosicao(sf::Vector2f(e2->getPosicao().x - (tam1.x + tam2.x) / 2, e1->getPosicao().y));
                    e1->setVelocidade(sf::Vector2f(-e1->getVelocidade().x * ACL, e1->getVelocidade().y));
                    return 3;
                }
            }
        }

        return 0;
    }
}
