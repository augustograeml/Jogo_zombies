#include "../Estados/Fases/fase.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/aleatorio.h"
#include "../Persistencia/ranking.h"
#include <chrono>
#include <fstream>
#include <memory>
#include <set>

namespace Estados::Fases {
namespace {
using Persistencia::Json;
Json salvar_lista(Listas::ListaEntidade& lista) {
    Json dados = Json::array();
    for (auto it = lista.get_primeiro(); it != nullptr; ++it)
        dados.push_back(Persistencia::Serializador::salvar(**it));
    return dados;
}
void carregar_lista(const Json& dados, Listas::ListaEntidade& lista, int grupo) {
    if (!dados.is_array() || dados.size() > 10000) throw std::runtime_error("Lista de entidades invalida.");
    // A lista insere na frente: percorrer ao contrario preserva a ordem das colisoes.
    for (auto it = dados.rbegin(); it != dados.rend(); ++it) {
        auto entidade = Persistencia::Serializador::carregar(*it);
        const bool jogador = dynamic_cast<Entidades::Personagens::Jogador*>(entidade.get()) != nullptr;
        const bool inimigo = dynamic_cast<Entidades::Personagens::Inimigo*>(entidade.get()) != nullptr;
        const bool obstaculo = dynamic_cast<Entidades::Obstaculos::Obstaculo*>(entidade.get()) != nullptr;
        if ((grupo == 0 && !jogador) || (grupo == 1 && !inimigo) || (grupo == 2 && !obstaculo))
            throw std::runtime_error("Entidade na lista errada.");
        lista.incluir(entidade.get());
        entidade.release();
    }
}
}
Fase::Fase(int id, bool carregar) : Estado(id), ja_criado(carregar),
    jogador2(id == 7 || id == 9), num_jogadores(jogador2 ? 2 : 1) {
    gC.set_jogadores(&jogadores);
    gC.set_inimigos(&inimigos);
    gC.set_obstaculos(&obstaculos);
    std::random_device aleatorio;
    partida_id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) +
                 "-" + std::to_string(aleatorio()) + "-" + std::to_string(id);
    if (!carregar) criar_jogadores();
}
void Fase::criar_jogadores() {
    // Insercao invertida mantem jogador 1 como primeiro elemento.
    if (jogador2) jogadores.incluir(new Entidades::Personagens::Jogador({150, 50}, {0, 0}, true));
    jogadores.incluir(new Entidades::Personagens::Jogador({80, 50}, {0, 0}, false));
}
void Fase::criar_inimigos(std::string caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo) throw std::runtime_error("Cenario nao encontrado: " + caminho);
    std::string linha;
    int y = 0;
    while (std::getline(arquivo, linha)) {
        int x = 0;
        for (char tipo : linha) {
            const sf::Vector2f pos(x++ * TAM, y * TAM);
            if (tipo == '3') inimigos.incluir(new Entidades::Personagens::Zumbi(pos, {0, 0}));
            if (tipo == '4') inimigos.incluir(new Entidades::Personagens::Arqueiro(pos, {0, 0}));
            if (tipo == '9') inimigos.incluir(new Entidades::Personagens::Gigante(pos, {0, 0}));
        }
        ++y;
    }
}
void Fase::criar_cenario(std::string caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo) throw std::runtime_error("Cenario nao encontrado: " + caminho);
    std::string linha;
    int y = 0;
    while (std::getline(arquivo, linha)) {
        int x = 0;
        for (char tipo : linha) {
            const sf::Vector2f pos(x++ * TAM, y * TAM);
            using namespace Entidades::Obstaculos;
            switch (tipo) {
                case '0': obstaculos.incluir(new Neve(pos)); break;
                case '5': obstaculos.incluir(new Espinho(pos)); break;
                case '6': obstaculos.incluir(new Coracao(pos)); break;
                case '7': obstaculos.incluir(new Musgo(pos)); break;
                case '8': obstaculos.incluir(new Caixa(pos)); break;
                default: break;
            }
        }
        ++y;
    }
}
void Fase::ao_entrar() {
    relogio.restart(); // Descarta tempo no menu, em pausa e fora do processo.
    atualizar();
}
void Fase::tratar_evento(const sf::Event& evento) {
    if ((evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Escape) ||
        evento.type == sf::Event::LostFocus) pGE->set_estado_atual(5);
}
void Fase::simular_passo() {
    if (finalizada) return;
    Persistencia::motor() = motor_fase;
    jogadores.executar();
    inimigos.executar();
    motor_fase = Persistencia::motor();
    gerenciar_colisoes();
    // Morte por queda evita uma partida sem possibilidade de terminar.
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it)
        if ((*it)->get_vida() <= 0 || (*it)->getPosicao().y > 2000) (*it)->morrer();
    ++passos;
    set_tempo_jogadores();
    if (gC.get_jogadores_vivos()) concluir(false);
    else if (gC.get_inimigos_vivos()) concluir(true);
}
void Fase::executar_comum() {
    constexpr double passo = 1.0 / 60.0;
    acumulador += std::min(0.25, static_cast<double>(relogio.restart().asSeconds()));
    while (acumulador >= passo && !finalizada) {
        acumulador -= passo;
        simular_passo();
    }
    if (pGE->get_estado_atual() != Estado::id) return;
    atualizar();
    pGG->desenharFundo(&shape);
    obstaculos.desenhar();
    jogadores.desenhar();
    inimigos.desenhar();
    for (auto it = inimigos.get_primeiro(); it != nullptr; ++it) {
        auto* projeteis = static_cast<Entidades::Personagens::Inimigo*>(*it)->get_projeteis();
        if (projeteis) for (auto& p : *projeteis) if (p.get_vivo()) p.desenhar();
    }
}
void Fase::atualizar() {
    sf::Vector2f centro(0, 0);
    int vivos = 0;
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        if ((*it)->get_vivo()) { centro += (*it)->getPosicao(); ++vivos; }
    }
    if (vivos) pGG->centralizarCamera(centro / static_cast<float>(vivos));
}
void Fase::set_tempo_jogadores() {
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it)
        static_cast<Entidades::Personagens::Jogador*>(*it)->set_tempo(get_tempo());
}
void Fase::fim_de_jogo() { concluir(true); }
void Fase::concluir(bool venceu) {
    if (finalizada) return;
    finalizada = true;
    vitoria = venceu;
    set_tempo_jogadores();
    try { salvar(); }
    catch (const std::exception& erro) { pGE->mensagem = std::string("Falha ao salvar resultado: ") + erro.what(); }
    pGE->set_estado_atual(venceu ? 10 : 0);
    if (!venceu) pGE->mensagem = "Fim de jogo. Escolha Novo Jogo para tentar novamente.";
}
Json Fase::capturar() {
    std::ostringstream aleatorio;
    aleatorio << motor_fase;
    return {{"formato", "zombies-partida"}, {"versao", 1}, {"estado", Estado::id},
            {"fase", get_numero_fase()}, {"numero_jogadores", num_jogadores},
            {"passos", passos}, {"acumulador", acumulador}, {"partida_id", partida_id},
            {"finalizada", finalizada}, {"vitoria", vitoria}, {"ranking_registrado", ranking_registrado},
            {"aleatorio", aleatorio.str()},
            {"jogadores", salvar_lista(jogadores)}, {"inimigos", salvar_lista(inimigos)},
            {"obstaculos", salvar_lista(obstaculos)}};
}
void Fase::salvar(const std::filesystem::path& caminho) { Persistencia::escrever_json(caminho, capturar()); }
void Fase::restaurar(const Json& j) {
    const auto motor_anterior = Persistencia::motor();
    try {
        if (j.at("formato") != "zombies-partida" || Persistencia::inteiro(j.at("versao"), 1, 1) != 1 ||
            Persistencia::inteiro(j.at("estado"), 6, 9) != Estado::id ||
            Persistencia::inteiro(j.at("fase"), 1, 2) != get_numero_fase() ||
            Persistencia::inteiro(j.at("numero_jogadores"), 1, 2) != num_jogadores)
            throw std::runtime_error("Salvamento incompativel com esta fase.");
        if (!j.at("passos").is_number_integer()) throw std::runtime_error("Tempo de partida invalido.");
        auto novos_passos = static_cast<std::uint64_t>(Persistencia::numero(j.at("passos"), 0, 6e13));
        double novo_acumulador = Persistencia::numero(j.at("acumulador"), 0, 0.25);
        auto novo_id = j.at("partida_id").get<std::string>();
        if (novo_id.empty() || novo_id.size() > 128) throw std::runtime_error("Identificador de partida invalido.");
        const bool terminou = j.at("finalizada").get<bool>();
        const bool venceu = j.at("vitoria").get<bool>();
        const bool registrado = j.at("ranking_registrado").get<bool>();
        if ((venceu && !terminou) || (registrado && !venceu)) throw std::runtime_error("Resultado inconsistente.");
        auto novo_motor = Persistencia::ler_motor(j.at("aleatorio").get<std::string>());
        Listas::ListaEntidade novos_jogadores, novos_inimigos, novos_obstaculos;
        carregar_lista(j.at("jogadores"), novos_jogadores, 0);
        carregar_lista(j.at("inimigos"), novos_inimigos, 1);
        carregar_lista(j.at("obstaculos"), novos_obstaculos, 2);
        if (novos_jogadores.get_tamanho() != num_jogadores) throw std::runtime_error("Quantidade de jogadores invalida.");
        std::set<bool> identidades;
        for (auto it = novos_jogadores.get_primeiro(); it != nullptr; ++it)
            identidades.insert(static_cast<Entidades::Personagens::Jogador*>(*it)->eh_jogador2());
        if (identidades.size() != static_cast<std::size_t>(num_jogadores) || !identidades.count(false))
            throw std::runtime_error("Identidades dos jogadores invalidas.");
        jogadores.trocar(novos_jogadores);
        inimigos.trocar(novos_inimigos);
        obstaculos.trocar(novos_obstaculos);
        passos = novos_passos;
        acumulador = novo_acumulador;
        partida_id = std::move(novo_id);
        finalizada = terminou;
        vitoria = venceu;
        ranking_registrado = registrado;
        Persistencia::motor() = novo_motor;
        motor_fase = novo_motor;
        relogio.restart();
    } catch (...) { Persistencia::motor() = motor_anterior; throw; }
}
bool Fase::registrar_resultado(const std::vector<std::string>& nomes) {
    if (!finalizada || !vitoria) throw std::runtime_error("A fase ainda nao foi vencida.");
    if (ranking_registrado) { salvar(); return true; }
    Persistencia::RepositorioRanking().registrar({partida_id, get_numero_fase(), num_jogadores, nomes, get_tempo()});
    ranking_registrado = true;
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        auto* jogador = static_cast<Entidades::Personagens::Jogador*>(*it);
        jogador->set_nome(nomes.at(jogador->eh_jogador2() ? 1 : 0));
    }
    salvar();
    return true;
}
}
