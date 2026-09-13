#include "../Estados/Fases/fase1.h"
#include "../Estados/Fases/fase2.h"
#include "../Logica/mundo.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>

using Entidades::Personagens::Jogador;

// Injeta somente teclas logicas. Posicao, velocidade e apoio sao atualizados
// pelas mesmas funcoes do jogador real; nao ha teleporte entre patamares.
class JogadorDeTeste : public Jogador {
public:
    float destino = 0;
    JogadorDeTeste(sf::Vector2f inicio, bool segundo) : Jogador(inicio, {0,0}, segundo) {}
    bool apoiado() const { return nochao; }
    void executar() override {
        if (!get_vivo()) return;
        const float vx = getVelocidade().x;
        const float erro = destino - getPosicao().x - vx * std::abs(vx) / .4f;
        mover_com_controles(erro < -3, erro > 3, true, false);
    }
};

template<class Base> class Percurso : public Base {
    void fotografar(const std::string& ponto) {
        if (!std::getenv("ZOMBIES_CAPTURAR_PERCURSO")) return;
        const auto pasta = std::filesystem::temp_directory_path() / "zombies-percurso-continuo";
        std::filesystem::create_directories(pasta);
        auto* janela = Gerenciadores::Gerenciador_Grafico::get_instancia()->get_Janela();
        const auto antes = this->capturar();
        janela->clear(); this->desenhar_partida(); janela->display();
        assert(this->capturar() == antes);
        sf::Texture pixels;
        assert(pixels.create(janela->getSize().x, janela->getSize().y));
        pixels.update(*janela);
        const auto nome = "fase" + std::to_string(this->get_numero_fase()) +
            (this->num_jogadores == 2 ? "-dupla-" : "-solo-") + ponto + ".png";
        assert(pixels.copyToImage().saveToFile((pasta / nome).string()));
    }
public:
    explicit Percurso(int id) : Base(id, false) {
        std::vector<sf::Vector2f> inicios;
        for (auto it = this->jogadores.get_primeiro(); it != nullptr; ++it)
            inicios.push_back((*it)->getPosicao());
        this->jogadores.limpar();
        for (int i = static_cast<int>(inicios.size()) - 1; i >= 0; --i)
            this->jogadores.incluir(new JogadorDeTeste(inicios[i], i != 0));
        // Navegacao isolada da IA: combate permanece nos testes de integracao.
        // Curas desativadas impedem que uma coleta esconda dano de espinhos.
        this->inimigos.limpar();
        for (auto it = this->obstaculos.get_primeiro(); it != nullptr; ++it)
            if (dynamic_cast<Entidades::Obstaculos::Coracao*>(*it)) (*it)->morrer();
        this->ao_entrar();
    }

    void verificar() {
        const std::vector<sf::Vector2f> caminho = this->get_numero_fase() == 1 ?
            std::vector<sf::Vector2f>{{520,900},{760,800},{960,700},{1130,800},{1400,700},
                {1710,700},{1960,800},{2160,700},{2400,800},{2660,700},{2890,700},
                {3000,700},{3260,800},{3600,800},{3900,800}} :
            std::vector<sf::Vector2f>{{520,900},{760,800},{1050,800},{1370,850},{1540,750},
                {1790,850},{2050,750},{2250,650},{2400,750},{2660,650},{2890,650},
                {3030,650},{3260,750},{3570,750},{3900,750}};
        std::vector<std::size_t> etapas(this->num_jogadores, 0);
        fotografar("inicio");
        for (unsigned passo = 0; passo < 5000; ++passo) {
            unsigned indice = 0;
            bool terminou = true;
            for (auto it = this->jogadores.get_primeiro(); it != nullptr; ++it, ++indice) {
                auto* j = static_cast<JogadorDeTeste*>(*it);
                auto& etapa = etapas[indice];
                if (etapa == caminho.size()) continue;
                const float separacao = this->num_jogadores == 2 ? (indice ? 20.f : -20.f) : 0.f;
                auto alvo = caminho[etapa]; alvo.x += separacao;
                if (j->apoiado() && std::abs(j->getPosicao().x - alvo.x) < 30 &&
                    j->getPosicao().y + j->getTamanho().y <= alvo.y + 1) ++etapa;
                if (etapa == caminho.size()) continue;
                terminou = false;
                j->destino = caminho[etapa].x + separacao;
            }
            if (terminou) {
                fotografar("final");
                std::cout << "Percurso continuo fase " << this->get_numero_fase()
                    << (this->num_jogadores == 2 ? " dupla: " : " solo: ")
                    << passo << " passos, todos os pontos e nenhum dano.\n";
                return;
            }
            auto contexto = this->mundo();
            // Sem inimigos, a indicacao de vitoria nao encerra este ensaio de navegacao.
            const auto fim = Logica::simular(contexto);
            if (fim == Logica::FimPasso::Derrota) throw std::runtime_error("Derrota durante a navegacao.");
            for (auto it = this->jogadores.get_primeiro(); it != nullptr; ++it) {
                assert((*it)->get_vivo() && (*it)->get_vida() == 20);
                assert((*it)->getPosicao().y < this->limites.top + this->limites.height);
            }
            if (passo == 1000) fotografar("meio");
        }
        throw std::runtime_error("Percurso continuo nao alcancou a arena no prazo.");
    }
};

int main() {
    for (int id : {6,7}) { Percurso<Estados::Fases::Fase1> fase(id); fase.verificar(); }
    for (int id : {8,9}) { Percurso<Estados::Fases::Fase2> fase(id); fase.verificar(); }
}
