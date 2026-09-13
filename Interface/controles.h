#pragma once
#include "../Logica/entrada.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <string>
namespace Interface {
struct ConfiguracaoControles {
    std::array<std::array<int,4>,2> teclas{{{{sf::Keyboard::A,sf::Keyboard::D,sf::Keyboard::W,sf::Keyboard::S}},
        {{sf::Keyboard::Left,sf::Keyboard::Right,sf::Keyboard::Up,sf::Keyboard::Down}}}};
    std::array<unsigned,2> botoes{{0,0}};
    std::array<bool,2> personalizado{{false,false}};
    static ConfiguracaoControles& instancia();
    void validar() const;
    void salvar() const;
    static ConfiguracaoControles carregar();
    void atribuir(unsigned jogador,unsigned acao,int tecla);
};
Logica::Comandos ler_controles(bool solo);
// Tambem usada nos testes, sem ler hardware.
Logica::Controle combinar_controle(Logica::Controle teclado,float eixo_x,float eixo_y,bool pulo);
class MenuControles {
    unsigned opcao=0;
    bool aguardando=false;
    std::string mensagem;
public:
    bool tratar(const sf::Event& evento); // false: voltar a pausa.
    void desenhar(sf::RenderWindow& janela) const;
};
}
