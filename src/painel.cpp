#include "../Interface/painel.h"
#include "../Persistencia/ranking.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace Interface {
std::string formatar_tempo(double segundos) {
    const auto centesimos = static_cast<std::uint64_t>(std::llround(segundos * 100));
    std::ostringstream texto;
    texto << std::setfill('0') << std::setw(2) << centesimos / 6000 << ':'
          << std::setw(2) << centesimos / 100 % 60 << '.' << std::setw(2) << centesimos % 100;
    return texto.str();
}
PainelPartida::PainelPartida() {
    if (!fonte.loadFromFile("Design/fonte/fonte_simas.ttf"))
        throw std::runtime_error("Fonte do painel nao encontrada.");
}
void PainelPartida::atualizar_recorde(int numero_fase, int quantidade) {
    fase = numero_fase; jogadores = quantidade;
    recorde.reset(); titulares.clear(); erro_recorde = false;
    try {
        const auto lista = Persistencia::RepositorioRanking().consultar(fase, jogadores);
        if (!lista.empty()) {
            recorde = lista.front().segundos;
            for (const auto& nome : lista.front().nomes) {
                if (!titulares.empty()) titulares += " + ";
                titulares += nome;
            }
        }
    } catch (const std::exception&) { erro_recorde = true; }
}
void PainelPartida::desenhar(sf::RenderWindow& janela, double segundos) const {
    const auto camera = janela.getView();
    janela.setView(janela.getDefaultView());
    const float largura = janela.getView().getSize().x;
    const float divisao1 = largura * 0.22f, divisao2 = largura * 0.55f;
    sf::RectangleShape fundo({largura, 100});
    fundo.setFillColor(sf::Color(8, 15, 23, 232)); janela.draw(fundo);
    sf::RectangleShape detalhe({largura, 3}); detalhe.setPosition(0, 98);
    detalhe.setFillColor(sf::Color(66, 218, 181)); janela.draw(detalhe);
    auto texto = [&](const std::string& valor, float x, float y, unsigned tamanho,
                     sf::Color cor, float limite) {
        sf::Text t(sf::String::fromUtf8(valor.begin(), valor.end()), fonte, tamanho);
        t.setPosition(x, y); t.setFillColor(cor);
        const float w = t.getLocalBounds().width;
        if (w > limite) t.setScale(limite / w, limite / w);
        janela.draw(t);
    };
    const sf::Color legenda(173, 190, 203), verde(107, 243, 211), ouro(255, 215, 122);
    texto("FASE " + std::to_string(fase), 20, 14, 20, sf::Color::White, divisao1 - 35);
    texto(jogadores == 1 ? "SOLO" : "DUPLA", 20, 57, 16, legenda, divisao1 - 35);
    texto("TEMPO DA PARTIDA", divisao1, 13, 14, legenda, divisao2 - divisao1 - 20);
    texto(formatar_tempo(segundos), divisao1, 43, 28, verde, divisao2 - divisao1 - 20);
    texto("RECORDE DA FASE", divisao2, 13, 14, legenda, largura - divisao2 - 20);
    texto(recorde ? formatar_tempo(*recorde) : (erro_recorde ? "Indisponivel" : "Sem recorde"),
          divisao2, 37, 23, ouro, largura - divisao2 - 20);
    if (recorde) texto(titulares, divisao2, 73, 12, legenda, largura - divisao2 - 20);
    janela.setView(camera);
}
}
