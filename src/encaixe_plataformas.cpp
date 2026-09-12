#include "../Recursos/plataforma.h"
#include "../Recursos/catalogo.h"
#include "../Listas/listaEntidades.h"
#include "../Entidades/Obstaculos/neve.h"
#include "../Entidades/Obstaculos/musgo.h"
#include <map>
#include <tuple>

namespace Recursos {
namespace {
int material(Entidades::Entidade* e) {
    if(dynamic_cast<Entidades::Obstaculos::Neve*>(e)) return 1;
    if(dynamic_cast<Entidades::Obstaculos::Musgo*>(e)) return 2;
    return 0;
}
const sf::Texture& tile(int tipo,int variante,unsigned vizinhos) {
    // No maximo 2 materiais x 6 trechos x 16 encaixes, criados sob demanda.
    static std::map<std::tuple<int,int,unsigned>,sf::Texture> cache;
    const auto chave=std::make_tuple(tipo,variante,vizinhos);
    if(auto it=cache.find(chave);it!=cache.end()) return it->second;
    static const sf::Image neve=textura("Design/imagens/plataforma.png")->copyToImage();
    static const sf::Image musgo=textura("Design/imagens/bloco_musgo.png")->copyToImage();
    const auto& fonte=tipo==1?neve:musgo;
    sf::Image imagem; imagem.create(128,128);
    const bool coberto=vizinhos&1, direita=vizinhos&2, abaixo=vizinhos&4, esquerda=vizinhos&8;
    for(unsigned y=0;y<128;++y) for(unsigned x=0;x<128;++x) {
        const unsigned sx=variante<3?variante*256+x*2:(6-variante)*256-1-x*2;
        // Neve/musgo somente no topo exposto. Paredes usam o corpo da pedra.
        const unsigned sy=coberto?48+y*207/127:y*2;
        auto cor=fonte.getPixel(sx,sy);
        // As duas faces conectadas terminam na mesma cor de pedra. Uma faixa
        // curta mistura a borda, eliminando o corte horizontal da textura.
        const float mistura=coberto && y<6?(6-y)/6.f:abaixo && y>121?(y-121)/6.f:0.f;
        if(mistura>0) {
            const auto a=fonte.getPixel(sx,48),b=fonte.getPixel(sx,255);
            cor.r=static_cast<sf::Uint8>(cor.r*(1-mistura)+(a.r+b.r)*.5f*mistura);
            cor.g=static_cast<sf::Uint8>(cor.g*(1-mistura)+(a.g+b.g)*.5f*mistura);
            cor.b=static_cast<sf::Uint8>(cor.b*(1-mistura)+(a.b+b.b)*.5f*mistura);
        }
        float luz=1.f;
        if(!esquerda && x<5) luz*=1.12f;
        if(!direita && x>122) luz*=.72f;
        if(!abaixo && y>121) luz*=.68f;
        for(auto* canal:{&cor.r,&cor.g,&cor.b}) *canal=static_cast<sf::Uint8>(std::min(255.f,*canal*luz));
        imagem.setPixel(x,y,cor);
    }
    sf::Texture resultado;
    if(!resultado.loadFromImage(imagem)) throw std::runtime_error("Falha ao compor plataforma.");
    return cache.emplace(chave,std::move(resultado)).first->second;
}
}
void encaixar_plataformas(Listas::ListaEntidade& blocos) {
    using Celula=std::tuple<int,float,float>;
    std::map<Celula,Entidades::Entidade*> grade;
    for(auto it=blocos.get_primeiro();it!=nullptr;++it) {
        auto* e=*it; const int tipo=material(e); const auto* forma=e->get_corpo();
        // Saves com geometria customizada conservam seu desenho independente.
        if(tipo && e->get_vivo() && forma->getSize()==sf::Vector2f(50,50) &&
           forma->getScale()==sf::Vector2f(1,1) && forma->getRotation()==0 && forma->getOrigin()==sf::Vector2f()) {
            const auto p=forma->getPosition(); grade[{tipo,p.x,p.y}]=e;
        }
    }
    for(const auto& [celula,e]:grade) {
        const auto [tipo,x,y]=celula;
        const unsigned mascara=(grade.count({tipo,x,y-50})?1:0) | (grade.count({tipo,x+50,y})?2:0) |
                               (grade.count({tipo,x,y+50})?4:0) | (grade.count({tipo,x-50,y})?8:0);
        const int coluna=static_cast<int>(std::floor(x/50));
        const int variante=(coluna%6+6)%6; // Mesma faixa ao longo de toda a coluna.
        auto* forma=e->get_corpo();
        forma->setTexture(&tile(tipo,variante,mascara),true);
    }
}
}
