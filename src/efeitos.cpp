#include "../Audio/sintese.h"
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
            if(j.contains("categorias")) for(std::size_t i=0;i<3;++i) novo.volumes[i]=Persistencia::numero(j.at("categorias").at(i),0,100);
        } } catch(const std::exception& erro) {
            std::cerr << "Preferencias de audio invalidas; usando valores padrao: " << erro.what() << '\n';
            novo=Preferencias{};
        }
        return novo;
    }();
    return p;
}
void Preferencias::salvar() const { Persistencia::escrever_json("preferencias.json", {{"volume", volume}, {"mudo", mudo}, {"categorias",volumes}}); }
Categoria categoria(Logica::Evento e) {
    if(e==Logica::Evento::Vitoria || e==Logica::Evento::Derrota) return Categoria::Resultado;
    if(e==Logica::Evento::Salto || e==Logica::Evento::Coleta) return Categoria::Movimento;
    return Categoria::Combate;
}
float Preferencias::volume_evento(Logica::Evento e) const { return mudo?0:volume*volumes[static_cast<std::size_t>(categoria(e))]/100.f; }
Efeitos::Efeitos() {
    constexpr unsigned taxa = 22050;
    for (std::size_t n = 0; n < buffers.size(); ++n) {
        const auto amostras = sintetizar(static_cast<Logica::Evento>(n),taxa);
        if (!buffers[n].loadFromSamples(amostras.data(), amostras.size(), 1, taxa))
            throw std::runtime_error("Nao foi possivel preparar os efeitos sonoros.");
    }
}
Efeitos& Efeitos::instancia() { static Efeitos e; ativos=&e; return e; }
void Efeitos::receber(const Logica::Notificacao& e) {
    auto& p=Preferencias::instancia();
    if (p.mudo || p.volume == 0) return;
    const auto indice=proxima++ % vozes.size();
    categorias[indice]=categoria(e.tipo);
    auto& voz=vozes[indice]; voz.stop();
    voz.setBuffer(buffers.at(static_cast<std::size_t>(e.tipo))); voz.setVolume(p.volume_evento(e.tipo)); voz.play();
}
void Efeitos::parar() { for (auto& voz:vozes) voz.stop(); }
void Efeitos::aplicar_volume() { auto& p=Preferencias::instancia(); for(std::size_t i=0;i<vozes.size();++i) vozes[i].setVolume(p.mudo?0:p.volume*p.volumes[static_cast<std::size_t>(categorias[i])]/100.f); }
void habilitar() { Preferencias::instancia(); habilitado=true; }
void desligar() { habilitado=false; interromper(); }
void configurar_categoria(Categoria c,float variacao) {
    auto& p=Preferencias::instancia(); const auto anterior=p;
    auto& v=p.volumes.at(static_cast<std::size_t>(c)); v=std::clamp(v+variacao,0.f,100.f);
    try { p.salvar(); } catch(...) {p=anterior; throw;}
    if(ativos) ativos->aplicar_volume();
}
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
