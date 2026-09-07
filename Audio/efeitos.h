#pragma once
#include "../Logica/eventos.h"
#include <SFML/Audio.hpp>
#include <array>
#include <memory>
namespace Audio {
// Buffers sintetizados localmente em C++; nenhum arquivo de audio externo necessario.
enum class Categoria { Movimento, Combate, Resultado };
Categoria categoria(Logica::Evento evento);
class Efeitos : public Logica::Observador {
    std::array<sf::SoundBuffer, 8> buffers;
    std::array<sf::Sound, 12> vozes;
    std::size_t proxima = 0;
    std::array<Categoria,12> categorias{};
    Efeitos();
public:
    static Efeitos& instancia();
    void receber(const Logica::Notificacao& e) override;
    void parar();
    void aplicar_volume();
};
struct Preferencias {
    float volume = 60;
    bool mudo = false;
    std::array<float,3> volumes{{100,100,100}};
    float volume_evento(Logica::Evento e) const;
    static Preferencias& instancia();
    void salvar() const;
};
// Nao inicializa dispositivo de audio em testes ou ao carregar um save.
void habilitar();
void interromper();
void desligar();
void configurar_categoria(Categoria categoria, float variacao);
void configurar(float variacao, bool alternar_mudo);
void publicar(const Logica::Notificacao& e);
class ObservadorSom : public Logica::Observador {
public:
    void receber(const Logica::Notificacao& e) override { publicar(e); }
};
}
