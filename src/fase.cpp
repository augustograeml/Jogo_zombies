#include "../Estados/Fases/construtor_cenario.h"
#include "../Persistencia/slots.h"
#include "../Persistencia/pontos.h"
#include "../Persistencia/migracao.h"
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
    eventos.assinar(pontuacao);
    eventos.assinar(som);
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
void Fase::criar_inimigos(std::string caminho) { ConstrutorCenario::inimigos(caminho, inimigos); }
void Fase::criar_cenario(std::string caminho) { ConstrutorCenario::obstaculos(caminho, obstaculos); gC.invalidar_grade(); }
void Fase::ao_entrar() {
    painel.atualizar_recorde(get_numero_fase(), num_jogadores);
    relogio.restart(); // Descarta tempo no menu, em pausa e fora do processo.
    atualizar();
}
void Fase::tratar_evento(const sf::Event& evento) {
    if ((evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Escape) ||
        evento.type == sf::Event::LostFocus) pGE->set_estado_atual(Estados::Tela::Pausa);
}
void Fase::simular_passo() {
    if (resultado.finalizada) return;
    const auto inicio = std::chrono::steady_clock::now();
    Persistencia::motor() = motor_fase;
    std::vector<Logica::Alvo> alvos;
    struct Antes { Entidades::Entidade* entidade; int vida; bool vivo; float vy; };
    std::vector<Antes> antes;
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        auto* j = static_cast<Entidades::Personagens::Jogador*>(*it);
        j->atualizar_protecao();
        alvos.push_back({j->eh_jogador2() ? 2 : 1, j->getPosicao().x, j->getPosicao().y, j->get_vivo()});
        antes.push_back({j,j->get_vida(),j->get_vivo(),j->getVelocidade().y});
    }
    for (auto it = inimigos.get_primeiro(); it != nullptr; ++it) {
        auto* a = dynamic_cast<Entidades::Personagens::Arqueiro*>(*it);
        if (a) a->perceber(alvos);
        antes.push_back({*it,(*it)->get_vida(),(*it)->get_vivo(),0});
    }
    for (auto it = obstaculos.get_primeiro(); it != nullptr; ++it)
        if (dynamic_cast<Entidades::Obstaculos::Coracao*>(*it))
            antes.push_back({*it,(*it)->get_vida(),(*it)->get_vivo(),0});
    jogadores.executar();
    inimigos.executar();
    motor_fase = Persistencia::motor();
    gerenciar_colisoes();
    // Morte por queda evita uma partida sem possibilidade de terminar.
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it)
        if ((*it)->get_vida() <= 0 || (*it)->getPosicao().y > 2000) (*it)->morrer();
    for (const auto& a : antes) {
        auto* e = a.entidade;
        if (auto* j = dynamic_cast<Entidades::Personagens::Jogador*>(e)) {
            if (j->get_vida() < a.vida) eventos.publicar({Logica::Evento::Dano});
            if (a.vy >= 0 && j->getVelocidade().y < -5) eventos.publicar({Logica::Evento::Salto});
        }
        if (auto* arq = dynamic_cast<Entidades::Personagens::Arqueiro*>(e))
            if (arq->disparou_no_passo()) eventos.publicar({Logica::Evento::Disparo});
        if (a.vivo && !e->get_vivo()) {
            using namespace Persistencia::Pontuacao;
            if (dynamic_cast<Entidades::Obstaculos::Coracao*>(e)) eventos.publicar({Logica::Evento::Coleta, coleta});
            else if (dynamic_cast<Entidades::Personagens::Inimigo*>(e)) {
                const int pontos = dynamic_cast<Entidades::Personagens::Gigante*>(e) ? gigante :
                    dynamic_cast<Entidades::Personagens::Arqueiro*>(e) ? arqueiro : zumbi;
                eventos.publicar({Logica::Evento::InimigoDerrotado, pontos});
            }
        }
    }
    ++sessao.passos;
    set_tempo_jogadores();
    if (gC.get_jogadores_vivos()) concluir(false);
    else if (gC.get_inimigos_vivos()) concluir(true);
    micros_simulacao=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-inicio).count();
}
void Fase::executar_comum() {
    sessao.acumular(relogio.restart().asSeconds());
    while (!resultado.finalizada && sessao.proximo()) {
        simular_passo();
    }
    if (pGE->get_estado_atual() != Estado::id) return;
    const auto inicio_desenho=std::chrono::steady_clock::now();
    atualizar();
    pGG->desenharFundo(&shape);
    const auto& camera=pGG->get_Janela()->getView();
    const auto canto=camera.getCenter()-camera.getSize()/2.f;
    // Margem conserva sprites que ultrapassam a caixa fisica; so o desenho e filtrado.
    const sf::FloatRect visivel(canto.x-100,canto.y-100,camera.getSize().x+200,camera.getSize().y+200);
    obstaculos.desenhar(visivel);
    jogadores.desenhar(visivel);
    inimigos.desenhar(visivel);
    for (auto it = inimigos.get_primeiro(); it != nullptr; ++it) {
        auto* projeteis = static_cast<Entidades::Personagens::Inimigo*>(*it)->get_projeteis();
        if (projeteis) for (auto& p : *projeteis) if (p.get_vivo() && visivel.intersects(p.get_corpo()->getGlobalBounds())) p.desenhar();
    }
    std::vector<int> vidas;
    for(auto it=jogadores.get_primeiro();it!=nullptr;++it) vidas.push_back((*it)->get_vida());
    painel.desenhar(*pGG->get_Janela(), get_tempo_sessao(), get_pontos(), vidas);
    micros_desenho=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-inicio_desenho).count();
}
void Fase::atualizar() {
    sf::Vector2f centro(0, 0);
    int vivos = 0;
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        if ((*it)->get_vivo()) { centro += (*it)->getPosicao(); ++vivos; }
    }
    // Reserva uma faixa para o painel sem cobrir o jogador na plataforma inicial.
    auto* janela = pGG->get_Janela();
    auto camera = janela->getView();
    const auto tamanho = janela->getDefaultView().getSize();
    constexpr float altura_painel = 158;
    camera.setSize(tamanho.x, tamanho.y - altura_painel);
    if (vivos) {
        centro = centro/static_cast<float>(vivos) + sf::Vector2f(25,25);
        const auto metade=camera.getSize()/2.f;
        const auto limites=shape.getGlobalBounds();
        centro.x=std::clamp(centro.x,metade.x,std::max(metade.x,limites.width-metade.x));
        centro.y=std::clamp(centro.y,metade.y,std::max(metade.y,limites.height-metade.y));
        camera.setCenter(centro);
    }
    camera.setViewport({0, altura_painel / tamanho.y, 1, 1 - altura_painel / tamanho.y});
    janela->setView(camera);
}
void Fase::set_tempo_jogadores() {
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it)
        static_cast<Entidades::Personagens::Jogador*>(*it)->set_tempo(get_tempo());
}
void Fase::fim_de_jogo() { concluir(true); }
void Fase::concluir(bool venceu) {
    if (resultado.finalizada) return;
    resultado.finalizada = true;
    resultado.vitoria = venceu;
    if (venceu) eventos.publicar({Logica::Evento::Vitoria, Persistencia::Pontuacao::conclusao});
    set_tempo_jogadores();
    try { salvar(); }
    catch (const std::exception& erro) { pGE->mensagem = std::string("Falha ao salvar resultado: ") + erro.what(); }
    pGE->set_estado_atual(Estados::Tela::Nome);
}
Json Fase::capturar() {
    std::ostringstream aleatorio;
    aleatorio << motor_fase;
    return {{"formato", "zombies-partida"}, {"versao", 2}, {"estado", Estado::id},
            {"fase", get_numero_fase()}, {"numero_jogadores", num_jogadores},
            {"passos", sessao.passos}, {"passos_anteriores", sessao.passos_anteriores},
            {"acumulador", sessao.acumulador}, {"partida_id", partida_id},
            {"finalizada", resultado.finalizada}, {"vitoria", resultado.vitoria}, {"ranking_registrado", resultado.ranking_registrado},
            {"nomes_confirmados", resultado.nomes_confirmados}, {"pontos", pontuacao.pontos()},
            {"aleatorio", aleatorio.str()},
            {"jogadores", salvar_lista(jogadores)}, {"inimigos", salvar_lista(inimigos)},
            {"obstaculos", salvar_lista(obstaculos)}};
}
void Fase::salvar(const std::filesystem::path& caminho) {
    if (caminho.empty()) Persistencia::Slots::instancia().salvar(capturar());
    else Persistencia::escrever_json(caminho, capturar());
}
void Fase::restaurar(const Json& dados) {
    const auto motor_anterior = Persistencia::motor();
    try {
        const auto j=Persistencia::migrar_partida(dados);
        if (j.at("formato") != "zombies-partida" ||
            Persistencia::inteiro(j.at("estado"), 6, 9) != Estado::id ||
            Persistencia::inteiro(j.at("fase"), 1, 2) != get_numero_fase() ||
            Persistencia::inteiro(j.at("numero_jogadores"), 1, 2) != num_jogadores)
            throw std::runtime_error("Salvamento incompativel com esta fase.");
        if (!j.at("passos").is_number_integer()) throw std::runtime_error("Tempo de partida invalido.");
        auto novos_passos = static_cast<std::uint64_t>(Persistencia::numero(j.at("passos"), 0, 6e13));
        const auto anteriores = j.value("passos_anteriores", Json(0));
        if (!anteriores.is_number_integer()) throw std::runtime_error("Tempo anterior invalido.");
        const auto novos_anteriores = static_cast<std::uint64_t>(Persistencia::numero(anteriores, 0, 6e13));
        double novo_acumulador = Persistencia::numero(j.at("acumulador"), 0, 0.25);
        auto novo_id = j.at("partida_id").get<std::string>();
        if (novo_id.empty() || novo_id.size() > 128) throw std::runtime_error("Identificador de partida invalido.");
        const bool terminou = j.at("finalizada").get<bool>();
        const bool venceu = j.at("vitoria").get<bool>();
        const bool registrado = j.at("ranking_registrado").get<bool>();
        const bool confirmados = j.value("nomes_confirmados", registrado);
        if ((venceu && !terminou) || (registrado && (!venceu || !confirmados)) || (confirmados && !terminou))
            throw std::runtime_error("Resultado inconsistente.");
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
        const int pontos = Persistencia::inteiro(j.value("pontos", Json(0)), 0, 1000000000);
        jogadores.trocar(novos_jogadores);
        inimigos.trocar(novos_inimigos);
        obstaculos.trocar(novos_obstaculos);
        gC.invalidar_grade();
        pontuacao.restaurar(pontos);
        sessao.passos = novos_passos;
        sessao.passos_anteriores = novos_anteriores;
        sessao.acumulador = novo_acumulador;
        partida_id = std::move(novo_id);
        resultado.finalizada = terminou;
        resultado.vitoria = venceu;
        resultado.ranking_registrado = registrado;
        resultado.nomes_confirmados = confirmados;
        Persistencia::motor() = novo_motor;
        motor_fase = novo_motor;
        relogio.restart();
    } catch (...) { Persistencia::motor() = motor_anterior; throw; }
}
bool Fase::registrar_resultado(const std::vector<std::string>& nomes) {
    if (!resultado.finalizada) throw std::runtime_error("A partida ainda nao terminou.");
    if (resultado.nomes_confirmados) { salvar(); return resultado.ranking_registrado; }
    if (nomes.size() != static_cast<std::size_t>(num_jogadores))
        throw std::runtime_error("Informe o nome de cada jogador.");
    std::vector<std::string> validados;
    for (const auto& nome : nomes) validados.push_back(Persistencia::nome_valido(nome));
    // Derrotas recebem nome, mas nao competem com tempos de fases concluidas.
    if (resultado.vitoria) {
        Persistencia::RepositorioRanking().registrar({partida_id, get_numero_fase(), num_jogadores, validados, get_tempo()});
    }
    if (get_pontos() > 0) Persistencia::RepositorioPontos().registrar(
        {partida_id, get_numero_fase(), num_jogadores, validados, get_pontos(), get_tempo(), resultado.vitoria});
    for (auto it = jogadores.get_primeiro(); it != nullptr; ++it) {
        auto* jogador = static_cast<Entidades::Personagens::Jogador*>(*it);
        jogador->set_nome(validados.at(jogador->eh_jogador2() ? 1 : 0));
    }
    resultado.ranking_registrado = resultado.vitoria;
    resultado.nomes_confirmados = true;
    salvar();
    return resultado.ranking_registrado;
}
}
