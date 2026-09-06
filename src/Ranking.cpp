#include "../Persistencia/pontos.h"
#include "../Recursos/catalogo.h"
#include "../Estados/Menus/ranking.h"
#include "../Persistencia/ranking.h"
#include <iomanip>
#include <sstream>
namespace Estados::Menus {
Ranking::Ranking(int id) : Estado(id) {
    fonte.loadFromFile(Recursos::caminho("Design/fonte/fonte_simas.ttf").string());
    imagem.loadFromFile(Recursos::caminho("Design/imagens/rankingzombies++.png").string());
}
void Ranking::atualizar() {
    textos.clear();
    auto texto = [this](const std::string& valor, float y, unsigned tamanho = 25) {
        sf::Text t(sf::String::fromUtf8(valor.begin(), valor.end()), fonte, tamanho);
        t.setPosition(90, y); t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(2);
        const float largura = t.getLocalBounds().width;
        if (largura > 840) t.setScale(840 / largura, 840 / largura);
        textos.push_back(t);
    };
    texto("Ranking - Fase " + std::to_string(fase) + (jogadores == 1 ? " - Solo" : " - Dupla"), 120, 32);
    texto(por_pontos ? "Maior pontuacao vence; empate pelo tempo" : "Menor tempo vence", 180, 20);
    texto("Setas: fase | Tab: solo/dupla | P: tempo/pontos", 215, 20);
    try {
        std::vector<Persistencia::ResultadoPontos> resultados;
        if (por_pontos) resultados=Persistencia::RepositorioPontos().consultar(fase,jogadores);
        else for (const auto& r : Persistencia::RepositorioRanking().consultar(fase,jogadores))
            resultados.push_back({r.id,r.fase,r.jogadores,r.nomes,0,r.segundos,true});
        if (resultados.empty()) texto("Nenhum resultado registrado.", 280);
        for (std::size_t i = 0; i < resultados.size() && i < 8; ++i) {
            const auto& r = resultados[i];
            std::ostringstream linha;
            linha << i + 1 << ". ";
            for (std::size_t n = 0; n < r.nomes.size(); ++n) {
                if (n) linha << " + ";
                linha << r.nomes[n];
            }
            if(por_pontos) linha << " - " << r.pontos << " pts" << (r.vitoria?" (concluida)":"");
            linha << "  -  " << std::fixed << std::setprecision(3) << r.segundos << " s";
            texto(linha.str(), 280 + 58 * i, 23);
        }
    } catch (const std::exception& erro) { pGE->mensagem = std::string("Erro no ranking: ") + erro.what(); }
    texto("Enter ou Esc: voltar", 820, 25);
}
void Ranking::ao_entrar() { atualizar(); }
void Ranking::tratar_evento(const sf::Event& evento) {
    if (evento.type != sf::Event::KeyPressed) return;
    if (evento.key.code == sf::Keyboard::Escape || evento.key.code == sf::Keyboard::Enter) pGE->set_estado_atual(Estados::Tela::Principal);
    else if (evento.key.code == sf::Keyboard::P) { por_pontos=!por_pontos; atualizar(); }
    else if (evento.key.code == sf::Keyboard::Left || evento.key.code == sf::Keyboard::Right) {
        fase = fase == 1 ? 2 : 1; atualizar();
    } else if (evento.key.code == sf::Keyboard::Tab) { jogadores = jogadores == 1 ? 2 : 1; atualizar(); }
}
void Ranking::executar() {
    auto* grafico = Gerenciadores::Gerenciador_Grafico::get_instancia();
    grafico->resetarCamera(); grafico->desenharTextura(&imagem);
    sf::RectangleShape painel({870, 500});
    painel.setPosition(75, 260); painel.setFillColor(sf::Color(12, 8, 22, 210));
    grafico->get_Janela()->draw(painel);
    for (const auto& texto : textos) grafico->get_Janela()->draw(texto);
}
}
