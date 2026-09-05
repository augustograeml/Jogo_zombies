//codigo reaproveitado do Peteco

#pragma once

#include "lista.h"
#include "../Entidades/entidade.h"

 // Coleção
namespace Listas
{
    class ListaEntidade
    {
        private:
            Lista<Entidades::Entidade> LEs;
            static ListaEntidade* instancia_lista_inimigos;
            static ListaEntidade* instancia_lista_obstaculos;
        public:
            ListaEntidade();
            ~ListaEntidade();
            void limpar() { LEs.limpar(); }
            void trocar(ListaEntidade& outra) noexcept { LEs.trocar(outra.LEs); }
            void incluir(Entidades::Entidade* pE);
            void remover(Entidades::Entidade *pE);
            static ListaEntidade* get_instancia_inimigos();
            static ListaEntidade* get_instancia_obstaculos();
            int get_tamanho() const;
            void desenhar();
            void executar();
            Lista<Entidades::Entidade>::Iterador get_primeiro();
        };
}
