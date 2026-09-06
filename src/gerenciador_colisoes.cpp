// codigo reaproveitado do Peteco

#include "../Gerenciadores/gerenciador_colisoes.h"
#include "../Entidades/Personagens/jogador.h"
#include "../Entidades/Personagens/inimigo.h"
#include "../Entidades/Personagens/arqueiro.h"
#include "../Entidades/Personagens/gigante.h"
#include "../Entidades/Obstaculos/obstaculo.h"
#include "../Entidades/Obstaculos/espinho.h"
#include "../Entidades/Obstaculos/coracao.h"
#include "../Entidades/Obstaculos/caixa.h"
#include "../Entidades/Obstaculos/neve.h"
#include "../Entidades/Obstaculos/musgo.h"
#include <math.h>
#include <algorithm>
#include <chrono>
#include "../Fisica/aabb.h"
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

    void Gerenciador_Colisoes::construir_grade() {
        grade.clear(); ordem_obstaculos.clear(); indices_obstaculos.clear();
        if (obstaculos) for (auto it=obstaculos->get_primeiro(); it!=nullptr; ++it) {
            const auto r=(*it)->get_corpo()->getGlobalBounds();
            const auto indice=ordem_obstaculos.size(); ordem_obstaculos.push_back(*it); indices_obstaculos[*it]=indice;
            for(int x=static_cast<int>(std::floor(r.left/100)); x<=static_cast<int>(std::floor((r.left+r.width)/100)); ++x)
                for(int y=static_cast<int>(std::floor(r.top/100)); y<=static_cast<int>(std::floor((r.top+r.height)/100)); ++y)
                    grade[{x,y}].push_back(indice);
        }
        grade_suja=false;
    }
    std::vector<Entidades::Entidade*> Gerenciador_Colisoes::candidatos(Entidades::Entidade* entidade) {
        if(grade_suja) construir_grade();
        medidas.pares_teoricos += ordem_obstaculos.size();
        if(!usar_grade) { medidas.testes_obstaculos += ordem_obstaculos.size(); return ordem_obstaculos; }
        const auto r=entidade->get_corpo()->getGlobalBounds();
        std::vector<std::size_t> indices;
        for(int x=static_cast<int>(std::floor((r.left-.1f)/100)); x<=static_cast<int>(std::floor((r.left+r.width+.1f)/100)); ++x)
            for(int y=static_cast<int>(std::floor((r.top-.1f)/100)); y<=static_cast<int>(std::floor((r.top+r.height+.1f)/100)); ++y) {
                auto celula=grade.find({x,y});
                if(celula!=grade.end()) indices.insert(indices.end(),celula->second.begin(),celula->second.end());
            }
        std::sort(indices.begin(),indices.end()); indices.erase(std::unique(indices.begin(),indices.end()),indices.end());
        std::vector<Entidades::Entidade*> resultado;
        for(auto i:indices) resultado.push_back(ordem_obstaculos[i]);
        medidas.testes_obstaculos += resultado.size();
        return resultado;
    }
    std::vector<Entidades::Entidade*> Gerenciador_Colisoes::candidatos_apos(Entidades::Entidade* movel, Entidades::Entidade* ultimo) {
        auto proximos=candidatos(movel);
        const auto limite=indices_obstaculos.at(ultimo);
        proximos.erase(std::remove_if(proximos.begin(),proximos.end(),[&](auto* e) {
            return indices_obstaculos.at(e)<=limite;
        }),proximos.end());
        return proximos;
    }
    void Gerenciador_Colisoes::colisao_jogadores_obstaculos() {
        if(!jogadores || !obstaculos) return;
        for(auto jog=jogadores->get_primeiro(); jog!=nullptr; ++jog) {
            auto* jogador=static_cast<Entidades::Personagens::Jogador*>(*jog);
            jogador->preparar_superficie();
            bool superficie_aplicada=false;
            bool apoio_definido=false;
            auto pendentes=candidatos(*jog);
            for(std::size_t i=0;i<pendentes.size();) {
                auto* obst=pendentes[i++];
                const auto antes=(*jog)->getPosicao();
                const int lado=colidiu(*jog,obst);
                if(!lado) continue;
                if(lado==4 && !apoio_definido) {
                    jogador->pousar(dynamic_cast<Entidades::Obstaculos::Neve*>(obst)!=nullptr);
                    apoio_definido=true;
                }
                const bool superficie=dynamic_cast<Entidades::Obstaculos::Neve*>(obst) || dynamic_cast<Entidades::Obstaculos::Musgo*>(obst);
                if(!superficie || (lado==4 && !superficie_aplicada)) {
                    obst->colidir(*jog,lado);
                    if(superficie) superficie_aplicada=true;
                }
                (*jog)->colidir(obst,lado);
                // Uma separacao pode entrar em outra celula: reconsultar apenas itens
                // posteriores preserva exatamente a ordem da varredura completa.
                if(usar_grade && antes!=(*jog)->getPosicao()) { pendentes=candidatos_apos(*jog,obst); i=0; }
            }
        }
    }
    void Gerenciador_Colisoes::colisao_inimigos_obstaculos() {
        if(!inimigos || !obstaculos) return;
        for(auto inim=inimigos->get_primeiro(); inim!=nullptr; ++inim) {
            auto pendentes=candidatos(*inim);
            for(std::size_t i=0;i<pendentes.size();) {
                auto* obst=pendentes[i++];
                const auto antes=(*inim)->getPosicao();
                const int lado=colidiu(*inim,obst);
                if(lado==1 || lado==3) static_cast<Entidades::Personagens::Inimigo*>(*inim)->mudar_direcao();
                if(usar_grade && antes!=(*inim)->getPosicao()) { pendentes=candidatos_apos(*inim,obst); i=0; }
            }
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
                    const bool descendo=(*jog)->getVelocidade().y>0.f;
                    int j = colidiu(*jog, *inim);
                    if(j==4 && descendo) {
                        auto* jogador=static_cast<Entidades::Personagens::Jogador*>(*jog);
                        const int base=dynamic_cast<Entidades::Personagens::Gigante*>(*inim)?10:20;
                        (*inim)->receber_dano(jogador->consumir_dano_queda(base));
                        auto impulso=jogador->getVelocidade(); impulso.y=-3.f;
                        jogador->setVelocidade(impulso); jogador->set_nochao(false);
                    } else if(j && j!=4) (*inim)->colidir(*jog,j);
                    else if(j==4) static_cast<Entidades::Personagens::Jogador*>(*jog)->pousar(false);
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
                for (auto* obst : candidatos(&flecha)) {
                    if (obst->get_vivo() && obst->get_corpo()->getGlobalBounds().intersects(
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
        const auto inicio=std::chrono::steady_clock::now();
        medidas = {};
        colisao_jogadores_obstaculos();
        colisao_jogadores_inimigos();
        colisao_inimigos_obstaculos();
        colisao_jogadores_projeteis();
        colisao_obstaculos_projeteis();
        get_inimigos_vivos();
        get_jogadores_vivos();
        medidas.microssegundos=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-inicio).count();
    }

    int Gerenciador_Colisoes::colidiu(Entidades::Entidade* e1, Entidades::Entidade* e2) {
        if(!e1 || !e2 || !e1->get_vivo() || !e2->get_vivo()) return 0;
        const auto a=e1->get_corpo()->getGlobalBounds(), b=e2->get_corpo()->getGlobalBounds();
        auto v=e1->getVelocidade();
        const auto contato=Fisica::resolver({a.left,a.top,a.width,a.height},{b.left,b.top,b.width,b.height},v.y);
        if(!contato.lado) return 0;
        e1->setPosicao(e1->getPosicao()+sf::Vector2f(contato.dx,contato.dy));
        if(contato.lado==4) { e1->set_nochao(true); if(v.y>0) v.y=0; }
        else if(contato.lado==2) { if(v.y<0) v.y=0; }
        else { v.x=0; }
        e1->setVelocidade(v);
        return contato.lado;
    }
}
