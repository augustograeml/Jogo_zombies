#include "../Animacao/articulada.h"
#include <algorithm>
#include <cmath>
namespace Animacao {
namespace {
float suave(float inicio, float fim, float valor) {
    const float t = std::clamp((valor-inicio)/(fim-inicio),0.f,1.f);
    return t*t*(3.f-2.f*t);
}
}
void desenhar_articulada(sf::RenderTarget& alvo, const sf::Texture& textura,
                        sf::FloatRect caixa, const Estado& estado, sf::Color cor) {
    constexpr unsigned colunas=18, linhas=24;
    const auto tamanho=textura.getSize();
    const float altura=caixa.height, largura=altura*tamanho.x/tamanho.y;
    const float fase=6.283185307f*(estado.quadro+estado.passos/2.f)/Recursos::Configuracao::quadros_corrida;
    const float passo=estado.correndo?std::sin(fase):0.f;
    const auto vertice=[&](unsigned coluna,unsigned linha) {
        const float u=static_cast<float>(coluna)/colunas,v=static_cast<float>(linha)/linhas;
        // Duas pernas em oposicao: a perna levantada da arte desce durante
        // o apoio e a traseira faz o movimento contrario. Cabeca e tronco ficam fixos.
        float x=(u-.5f)*largura, y=(v-1.f)*altura;
        const auto girar=[&](float pu,float pv,float angulo,float peso) {
            const float px=(pu-.5f)*largura,py=(pv-1.f)*altura;
            const float dx=(u-.5f)*largura-px,dy=(v-1.f)*altura-py;
            x+=peso*(px+dx*std::cos(angulo)-dy*std::sin(angulo)-(u-.5f)*largura);
            y+=peso*(py+dx*std::sin(angulo)+dy*std::cos(angulo)-(v-1.f)*altura);
        };
        if(estado.correndo) {
            const float frente=suave(.49f,.72f,u)*suave(.37f,.49f,v)*(1.f-suave(.68f,.80f,v));
            const float tras=suave(.66f,.85f,v)*(1.f-suave(.48f,.63f,u));
            girar(.50f,.62f,.55f+.60f*passo,frente);
            girar(.48f,.65f,-.45f*passo,tras);
            const float braco=(1.f-suave(.20f,.40f,u))*suave(.40f,.56f,v)*(1.f-suave(.68f,.78f,v));
            girar(.30f,.48f,-.20f*passo,braco);
        }
        if (!estado.direita) x=-x;
        return sf::Vertex({caixa.left+caixa.width/2.f+x,caixa.top+caixa.height+y},cor,
                          {u*tamanho.x,v*tamanho.y});
    };
    sf::VertexArray malha(sf::Triangles,colunas*linhas*6);
    unsigned indice=0;
    for(unsigned y=0;y<linhas;++y) for(unsigned x=0;x<colunas;++x) {
        malha[indice++]=vertice(x,y); malha[indice++]=vertice(x+1,y); malha[indice++]=vertice(x,y+1);
        malha[indice++]=vertice(x+1,y); malha[indice++]=vertice(x+1,y+1); malha[indice++]=vertice(x,y+1);
    }
    sf::RenderStates estados; estados.texture=&textura;
    alvo.draw(malha,estados);
}
}
