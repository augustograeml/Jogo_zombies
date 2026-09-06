#pragma once
#include "../Logica/eventos.h"
#include <SFML/Audio.hpp>
#include <array>
#include <memory>
namespace Audio {
// Buffers sintetizados localmente em C++; nenhum arquivo de audio externo necessario.
class Efeitos : public Logica::Observador {
    std::array<sf::SoundBuffer, 6> buffers;
    std::array<sf::Sound, 12> vozes;
    std::size_t proxima = 0;
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
    static Preferencias& instancia();
    void salvar() const;
};
// Nao inicializa dispositivo de audio em testes ou ao carregar um save.
void habilitar();
void interromper();
void configurar(float variacao, bool alternar_mudo);
void publicar(const Logica::Notificacao& e);
class ObservadorSom : public Logica::Observador {
public:
    void receber(const Logica::Notificacao& e) override { publicar(e); }
};
}
