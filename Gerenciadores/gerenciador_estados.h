//codigo reaproveitado do peteco

#pragma once
#include <vector>
#include "../Estados/identificador.h"
#include <string>

using namespace std;

namespace Estados
{
    class Estado;
}



namespace Gerenciadores
{
    class Gerenciador_Estados
    {
        private:
            int estadoAtual, fase;
            vector<Estados::Estado*> estados;

            Gerenciador_Estados();
        public:
            ~Gerenciador_Estados();
            void encerrar();
            Gerenciador_Estados(const Gerenciador_Estados&) = delete;
            Gerenciador_Estados& operator=(const Gerenciador_Estados&) = delete;

            static Gerenciador_Estados* get_instancia();
            void set_estado_atual(int eA);
            void set_estado_atual(Estados::Tela tela) { set_estado_atual(Estados::codigo(tela)); }
            void set_fase(int f);

            void deleta_estados(int i);

            int get_estado_atual();
            int get_fase();
            void adicionar_estado(Estados::Estado* pE);
            void executar();
            Estados::Estado* get_estado(int id);
            bool salvar_partida();
            std::string mensagem;

    };

}
