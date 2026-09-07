#include "../Persistencia/mundo.h"
#include "../Persistencia/entidades.h"
#include "../Persistencia/migracao.h"
#include <set>
#include "../Entidades/Personagens/jogador.h"
#include "../Entidades/Personagens/zumbi.h"
#include "../Entidades/Personagens/arqueiro.h"
#include "../Entidades/Personagens/gigante.h"
#include "../Entidades/Obstaculos/coracao.h"
#include "../Persistencia/aleatorio.h"
#include "../Persistencia/pontos.h"
namespace Persistencia {
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
Json capturar_mundo(Logica::Mundo& m) {
    auto& jogadores=m.jogadores; auto& inimigos=m.inimigos; auto& obstaculos=m.obstaculos;
    auto& gC=m.gC; auto& sessao=m.sessao; auto& resultado=m.resultado;
    auto& motor_fase=m.motor_fase;
    auto& pontuacao=m.pontuacao; auto& partida_id=m.partida_id; const int num_jogadores=m.num_jogadores;

    std::ostringstream aleatorio;
    aleatorio << motor_fase;
    return {{"formato", "zombies-partida"}, {"versao", 2}, {"estado", m.estado},
            {"fase", m.numero_fase()}, {"numero_jogadores", num_jogadores},
            {"passos", sessao.passos}, {"passos_anteriores", sessao.passos_anteriores},
            {"acumulador", sessao.acumulador}, {"partida_id", partida_id},
            {"finalizada", resultado.finalizada}, {"vitoria", resultado.vitoria}, {"ranking_registrado", resultado.ranking_registrado},
            {"nomes_confirmados", resultado.nomes_confirmados}, {"pontos", pontuacao.pontos()},
            {"aleatorio", aleatorio.str()},
            {"jogadores", salvar_lista(jogadores)}, {"inimigos", salvar_lista(inimigos)},
            {"obstaculos", salvar_lista(obstaculos)}};
}
void restaurar_mundo(Logica::Mundo& m, const Json& dados) {
    auto& jogadores=m.jogadores; auto& inimigos=m.inimigos; auto& obstaculos=m.obstaculos;
    auto& gC=m.gC; auto& sessao=m.sessao; auto& resultado=m.resultado;
    auto& motor_fase=m.motor_fase;
    auto& pontuacao=m.pontuacao; auto& partida_id=m.partida_id; const int num_jogadores=m.num_jogadores;

    const auto motor_anterior = Persistencia::motor();
    try {
        const auto j=Persistencia::migrar_partida(dados);
        if (j.at("formato") != "zombies-partida" ||
            Persistencia::inteiro(j.at("estado"), 6, 9) != m.estado ||
            Persistencia::inteiro(j.at("fase"), 1, 2) != m.numero_fase() ||
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
        for (auto it = novos_jogadores.get_primeiro(); it != nullptr; ++it) {
            identidades.insert(static_cast<Entidades::Personagens::Jogador*>(*it)->eh_jogador2());
            static_cast<Entidades::Personagens::Jogador*>(*it)->set_controles_solo(num_jogadores==1);
        }
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
    } catch (...) { Persistencia::motor() = motor_anterior; throw; }
}
}
