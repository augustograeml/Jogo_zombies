#include "../Recursos/catalogo.h"
#include "../Estados/Menus/menu_jogadores.h"

namespace Estados
{
    namespace Menus
    {

        Menu_Jogadores::Menu_Jogadores(int id) : Menu(id)
        {
            pObserver = new Observers::Observer_Menu_Jogadores();
            pObserver->set_menu(this);

            inicializa_valores();
        }

        Menu_Jogadores::~Menu_Jogadores()
        {
        }

        bool Menu_Jogadores::get_jogador2()
        {
            return jogador2;
        }

        void Menu_Jogadores::inicializa_valores()
        {
            selecionado = deselecionado = jogador2 = false;
            imagem->loadFromFile(Recursos::caminho("Design/imagens/menu2jogadores.png").string());
            fonte->loadFromFile(Recursos::caminho("Design/fonte/Teko-Bold.otf").string());

            opcoes = {"Players", "Um jogador", "Dois Jogadores", "Sair"};
            textos.resize(4);
            coordenadas = {{250, 40}, {335, 604}, {564, 604}, {493, 650}};
            tamanhos = {150, 20, 20, 20};

            for (std::size_t i{}; i < textos.size(); i++)
            {
                textos[i].setFont(*fonte);
                textos[i].setString(opcoes[i]);
                textos[i].setCharacterSize(tamanhos[i]);
                textos[i].setFillColor(sf::Color::White);
                textos[i].setOutlineColor(sf::Color::Black);
                textos[i].setPosition(coordenadas[i]);
            }

            textos[0].setFillColor(sf::Color::White);
            textos[0].setOutlineThickness(20);

            textos[1].setOutlineThickness(4);

            pos = 1;
        }

        void Menu_Jogadores::selecionar()
        {
            if(!deselecionado)
            {
                if (pos == 3)
                    pGE->set_estado_atual(Estados::Tela::Principal);
                else if (pos == 1)
                    pGE->set_estado_atual(Estados::Tela::FasesSolo);
                else if (pos == 2)
                    pGE->set_estado_atual(Estados::Tela::FasesDupla);
            }
        }

        void Menu_Jogadores::loop_evento() {}
    }
}
