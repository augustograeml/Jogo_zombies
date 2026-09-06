#include "../Recursos/catalogo.h"
#include <algorithm>
#include <map>
#include <stdexcept>
namespace Recursos {
const std::vector<Quadro>& caminhada(Animado tipo) {
    static std::map<Animado, std::vector<Quadro>> cache;
    const auto pronto=cache.find(tipo);
    if(pronto!=cache.end()) return pronto->second;
    const char* nome=tipo==Animado::Zumbi?"imagens/zumbi_corrida_atlas.png":
        tipo==Animado::Arqueiro?"imagens/arqueiro_corrida_atlas.png":"imagens/gigante_corrida_atlas.png";
    const auto atlas=textura(nome); auto imagem=atlas->copyToImage(); const auto tamanho=imagem.getSize();
    // Alguns exportadores gravam o quadriculado de transparencia em RGB.
    // Tratar apenas o fundo neutro conectado a borda; preservar brancos internos
    // (olhos, luvas, flechas) protegidos pelos contornos da propria arte.
    bool possui_alpha=false;
    for(unsigned y=0;y<tamanho.y && !possui_alpha;++y) for(unsigned x=0;x<tamanho.x;++x)
        if(imagem.getPixel(x,y).a==0) { possui_alpha=true; break; }
    if(!possui_alpha) {
        std::vector<unsigned> fundo;
        const auto incluir=[&](unsigned x,unsigned y) {
            const auto c=imagem.getPixel(x,y);
            const int menor=std::min({c.r,c.g,c.b}),maior=std::max({c.r,c.g,c.b});
            if(c.a && menor>=200 && maior-menor<=16) {
                imagem.setPixel(x,y,sf::Color::Transparent); fundo.push_back(y*tamanho.x+x);
            }
        };
        for(unsigned x=0;x<tamanho.x;++x) { incluir(x,0); incluir(x,tamanho.y-1); }
        for(unsigned y=0;y<tamanho.y;++y) { incluir(0,y); incluir(tamanho.x-1,y); }
        for(std::size_t i=0;i<fundo.size();++i) {
            const auto x=fundo[i]%tamanho.x,y=fundo[i]/tamanho.x;
            if(x) incluir(x-1,y);
            if(x+1<tamanho.x) incluir(x+1,y);
            if(y) incluir(x,y-1);
            if(y+1<tamanho.y) incluir(x,y+1);
        }
    }
    // Os desenhos ultrapassam as celulas nominalmente iguais. Identificar cada
    // silhueta pela transparencia evita cortar membros ou mostrar o vizinho.
    struct Regiao { int id; unsigned x0,y0,x1,y1; std::size_t area; };
    std::vector<int> rotulos(static_cast<std::size_t>(tamanho.x)*tamanho.y,-1);
    std::vector<Regiao> regioes; std::vector<unsigned> fila;
    for(unsigned y=0;y<tamanho.y;++y) for(unsigned x=0;x<tamanho.x;++x) {
        const unsigned inicio=y*tamanho.x+x;
        if(rotulos[inicio]!=-1 || imagem.getPixel(x,y).a<=8) continue;
        const int id=static_cast<int>(regioes.size()); Regiao r{id,x,y,x,y,0};
        fila.clear(); fila.push_back(inicio); rotulos[inicio]=id;
        for(std::size_t i=0;i<fila.size();++i) {
            const unsigned atual=fila[i], px=atual%tamanho.x,py=atual/tamanho.x;
            r.x0=std::min(r.x0,px); r.x1=std::max(r.x1,px); r.y0=std::min(r.y0,py); r.y1=std::max(r.y1,py); ++r.area;
            for(int dy=-1;dy<=1;++dy) for(int dx=-1;dx<=1;++dx) {
                const int nx=static_cast<int>(px)+dx,ny=static_cast<int>(py)+dy;
                if(nx<0 || ny<0 || nx>=static_cast<int>(tamanho.x) || ny>=static_cast<int>(tamanho.y)) continue;
                const unsigned n=static_cast<unsigned>(ny)*tamanho.x+static_cast<unsigned>(nx);
                if(rotulos[n]==-1 && imagem.getPixel(nx,ny).a>8) { rotulos[n]=id; fila.push_back(n); }
            }
        }
        regioes.push_back(r);
    }
    std::sort(regioes.begin(),regioes.end(),[](const Regiao& a,const Regiao& b){return a.area>b.area;});
    if(regioes.size()<8 || regioes[7].area<1000) throw std::runtime_error(std::string("Atlas sem oito silhuetas completas: ")+nome);
    regioes.resize(8);
    std::sort(regioes.begin(),regioes.end(),[&](const Regiao& a,const Regiao& b){
        const bool linha_a=(a.y0+a.y1)/2>=tamanho.y/2,linha_b=(b.y0+b.y1)/2>=tamanho.y/2;
        return linha_a!=linha_b?linha_a<linha_b:a.x0+a.x1<b.x0+b.x1;
    });
    std::vector<Quadro> quadros;
    for(const auto& r:regioes) {
        sf::Image recorte; recorte.create(r.x1-r.x0+1,r.y1-r.y0+1,sf::Color::Transparent);
        for(unsigned y=r.y0;y<=r.y1;++y) for(unsigned x=r.x0;x<=r.x1;++x)
            if(rotulos[y*tamanho.x+x]==r.id) recorte.setPixel(x-r.x0,y-r.y0,imagem.getPixel(x,y));
        auto quadro=std::make_shared<sf::Texture>();
        if(!quadro->loadFromImage(recorte)) throw std::runtime_error(std::string("Falha ao preparar quadro: ")+nome);
        quadros.push_back({std::move(quadro),{0,0,static_cast<int>(recorte.getSize().x),static_cast<int>(recorte.getSize().y)}});
    }
    return cache.emplace(tipo,std::move(quadros)).first->second;
}
}
