#pragma once
#include "menu.h"
namespace Estados::Menus {
class Nome : public Menu {
    sf::String entrada;
    std::vector<std::string> nomes;
    int quantidade = 1;
public:
    explicit Nome(int id);
    void inicializa_valores() override;
    void loop_evento() override {}
    void selecionar() override;
    void ao_entrar() override;
    void tratar_evento(const sf::Event& evento) override;
    void executar() override;
};
}
