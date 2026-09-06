#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Estados/Menus/menu_principal.h"
#include "../Estados/Menus/menu_jogadores.h"
#include "../Estados/Menus/menu_fase.h"
#include "../Estados/Menus/pause.h"
#include "../Estados/Menus/nome.h"
#include "../Estados/Menus/ranking.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/ranking.h"
#include "../Gerenciadores/gerenciador_eventos.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

using Persistencia::Json;
using Estados::Fases::Fase;
using Estados::Fases::Fase1;
using Estados::Fases::Fase2;
namespace fs = std::filesystem;
namespace {
int verificacoes = 0;
void exigir(bool condicao, const std::string& descricao) {
    ++verificacoes;
    if (!condicao) throw std::runtime_error(descricao);
}
template<class Funcao> void rejeitar(Funcao funcao, const std::string& descricao) {
    bool falhou = false;
    try { funcao(); } catch (const std::exception&) { falhou = true; }
    exigir(falhou, descricao);
}
void preparar_pasta(const fs::path& pasta, const fs::path& recursos) {
    fs::create_directories(pasta);
    if (!fs::exists(pasta / "Design")) {
        std::error_code erro;
        fs::create_directory_symlink(recursos, pasta / "Design", erro);
        if (erro) fs::copy(recursos, pasta / "Design", fs::copy_options::recursive);
    }
    fs::current_path(pasta);
}
void menus() {
    auto* estados = Gerenciadores::Gerenciador_Estados::get_instancia();
    estados->adicionar_estado(new Estados::Menus::Menu_Principal(0));
    estados->adicionar_estado(new Estados::Menus::Menu_Jogadores(1));
    estados->adicionar_estado(new Estados::Menus::Menu_Fase(2, false));
    estados->adicionar_estado(new Estados::Menus::Menu_Fase(3, true));
    estados->adicionar_estado(new Estados::Menus::Ranking(4));
    estados->adicionar_estado(new Estados::Menus::Pause(5));
    estados->adicionar_estado(new Estados::Menus::Nome(10));
}
std::unique_ptr<Fase> criar(int id, bool carregar) {
    if (id < 8) return std::make_unique<Fase1>(id, carregar);
    return std::make_unique<Fase2>(id, carregar);
}
Json cenario_de_teste(Fase& fase) {
    using Persistencia::Serializador;
    using namespace Entidades;
    Json j = fase.capturar();
    j["passos"] = 1234;
    j["acumulador"] = 0.004;
    for (auto& jogador : j["jogadores"]) {
        jogador["posicao"] = {100.f, 100.f};
        jogador["velocidade"] = {0.f, 0.f};
        jogador["vida"] = 17;
        jogador["extra"]["nome"] = jogador["extra"]["jogador2"].get<bool>() ? "Bia" : "Antônio";
        jogador["extra"]["tempo"] = 1234.0 / 60;
    }
    Personagens::Arqueiro arqueiro({650, 100}, {0.3f, -0.2f});
    auto arq = Serializador::salvar(arqueiro);
    arq["extra"]["recarga"] = 17;
    arq["extra"]["direcao"] = true;
    Projetil flecha({200, 110}, false);
    arq["extra"]["projeteis"].push_back(Serializador::salvar(flecha));
    auto morto = arq;
    morto["vivo"] = false;
    morto["extra"]["projeteis"][0]["posicao"] = {400.f, 200.f};
    Personagens::Zumbi zumbi({1500, 300}, {0.1f, 0.f});
    Personagens::Gigante gigante({1800, 300}, {-0.1f, 0.f});
    auto gigante_json = Serializador::salvar(gigante);
    gigante_json["vida"] = 70;
    gigante_json["extra"]["ja_inc"] = true;
    gigante_json["extra"]["direcao"] = true;
    j["inimigos"] = Json::array({arq, morto, Serializador::salvar(zumbi), gigante_json});
    Obstaculos::Coracao coracao({500, 500});
    auto coracao_json = Serializador::salvar(coracao);
    coracao_json["vivo"] = false;
    coracao_json["extra"]["cura"] = 5;
    Obstaculos::Neve neve({1500, 350});
    Obstaculos::Musgo musgo({1800, 350});
    Obstaculos::Caixa caixa({1000, 500});
    Obstaculos::Espinho espinho({1200, 500});
    j["obstaculos"] = Json::array({coracao_json, Serializador::salvar(neve), Serializador::salvar(musgo),
                                   Serializador::salvar(caixa), Serializador::salvar(espinho)});
    return j;
}
void persistencia() {
    for (int id = 6; id <= 9; ++id) {
        auto original = criar(id, false);
        exigir(original->capturar()["jogadores"].size() == (id % 2 ? 2u : 1u), "Numero de jogadores na construcao");
        const auto inicial = cenario_de_teste(*original);
        original->restaurar(inicial);
        original->salvar("teste-partida.json");
        auto carregada = criar(id, true);
        carregada->restaurar(Persistencia::ler_json("teste-partida.json"));
        exigir(carregada->capturar() == inicial, "Todos os estados devem sobreviver ao salvamento");
        original->simular_passo(); carregada->simular_passo();
        auto primeiro = carregada->capturar();
        exigir(primeiro == original->capturar(), "Primeiro passo depois do carregamento");
        exigir(primeiro["inimigos"][0]["extra"]["projeteis"][0]["posicao"][0] == 188.f,
               "Flecha deve continuar para a esquerda");
        exigir(primeiro["inimigos"][1]["extra"]["projeteis"][0]["posicao"][0] == 388.f,
               "Flecha de arqueiro morto tambem continua");
        for (int passo = 0; passo < 79; ++passo) { original->simular_passo(); carregada->simular_passo(); }
        const auto depois = carregada->capturar();
        exigir(depois == original->capturar(), "80 passos identicos incluindo IA, recarga e colisoes");
        exigir(depois["jogadores"][0]["vida"].get<int>() < 17, "Flecha carregada ainda causa dano");
        exigir(depois["obstaculos"][0]["vivo"] == false, "Coracao consumido nao reaparece");
        const auto antes_erro = carregada->capturar();
        auto invalido = inicial; invalido["inimigos"][0]["extra"]["recarga"] = -1;
        rejeitar([&] { carregada->restaurar(invalido); }, "Recarga negativa deve ser rejeitada");
        exigir(carregada->capturar() == antes_erro, "Falha de carregamento nao altera a partida existente");
        invalido = inicial; invalido["versao"] = 99;
        rejeitar([&] { carregada->restaurar(invalido); }, "Versao desconhecida");
        invalido = inicial; invalido["jogadores"][0]["velocidade"] = {"rapido", 0};
        rejeitar([&] { carregada->restaurar(invalido); }, "Velocidade corrompida");
        invalido = inicial; invalido["aleatorio"] = "quebrado";
        rejeitar([&] { carregada->restaurar(invalido); }, "Estado aleatorio invalido");
        invalido = inicial; invalido["inimigos"][0]["extra"]["projeteis"][0]["tipo"] = "arqueiro";
        rejeitar([&] { carregada->restaurar(invalido); }, "Flechas nao podem conter inimigos recursivos");
    }
    const Json anterior = {{"preservar", 1}};
    Persistencia::escrever_json("atomico.json", anterior);
    fs::create_directory("atomico.json.tmp");
    rejeitar([&] { Persistencia::escrever_json("atomico.json", Json{{"novo", 2}}); }, "Falha de gravacao deve aparecer");
    exigir(Persistencia::ler_json("atomico.json") == anterior, "Falha de gravacao preserva arquivo anterior");
    std::ofstream("corrompido.json") << "{truncado";
    rejeitar([] { Persistencia::ler_json("corrompido.json"); }, "Arquivo truncado");
    rejeitar([] { Persistencia::ler_json("ausente.json"); }, "Arquivo ausente");
}
void ranking() {
    fs::create_directories("legados");
    std::ofstream("legados/rankingfase1.txt") << "Lento.....120s\nRapido.....9s\nErrado.....-1s\nLixo\nNaN.....nans\n";
    Persistencia::RepositorioRanking repositorio("ranking-teste.json", "legados");
    const auto legado = repositorio.consultar(1, 1);
    exigir(legado.size() == 2 && legado[0].segundos == 9, "Importacao valida e ordenacao numerica dos legados");
    const Persistencia::Resultado r{"unico", 1, 1, {"Ana"}, 8.125};
    repositorio.registrar(r); repositorio.registrar(r);
    repositorio.registrar({"empate", 1, 1, {"Bia"}, 8.125});
    repositorio.registrar({"dupla", 1, 2, {"Ana", "Bia"}, 3.5});
    repositorio.registrar({"fase2", 2, 1, {"Caio"}, 2.5});
    Persistencia::RepositorioRanking reaberto("ranking-teste.json", "legados");
    auto lista = reaberto.consultar(1, 1);
    exigir(lista.size() == 4, "Repetir o mesmo resultado nao duplica o ranking");
    exigir(lista[0].id == "unico" && lista[1].id == "empate", "Empates mantem ordem de registro");
    exigir(lista[0].segundos == 8.125, "Ranking preserva fracao de segundo");
    exigir(reaberto.consultar(1, 2).size() == 1 && reaberto.consultar(2, 1).size() == 1,
           "Ranking separa fases e modalidades");
    rejeitar([&] { repositorio.registrar({"ruim", 1, 1, {" "}, 4}); }, "Nome vazio");
    rejeitar([&] { repositorio.registrar({"ruim", 1, 1, {"Ana"}, -4}); }, "Tempo negativo");
    std::ofstream("ranking-corrompido.json") << "quebrado";
    Persistencia::RepositorioRanking corrompido("ranking-corrompido.json", "legados");
    rejeitar([&] { corrompido.registrar(r); }, "Nao sobrescrever ranking corrompido");
    std::ifstream leitura("ranking-corrompido.json"); std::string conteudo; leitura >> conteudo;
    exigir(conteudo == "quebrado", "Ranking corrompido preservado");
}
sf::Event tecla(sf::Keyboard::Key codigo) {
    sf::Event evento{}; evento.type = sf::Event::KeyPressed; evento.key.code = codigo; return evento;
}
void movimento_e_tempo() {
    for (bool segundo : {false, true}) {
        Entidades::Personagens::Jogador jogador({100, 100}, {0, 0}, segundo);
        for (int i = 0; i < 120; ++i) {
            jogador.set_nochao(true);
            jogador.mover_com_controles(false, true, false, false);
        }
        exigir(jogador.getVelocidade().x == 4, "Movimento tem limite de velocidade");
        Entidades::Obstaculos::Neve neve({0, 0});
        for (int i = 0; i < 40; ++i) {
            jogador.set_nochao(true);
            jogador.mover_com_controles(false, false, false, false);
            neve.escorregar(&jogador);
        }
        exigir(jogador.getVelocidade().x > 0, "Gelo conserva deslizamento por mais tempo que piso normal");
        for (int i = 0; i < 120; ++i) {
            jogador.set_nochao(true);
            jogador.mover_com_controles(false, false, false, false);
            neve.escorregar(&jogador);
        }
        exigir(jogador.getVelocidade().x == 0, "Soltar teclas ainda permite parar na neve apos deslizar");
        const auto parado = jogador.getPosicao();
        for (int i = 0; i < 60; ++i) {
            jogador.set_nochao(true);
            jogador.mover_com_controles(false, false, false, false);
        }
        exigir(jogador.getPosicao() == parado, "Jogador permanece parado sem entrada");
        jogador.pousar(false); // Teclas opostas no piso normal; gelo foi testado acima.
        jogador.setVelocidade({-3, 0});
        for (int i = 0; i < 30; ++i) {
            jogador.set_nochao(true);
            jogador.mover_com_controles(true, true, false, false);
        }
        exigir(jogador.getVelocidade().x == 0, "Teclas opostas freiam sem favorecer um lado");
        jogador.set_nochao(true); jogador.mover_com_controles(false, false, true, false);
        exigir(jogador.getVelocidade().y == -6, "Pulo continua funcionando");
    }
    auto fase = criar(6, false);
    exigir(fase->get_tempo_sessao() == 0, "Novo jogo inicia cronometro zerado");
    auto dados = cenario_de_teste(*fase); dados["passos"] = 0;
    fase->restaurar(dados); fase->simular_passo();
    exigir(fase->get_tempo_sessao() == 1.0 / 60, "Cronometro acompanha simulacao");
    fase->continuar_sessao(600);
    auto retomada = criar(6, true); retomada->restaurar(fase->capturar());
    exigir(retomada->get_tempo_sessao() == 601.0 / 60, "Cronometro total e restaurado");
    auto novo = criar(6, false);
    exigir(novo->get_tempo_sessao() == 0, "Outra partida nao herda o tempo anterior");
    auto anterior = fase->capturar(); anterior.erase("passos_anteriores"); anterior.erase("nomes_confirmados");
    retomada->restaurar(anterior);
    exigir(retomada->get_tempo_sessao() == retomada->get_tempo(), "Saves da versao anterior ainda carregam");
    exigir(Interface::formatar_tempo(0) == "00:00.00" && Interface::formatar_tempo(65.5) == "01:05.50",
           "Painel formata minutos, segundos e centesimos");
}
void derrota_com_nome() {
    auto* ge = Gerenciadores::Gerenciador_Estados::get_instancia();
    for (int id : {6, 9}) {
        ge->set_estado_atual(0);
        auto fase = criar(id, false); auto dados = cenario_de_teste(*fase);
        for (auto& jogador : dados["jogadores"]) jogador["vida"] = 0;
        fase->restaurar(dados); ge->adicionar_estado(fase.get()); fase.release(); ge->set_estado_atual(id);
        auto* ativa = static_cast<Fase*>(ge->get_estado(id)); ativa->simular_passo();
        exigir(ge->get_estado_atual() == 10 && !ativa->get_vitoria(), "Derrota pede o nome em ambos os modos");
        ge->set_estado_atual(0); ge->deleta_estados(id); ge->set_fase(-1);
        static_cast<Estados::Menus::Menu_Principal*>(ge->get_estado(0))->fase_salva();
        exigir(ge->get_estado_atual() == 10, "Nome pendente da derrota sobrevive ao fechamento");
        auto* gg = Gerenciadores::Gerenciador_Grafico::get_instancia();
        gg->limpar(); ge->get_estado(10)->executar(); gg->mostrar();
        sf::Texture imagem; imagem.create(gg->get_Janela()->getSize().x, gg->get_Janela()->getSize().y);
        imagem.update(*gg->get_Janela()); imagem.copyToImage().saveToFile("derrota-verificada.png");
        const auto antes = Persistencia::ler_json("ranking.json");
        sf::Event letra{}; letra.type = sf::Event::TextEntered; letra.text.unicode = 'Z';
        const int quantidade = id == 9 ? 2 : 1;
        for (int i = 0; i < quantidade; ++i) {
            ge->get_estado(10)->tratar_evento(letra);
            ge->get_estado(10)->tratar_evento(tecla(sf::Keyboard::Enter));
            if (i + 1 < quantidade) exigir(ge->get_estado_atual() == 10, "Derrota em dupla pede os dois nomes");
        }
        exigir(ge->get_estado_atual() == 0, "Derrota volta ao menu apos os nomes");
        const auto salvo = Persistencia::ler_json("partida.json");
        exigir(salvo["nomes_confirmados"] == true && salvo["ranking_registrado"] == false,
               "Nome da derrota e salvo separadamente do ranking");
        for (const auto& jogador : salvo["jogadores"]) exigir(jogador["extra"]["nome"] == "Z", "Nome da derrota persistido");
        exigir(Persistencia::ler_json("ranking.json") == antes, "Derrota nao entra como tempo de conclusao");
    }
}
void fluxos() {
    auto* ge = Gerenciadores::Gerenciador_Estados::get_instancia();
    auto* menu = static_cast<Estados::Menus::Menu_Principal*>(ge->get_estado(0));
    menu->fase_salva();
    exigir(ge->get_estado_atual() == 0 && !ge->mensagem.empty(), "Continuar sem arquivo permanece no menu");
    auto fase = criar(7, false); const auto inicial = cenario_de_teste(*fase); fase->restaurar(inicial);
    ge->adicionar_estado(fase.get()); fase.release(); ge->set_estado_atual(7);
    auto* ativa = static_cast<Fase*>(ge->get_estado(7));
    ativa->tratar_evento(tecla(sf::Keyboard::Escape));
    exigir(ge->get_estado_atual() == 5, "Escape abre a pausa");
    const auto pausada = Persistencia::ler_json("partida.json");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ge->get_estado(5)->executar();
    exigir(ativa->capturar() == pausada, "Pausa nao avanca nenhum estado ou tempo");
    ge->get_estado(5)->tratar_evento(tecla(sf::Keyboard::Escape));
    exigir(ge->get_estado_atual() == 7 && ativa->capturar() == pausada, "Continuar em memoria preserva o estado");
    ge->set_estado_atual(5); ge->set_estado_atual(0); ge->deleta_estados(7); ge->set_fase(-1);
    menu->fase_salva();
    exigir(ge->get_estado_atual() == 7 && ge->get_fase() == 7, "Continuar pelo arquivo restaura fase e modo");
    exigir(static_cast<Fase*>(ge->get_estado(7))->capturar() == pausada, "Continuar restaura snapshot completo");
    ge->set_estado_atual(5); ge->set_estado_atual(0);
    auto fim = criar(8, false); auto vencida = cenario_de_teste(*fim);
    for (auto& inimigo : vencida["inimigos"]) { inimigo["vivo"] = false; if (inimigo["tipo"] == "arqueiro") inimigo["extra"]["projeteis"] = Json::array(); }
    fim->restaurar(vencida); ge->adicionar_estado(fim.get()); fim.release(); ge->set_estado_atual(8);
    static_cast<Fase*>(ge->get_estado(8))->simular_passo();
    exigir(ge->get_estado_atual() == 10, "Vitoria da fase 2 abre entrada do nome");
    ge->get_estado(10)->tratar_evento(tecla(sf::Keyboard::Escape));
    ge->deleta_estados(8); ge->set_fase(-1); menu->fase_salva();
    exigir(ge->get_estado_atual() == 10, "Resultado pendente sobrevive ao fechamento");
    sf::Event letra{}; letra.type = sf::Event::TextEntered; letra.text.unicode = 'A';
    ge->get_estado(10)->tratar_evento(letra);
    ge->get_estado(10)->tratar_evento(tecla(sf::Keyboard::Enter));
    exigir(ge->get_estado_atual() == 4, "Confirmar nome abre ranking");
    auto lista = Persistencia::RepositorioRanking().consultar(2, 1);
    bool encontrado = false; for (const auto& r : lista) if (r.nomes[0] == "A") encontrado = true;
    exigir(encontrado, "Conclusao registra nome e tempo");
    auto* concluida = static_cast<Fase*>(ge->get_estado(8));
    concluida->registrar_resultado({"A"});
    exigir(Persistencia::RepositorioRanking().consultar(2, 1).size() == lista.size(), "Confirmacao repetida nao duplica");
    // Registra imagens para inspecao, sem controlar o aplicativo por outra linguagem.
    auto* gg = Gerenciadores::Gerenciador_Grafico::get_instancia();
    gg->limpar(); ge->get_estado(4)->executar(); gg->mostrar();
    sf::Texture imagem; imagem.create(gg->get_Janela()->getSize().x, gg->get_Janela()->getSize().y);
    imagem.update(*gg->get_Janela()); imagem.copyToImage().saveToFile("ranking-verificado.png");

    derrota_com_nome();

    // A fase 1 cooperativa registra os dois nomes e mantem o avanco original.
    Persistencia::RepositorioRanking().registrar({"recorde-painel", 2, 2, {"Ana", "Bia"}, 12.5});
    ge->set_estado_atual(0);
    auto dupla = criar(7, false); auto fim_dupla = cenario_de_teste(*dupla);
    for (auto& inimigo : fim_dupla["inimigos"]) {
        inimigo["vivo"] = false;
        if (inimigo["tipo"] == "arqueiro") inimigo["extra"]["projeteis"] = Json::array();
    }
    dupla->restaurar(fim_dupla); ge->adicionar_estado(dupla.get()); dupla.release(); ge->set_estado_atual(7);
    static_cast<Fase*>(ge->get_estado(7))->simular_passo();
    const auto tempo_primeira_fase = static_cast<Fase*>(ge->get_estado(7))->get_passos_sessao();
    ge->get_estado(10)->tratar_evento(letra); ge->get_estado(10)->tratar_evento(tecla(sf::Keyboard::Enter));
    exigir(ge->get_estado_atual() == 10, "Dupla exige o segundo nome");
    letra.text.unicode = 'B'; ge->get_estado(10)->tratar_evento(letra);
    ge->get_estado(10)->tratar_evento(tecla(sf::Keyboard::Enter));
    exigir(ge->get_estado_atual() == 9 && ge->get_fase() == 9, "Dupla avanca para fase 2 corretamente");
    exigir(static_cast<Fase*>(ge->get_estado(9))->get_passos_sessao() == tempo_primeira_fase,
           "Troca automatica de fase preserva tempo total da partida");
    exigir(Persistencia::RepositorioRanking().consultar(1, 2).size() == 1, "Dupla registra um resultado de equipe");

    ge->get_estado(9)->executar(); gg->mostrar();
    exigir(gg->get_Janela()->getView().getViewport().top > 0,
           "Painel reserva espaco sem esconder os jogadores");
    imagem.update(*gg->get_Janela()); imagem.copyToImage().saveToFile("fase-verificada.png");
    fs::create_directory("partida.json.tmp");
    auto* eventos = Gerenciadores::Gerenciador_Eventos::get_instancia();
    eventos->processar_evento(tecla(sf::Keyboard::Escape));
    exigir(ge->get_estado_atual() == 5 && ge->mensagem.find("Nao foi possivel") != std::string::npos,
           "Erro de escrita nao impede a pausa nem fica oculto");
    const auto preservado = Persistencia::ler_json("partida.json");
    fs::create_directory("partida.json.tmp");
    sf::Event fechar{}; fechar.type = sf::Event::Closed; eventos->processar_evento(fechar);
    exigir(gg->get_JanelaAberta(), "Falha de salvamento nao fecha a janela e perde o progresso");
    exigir(Persistencia::ler_json("partida.json") == preservado, "Fechamento mal sucedido preserva checkpoint");
    eventos->processar_evento(tecla(sf::Keyboard::S));
    exigir(ge->mensagem == "Partida salva.", "S repete o salvamento na pausa");
    gg->limpar(); ge->get_estado(5)->executar(); gg->mostrar();
    imagem.update(*gg->get_Janela()); imagem.copyToImage().saveToFile("pausa-verificada.png");
    const auto ao_fechar = static_cast<Fase*>(ge->get_estado(9))->capturar();
    eventos->processar_evento(fechar);
    exigir(!gg->get_JanelaAberta(), "Fechamento normal encerra a janela");
    exigir(Persistencia::ler_json("partida.json") == ao_fechar, "Fechar em pausa salva todos os estados");
}
}
int main(int argc, char** argv) {
    try {
        const auto recursos = fs::absolute("Design");
        const auto pasta = argc == 3 ? fs::absolute(argv[2]) : fs::temp_directory_path() /
            ("zombies-testes-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        preparar_pasta(pasta, recursos);
        menus();
        if (argc == 3 && std::string(argv[1]) == "--produzir") {
            auto fase = criar(9, false); fase->restaurar(cenario_de_teste(*fase)); fase->salvar("checkpoint.json");
            for (int i = 0; i < 80; ++i) fase->simular_passo();
            Persistencia::escrever_json("esperado.json", fase->capturar());
            std::cout << "Checkpoint e continuacao de referencia produzidos.\n";
        } else if (argc == 3 && std::string(argv[1]) == "--verificar") {
            auto fase = criar(9, true); fase->restaurar(Persistencia::ler_json("checkpoint.json"));
            for (int i = 0; i < 80; ++i) fase->simular_passo();
            exigir(fase->capturar() == Persistencia::ler_json("esperado.json"), "Continuacao em processo novo deve ser identica");
            std::cout << "Retomada em processo novo: todos os estados identicos apos 80 passos.\n";
        } else {
            persistencia(); ranking(); movimento_e_tempo(); fluxos();
            std::cout << verificacoes << " verificacoes aprovadas. Artefatos: " << pasta << '\n';
        }
        delete Gerenciadores::Gerenciador_Estados::get_instancia();
        delete Gerenciadores::Gerenciador_Eventos::get_instancia();
        delete Gerenciadores::Gerenciador_Grafico::get_instancia();
        return 0;
    } catch (const std::exception& erro) { std::cerr << "FALHA: " << erro.what() << '\n'; return 1; }
}
