//codigo reaproveitado do Peteco

#pragma once
#include "../Listas/listaEntidades.h"
#include "../Entidades/entidade.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <cstdint>
using namespace std;

namespace Gerenciadores
{
    class Gerenciador_Colisoes
    {
        private:
            Listas::ListaEntidade* obstaculos;
            Listas::ListaEntidade* jogadores;
            Listas::ListaEntidade* inimigos;
            Listas::ListaEntidade* zumbis;
            Listas::ListaEntidade* atiradores;
            Listas::ListaEntidade* gigantes;
            Listas::ListaEntidade* projeteis;

            bool grade_suja = true;
            bool usar_grade = true;
            std::vector<Entidades::Entidade*> ordem_obstaculos;
            std::unordered_map<Entidades::Entidade*, std::size_t> indices_obstaculos;
            std::map<std::pair<int,int>, std::vector<std::size_t>> grade;
            void construir_grade();
            std::vector<Entidades::Entidade*> candidatos(Entidades::Entidade* entidade);
            std::vector<Entidades::Entidade*> candidatos_apos(Entidades::Entidade*, Entidades::Entidade*);
            bool sem_inimigos;
            bool sem_jogadores;
        public:
            struct Estatisticas { std::uint64_t pares_teoricos=0, testes_obstaculos=0; double microssegundos=0; };
            Estatisticas medidas;
            const Estatisticas& estatisticas() const { return medidas; }
            void ativar_grade(bool ativa) { usar_grade = ativa; }
            void invalidar_grade() { grade_suja = true; }
            Gerenciador_Colisoes();
            ~Gerenciador_Colisoes();

            void set_obstaculos(Listas::ListaEntidade* obs) {if(obs) { obstaculos = obs; invalidar_grade(); }}
            void set_jogadores(Listas::ListaEntidade* jog) {if(jog) jogadores = jog;}
            void set_inimigos(Listas::ListaEntidade* ini) {if(ini) inimigos = ini;}
            void set_zumbis(Listas::ListaEntidade* z) {if(z) zumbis = z;}
            void set_atiradores(Listas::ListaEntidade* a) {if(a) atiradores = a;}
            void set_gigantes(Listas::ListaEntidade* g) {if(g) gigantes = g;}
            void set_projeteis(Listas::ListaEntidade* proj) {if(proj) projeteis = proj;}

            void colisao_jogadores_obstaculos();
            void colisao_jogadores_inimigos();
            void colisao_inimigos_obstaculos();
            void colisao_jogadores_projeteis();
            void colisao_obstaculos_projeteis();
            void gerenciar_colisoes();
            
            int colidiu(Entidades::Entidade* e1, Entidades::Entidade* e2);

            bool get_inimigos_vivos();
            bool get_jogadores_vivos();
    };
}