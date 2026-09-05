//codigo reaproveitado do Peteco

#pragma once
#include <utility>

namespace Listas
{
    template<class TL>
    class Lista
    {
        // Classe aninhada privada
        template <class TE>
        class Elemento
        {
        private:
            TE* pinfo;
            Elemento<TE>* pProx;
        public:
            Elemento():
            pinfo(nullptr),
            pProx(nullptr)
            {

            }
            ~Elemento()
            {
                if (pinfo)
                    delete pinfo;
                pProx = nullptr;
            }
            TE* get_pinfo() { return pinfo; }
            Elemento<TE>* get_pProx() { return pProx; }

            void set_pinfo(TE* pi) { if (pi) { pinfo = pi; } }
            void set_pProx(Elemento<TE>* pP) { pProx = pP; }
        };
        private:
            Elemento<TL>* pPrimeiro;
            int tamanho;
        public:
            Lista():
            pPrimeiro(nullptr),
            tamanho(0)
            {

            }
            ~Lista()
            {
                limpar();
            }
            Lista(const Lista&) = delete;
            Lista& operator=(const Lista&) = delete;
            void trocar(Lista& outra) noexcept {
                std::swap(pPrimeiro, outra.pPrimeiro);
                std::swap(tamanho, outra.tamanho);
            }
            void limpar()
            {
                Elemento<TL>* aux = nullptr;
                while(pPrimeiro)
                {
                    aux = pPrimeiro;
                    pPrimeiro = pPrimeiro->get_pProx();
                    if (aux)
                        delete aux;
                }
                tamanho = 0;
                pPrimeiro = nullptr;
            }
            int get_tamanho() const
            {
                return tamanho;
            }
           void incluir(TL* elem)
            {
                if (!elem)
                    return;
                Elemento<TL>* aux = new Elemento<TL>();
                if (aux)
                {
                    aux->set_pinfo(elem);
                    aux->set_pProx(pPrimeiro);
                    pPrimeiro = aux;
                    tamanho++;
                }
            }
            
            /*void remover(TL *elem)
            {
                Elemento<TL>* aux = pPrimeiro;
                Elemento<TL>* anterior = nullptr;

                while(aux != nullptr)
                {
                    if(aux->get_pinfo() == elem)
                    {
                        if(elem == pPrimeiro->get_pinfo())
                        {
                        pPrimeiro = aux->get_pProx();
                        }
                        else 
                        {
                            anterior->set_pProx(aux->get_pProx());
                        }
                        tamanho--;
                        return;
                    }
                    anterior = aux;
                    aux = aux->get_pProx();
                }
                
            }*/
            void remover(TL *elem) 
            {
                    Elemento<TL>* aux = pPrimeiro;
                    Elemento<TL>* anterior = nullptr;

                    while (aux != nullptr) {
                        if (aux->get_pinfo() == elem) {
                            if (elem == pPrimeiro->get_pinfo()) {
                                pPrimeiro = aux->get_pProx();
                                aux->set_pProx(nullptr);
                            } else {
                                anterior->set_pProx(aux->get_pProx());
                            }
                            
                            delete aux;
                            tamanho--;
                            return;
                        }
                        anterior = aux;
                        aux = aux->get_pProx();
                    }
                }
            // Classe aninhada pública:
            class Iterador
            {
            private:
                Elemento<TL>* atual;
            public:
                Iterador(Elemento<TL>* a = nullptr):
                atual(a)
                {

                }
                ~Iterador()
                {
                    atual = nullptr;
                }
                Iterador& operator++()
                {
                    if (atual) atual = atual->get_pProx();
                    return *this;
                }
                Iterador operator++(int)
                {
                    Iterador anterior = *this;
                    ++(*this);
                    return anterior;
                }
                bool operator==(const Elemento<TL>* outro) const
                {
                    return atual == outro;
                }
                bool operator!=(const Elemento<TL>* outro) const
                {
                    return !(atual == outro);
                }
                void operator=(const Elemento<TL>* outro)
                {
                    atual = outro;
                }
                TL* operator*()
                {
                    return atual->get_pinfo();
                }
                const Elemento<TL>* get_atual() const
                {
                    return atual;
                }
            };
            Iterador get_primeiro()
            {
                return Iterador(pPrimeiro);
            }
    };
    
}
