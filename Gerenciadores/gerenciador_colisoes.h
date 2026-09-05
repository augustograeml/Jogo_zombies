//codigo reaproveitado do Peteco

#pragma once
#include "../Listas/listaEntidades.h"
#include "../Entidades/entidade.h"
#include <iostream>
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

            bool sem_inimigos;
            bool sem_jogadores;
        public:
            Gerenciador_Colisoes();
            ~Gerenciador_Colisoes();

            void set_obstaculos(Listas::ListaEntidade* obs) {if(obs) obstaculos = obs;}
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