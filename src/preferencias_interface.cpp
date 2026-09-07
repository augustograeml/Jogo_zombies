#include "../Interface/preferencias.h"
#include "../Persistencia/arquivo.h"
#include "../Recursos/catalogo.h"
#include <algorithm>
namespace Interface {
Preferencias& Preferencias::instancia() {
    static Preferencias p=[] {
        Preferencias n;
        try { if(std::filesystem::exists("interface.json")) {
            auto j=Persistencia::ler_json("interface.json");
            n.escala=Persistencia::numero(j.at("escala"),1,1.3);
            n.legivel=j.at("legivel").get<bool>(); n.contraste=j.at("contraste").get<bool>();
        }} catch(const std::exception&) { n=Preferencias{}; }
        return n;
    }(); return p;
}
void Preferencias::salvar() const {
    Persistencia::escrever_json("interface.json",{{"escala",escala},{"legivel",legivel},{"contraste",contraste}});
}
const sf::Font& fonte_legivel() {
    static sf::Font fonte=[] { sf::Font f;
        if(!f.loadFromFile(Recursos::caminho("Design/fonte/DejaVuSans.ttf").string()))
            throw std::runtime_error("Fonte legivel nao encontrada.");
        return f;
    }(); return fonte;
}
void aplicar_texto(sf::Text& t,float largura) {
    const auto& p=Preferencias::instancia();
    if(p.legivel) t.setFont(fonte_legivel());
    float s=p.escala;
    const auto w=t.getLocalBounds().width;
    if(w*s>largura && w>0) s=largura/w;
    t.setScale(s,s);
    if(p.contraste) { t.setFillColor(sf::Color::White); t.setOutlineColor(sf::Color::Black); t.setOutlineThickness(std::max(2.f,t.getOutlineThickness())); }
}
void configurar(int opcao) {
    auto& p=Preferencias::instancia(); const auto anterior=p;
    if(opcao==0) p.escala=p.escala<1.1f?1.15f:p.escala<1.2f?1.3f:1.f;
    if(opcao==1) p.legivel=!p.legivel;
    if(opcao==2) p.contraste=!p.contraste;
    try {p.salvar();} catch(...) {p=anterior; throw;}
}
}
