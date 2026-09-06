#include "../Listas/listaEntidades.h"
 
 namespace Listas
 {
    //atribuicao pras lista estaticas
    ListaEntidade* ListaEntidade::instancia_lista_inimigos = nullptr;
    ListaEntidade* ListaEntidade::instancia_lista_obstaculos = nullptr;

    ListaEntidade::ListaEntidade():
    LEs()
    {

    }
    ListaEntidade::~ListaEntidade()
    {

    }
    void ListaEntidade::incluir(Entidades::Entidade* pE)
    {
        LEs.incluir(pE);
    }
    void ListaEntidade::remover(Entidades::Entidade* pE)
    {
        LEs.remover(pE);
    }

    int ListaEntidade::get_tamanho() const
    {
        return LEs.get_tamanho();
    }
    void ListaEntidade::desenhar()
    {
        Lista<Entidades::Entidade>::Iterador aux = LEs.get_primeiro();
        while (aux != nullptr)
        {
            if((*aux)->get_vivo())
            (*aux)->desenhar();
            aux++;
        }
    }
    
    void ListaEntidade::desenhar(const sf::FloatRect& area)
    {
        for (auto it = LEs.get_primeiro(); it != nullptr; ++it)
            if ((*it)->get_vivo() && area.intersects((*it)->get_corpo()->getGlobalBounds())) (*it)->desenhar();
    }
    void ListaEntidade::executar()
    {
        Lista<Entidades::Entidade>::Iterador aux = LEs.get_primeiro();
        while (aux != nullptr)
        {
            (*aux)->executar();
            aux++;
        }        
    }

    Lista<Entidades::Entidade>::Iterador ListaEntidade::get_primeiro()
    {
        return LEs.get_primeiro();
    }

    ListaEntidade* ListaEntidade::get_instancia_inimigos()
    {
        if(!instancia_lista_inimigos)
            instancia_lista_inimigos = new ListaEntidade();
        
        return instancia_lista_inimigos;
    }
    ListaEntidade* ListaEntidade::get_instancia_obstaculos()
    {
        if(!instancia_lista_obstaculos)
            instancia_lista_obstaculos = new ListaEntidade();
        
        return instancia_lista_obstaculos;
    }
}
