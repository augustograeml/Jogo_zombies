#include "../Audio/efeitos.h"
#include "../Persistencia/arquivo.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>
namespace Audio {
namespace { bool habilitado = false; Efeitos* ativos = nullptr; }
Preferencias& Preferencias::instancia() {
    static Preferencias p = [] {
        Preferencias novo;
        try { if (std::filesystem::exists("preferencias.json")) {
            auto j = Persistencia::ler_json("preferencias.json");
            novo.volume = static_cast<float>(Persistencia::numero(j.at("volume"), 0, 100));
            novo.mudo = j.at("mudo").get<bool>();
        } } catch(const std::exception& erro) {
            std::cerr << "Preferencias de audio invalidas; usando valores padrao: " << erro.what() << '\n';
            novo=Preferencias{};
        }
        return novo;
    }();
    return p;
}
void Preferencias::salvar() const { Persistencia::escrever_json("preferencias.json", {{"volume", volume}, {"mudo", mudo}}); }
Efeitos::Efeitos() {
    constexpr unsigned taxa = 22050;
    const float frequencias[] = {440, 180, 100, 880, 240, 660};
    for (std::size_t n = 0; n < buffers.size(); ++n) {
        const unsigned tamanho = n == 5 ? taxa / 2 : taxa / 7;
        std::vector<sf::Int16> amostras(tamanho);
        for (unsigned i=0; i<tamanho; ++i) {
            const double t=static_cast<double>(i)/taxa, envelope=1.-static_cast<double>(i)/tamanho;
            amostras[i]=static_cast<sf::Int16>(6500*envelope*std::sin(6.28318530718*frequencias[n]*t));
        }
        if (!buffers[n].loadFromSamples(amostras.data(), amostras.size(), 1, taxa))
            throw std::runtime_error("Nao foi possivel preparar os efeitos sonoros.");
    }
}
Efeitos& Efeitos::instancia() { static Efeitos e; ativos=&e; return e; }
void Efeitos::receber(const Logica::Notificacao& e) {
    auto& p=Preferencias::instancia();
    if (p.mudo || p.volume == 0) return;
    auto& voz=vozes[proxima++ % vozes.size()]; voz.stop();
    voz.setBuffer(buffers.at(static_cast<std::size_t>(e.tipo))); voz.setVolume(p.volume); voz.play();
}
void Efeitos::parar() { for (auto& voz:vozes) voz.stop(); }
void Efeitos::aplicar_volume() { auto& p=Preferencias::instancia(); for(auto& v:vozes) v.setVolume(p.mudo?0:p.volume); }
void habilitar() { Preferencias::instancia(); habilitado=true; }
void interromper() { if (ativos) ativos->parar(); }
void publicar(const Logica::Notificacao& e) {
    if (!habilitado) return;
    try { Efeitos::instancia().receber(e); }
    catch(const std::exception& erro) {
        habilitado=false;
        std::cerr << "Audio indisponivel: " << erro.what() << '\n';
    }
}
void configurar(float variacao, bool alternar) {
    auto& p=Preferencias::instancia();
    const auto anterior=p;
    p.volume=std::clamp(p.volume+variacao,0.f,100.f); if(alternar) p.mudo=!p.mudo;
    try { p.salvar(); } catch (...) { p=anterior; throw; }
    if (ativos) ativos->aplicar_volume();
}
}
