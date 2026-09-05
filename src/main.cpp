#include "../jogo.h"
#include <iostream>
int main() {
    try {
        Jogo jogo;
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Nao foi possivel executar Zombies++: " << erro.what() << '\n';
        return 1;
    }
}
