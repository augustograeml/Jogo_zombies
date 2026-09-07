#pragma once
#include "../../Entidades/Personagens/jogador.h"
#include "../../Entidades/Personagens/zumbi.h"
#include "../../Entidades/Personagens/arqueiro.h"
#include "../../Entidades/Personagens/gigante.h"
#include "../../Entidades/Obstaculos/neve.h"
#include "../../Entidades/Obstaculos/musgo.h"
#include "../../Entidades/Obstaculos/espinho.h"
#include "../../Entidades/Obstaculos/caixa.h"
#include "../../Entidades/Obstaculos/coracao.h"
#include "../../Listas/listaEntidades.h"
#include "../../Gerenciadores/gerenciador_colisoes.h"
#include "../estado.h"
#include "../../Persistencia/arquivo.h"
#include <cstdint>
#include "../../Logica/eventos.h"
#include "../../Audio/efeitos.h"
#include "../../Logica/sessao.h"
#include <random>
#include "../../Interface/painel.h"
#include "../../Interface/camera.h"

namespace Logica { struct Mundo; }
namespace Estados::Fases {
class Fase : public Ente, public Estado {
protected:
    bool ja_criado;
    bool jogador2;
    sf::Texture Textura;
    sf::RectangleShape shape;
    Listas::ListaEntidade jogadores, obstaculos, inimigos;
    Gerenciadores::Gerenciador_Colisoes gC;
    int num_jogadores;
    sf::Clock relogio;
    Logica::Sessao sessao;
    Logica::ResultadoPartida resultado;
    Logica::Eventos eventos;
    Logica::Pontuacao pontuacao;
    Audio::ObservadorSom som;
    Interface::PainelPartida painel;
    Interface::CameraDupla camera_dupla;
    std::vector<sf::View> vistas;
    std::string partida_id;
    std::mt19937 motor_fase;
    double micros_simulacao=0, micros_desenho=0;
    Logica::Mundo mundo();
    void executar_comum();
    void desenhar_partida();
    void concluir(bool venceu);
public:
    explicit Fase(int id, bool carregar = false);
    ~Fase() override = default;
    void criar_jogadores();
    void criar_inimigos(std::string cenario);
    void criar_cenario(std::string caminho);
    void gerenciar_colisoes() { gC.gerenciar_colisoes(); }
    bool get_jaCriado() const { return ja_criado; }
    bool get_jogador2() const { return jogador2; }
    bool get_finalizada() const { return resultado.finalizada; }
    bool get_vitoria() const { return resultado.vitoria; }
    bool get_ranking_registrado() const { return resultado.ranking_registrado; }
    bool get_nomes_confirmados() const { return resultado.nomes_confirmados; }
    int get_num_jogadores() const { return num_jogadores; }
    int get_numero_fase() const { return Estado::id < 8 ? 1 : 2; }
    double get_tempo() const { return static_cast<double>(sessao.passos) / 60.0; }
    std::uint64_t get_passos_sessao() const { return sessao.passos_anteriores + sessao.passos; }
    double get_tempo_sessao() const { return static_cast<double>(get_passos_sessao()) / 60.0; }
    void continuar_sessao(std::uint64_t anteriores) { sessao.passos_anteriores = anteriores; }
    int get_pontos() const { return pontuacao.pontos(); }
    void usar_grade(bool ativa) { gC.ativar_grade(ativa); }
    const Gerenciadores::Gerenciador_Colisoes::Estatisticas& medidas_colisao() const { return gC.estatisticas(); }
    double custo_simulacao() const { return micros_simulacao; }
    double custo_desenho() const { return micros_desenho; }
    void set_tempo_jogadores();
    void salvar(const std::filesystem::path& caminho = {});
    Persistencia::Json capturar();
    // Constroi listas temporarias e valida tudo antes de trocar o mundo atual.
    void restaurar(const Persistencia::Json& dados);
    bool registrar_resultado(const std::vector<std::string>& nomes);
    void ao_entrar() override;
    void tratar_evento(const sf::Event& evento) override;
    virtual void atualizar();
    virtual void fim_de_jogo();
    void simular_passo();
};
}
