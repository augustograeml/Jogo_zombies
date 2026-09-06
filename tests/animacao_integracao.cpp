#include "../Entidades/Personagens/jogador.h"
#include "../Persistencia/entidades.h"
#include "../Recursos/catalogo.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
namespace fs = std::filesystem;

int main() {
    const auto exigir = [](bool ok, const char* mensagem) { if (!ok) throw std::runtime_error(mensagem); };
    try {
        Entidades::Personagens::Jogador jogador({80, 50}, {0, 0}, false);
        jogador.mover_com_controles(false, true, false, false);
        const auto salvo = Persistencia::Serializador::salvar(jogador);
        auto retomado = Persistencia::Serializador::carregar(salvo);
        exigir(Persistencia::Serializador::salvar(*retomado) == salvo, "Quadro parcial nao sobreviveu a carga");
        // Desenhar repetidamente equivale a atualizar a tela sem avancar a simulacao.
        for (unsigned desenho = 0; desenho < 30; ++desenho) jogador.desenhar();
        exigir(Persistencia::Serializador::salvar(jogador) == salvo, "Desenhar alterou estado persistido");
        const auto caixa = jogador.get_corpo()->getGlobalBounds();
        auto visual = jogador.get_animacao(); visual.direita = !visual.direita;
        jogador.restaurar_animacao(visual); jogador.desenhar();
        const auto depois = jogador.get_corpo()->getGlobalBounds();
        exigir(caixa == depois, "Virar sprite alterou a caixa fisica");
        Entidades::Personagens::Jogador luigi({0, 0}, {0, 0}, true);
        exigir(luigi.get_corpo()->getTexture() == Recursos::textura("imagens/luigiDireita.png").get(), "Arte de Luigi foi substituida");
        luigi.mover_com_controles(true, false, false, false);
        exigir(!luigi.get_animacao().direita, "Jogador 2 nao atualizou direcao");
        luigi.desenhar();
        bool faltando = false;
        try { Recursos::textura("imagens/__recurso_inexistente_teste__.png"); }
        catch (const std::runtime_error& erro) { faltando = std::string(erro.what()).find("__recurso_inexistente_teste__") != std::string::npos; }
        exigir(faltando, "Imagem ausente nao identificou o arquivo");
        const auto pasta = fs::temp_directory_path() / ("zombies-mapa-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(pasta);
        { std::ofstream arquivo(pasta / "bom.txt", std::ios::binary); arquivo << "0 1\r\n789\r\n"; }
        const auto mapa = Recursos::validar_mapa((pasta / "bom.txt").string());
        exigir(mapa.size() == 2 && mapa[0] == "0 1" && mapa[1] == "789", "Mapa CRLF ou espacos alterados");
        { std::ofstream arquivo(pasta / "ruim.txt"); arquivo << "0@\n"; }
        bool invalido = false;
        try { Recursos::validar_mapa((pasta / "ruim.txt").string()); }
        catch (const std::runtime_error& erro) { invalido = std::string(erro.what()).find("linha 1, coluna 2") != std::string::npos; }
        exigir(invalido, "Mapa invalido nao identificou linha e coluna");
        fs::remove_all(pasta);
        std::cout << "Animacao integrada: snapshot, desenho sem avancar, caixa independente, Luigi e erros de recursos OK.\n";
    } catch (const std::exception& erro) { std::cerr << erro.what() << '\n'; return 1; }
}
