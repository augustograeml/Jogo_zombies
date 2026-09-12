#include "../Persistencia/pontos.h"
#include "../Interface/preferencias.h"
#include "../Interface/tema.h"
#include "../Recursos/catalogo.h"
#include "../Estados/Menus/ranking.h"
#include "../Persistencia/ranking.h"
#include <iomanip>
#include <sstream>
namespace Estados::Menus {
Ranking::Ranking(int id) : Estado(id) {
    fonte.loadFromFile(Recursos::caminho("Design/fonte/BarlowCondensed-Medium.ttf").string());
    imagem.loadFromFile(Recursos::caminho("Design/imagens/rankingzombies++.png").string());
}
void Ranking::atualizar() {
    textos.clear();
    auto texto = [this](const std::string& valor, float y, unsigned tamanho = 25) {
        sf::Text t(sf::String::fromUtf8(valor.begin(), valor.end()), Interface::fonte_interface(), tamanho);
        t.setPosition(90, y); t.setFillColor(Interface::Tema::tinta());
        if(y==120) Interface::Tema::estilo_titulo(t);
        else if(y==180 || y==215 || y==820) t.setFillColor(Interface::Tema::secundaria());
        const float largura = t.getLocalBounds().width;
        if (largura > 840) t.setScale(840 / largura, 840 / largura);
        textos.push_back(t);
    };
    texto("RANKING / FASE " + std::to_string(fase) + (jogadores == 1 ? " / SOLO" : " / DUPLA"), 120, 48);
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
    if(Interface::Preferencias::instancia().contraste) {
        sf::RectangleShape fundo({1024,1024}); fundo.setFillColor(sf::Color::Black); grafico->get_Janela()->draw(fundo);
    }
    Interface::Tema::painel(*grafico->get_Janela(),{55,85,914,805});
    for (auto texto : textos) { Interface::aplicar_texto(texto,840); grafico->get_Janela()->draw(texto); }
}
}
