#include "../Estados/Fases/fase1.h"
#include "../Interface/preferencias.h"
#include "../Animacao/pose.h"
#include "../Persistencia/entidades.h"
#include "../Recursos/catalogo.h"
#include <cassert>
#include <iostream>
class Cena: public Estados::Fases::Fase1 {
public:
    Cena():Fase1(7,false) {}
    void separar(bool vertical) {
        unsigned i=0;
        for(auto it=jogadores.get_primeiro();it!=nullptr;++it,++i) {
            (*it)->setPosicao(i?sf::Vector2f(vertical?100:1800,vertical?1000:500):sf::Vector2f(100,500));
        }
    }
    void mostrar() { desenhar_partida(); }
};
int main() {
    assert(Animacao::pose_jogador(0,-2,0)==Animacao::Pose::Subida);
    assert(Animacao::pose_jogador(0,2,0)==Animacao::Pose::Queda);
    assert(Animacao::pose_jogador(1,0,12)==Animacao::Pose::Dano);
    auto* gg=Gerenciadores::Gerenciador_Grafico::get_instancia(); auto* janela=gg->get_Janela();
    Cena cena; cena.ao_entrar(); cena.separar(false);
    auto& preferencias=Interface::Preferencias::instancia(); preferencias.escala=1.3f; preferencias.legivel=true; preferencias.contraste=true;
    const auto antes=cena.capturar();
    for(auto tamanho:{sf::Vector2u(1024,1024),sf::Vector2u(1280,720),sf::Vector2u(640,800)}) {
        janela->setSize(tamanho); janela->clear(); cena.mostrar(); janela->display();
        assert(cena.capturar()==antes);
        sf::Texture t; assert(t.create(tamanho.x,tamanho.y)); t.update(*janela);
        assert(t.copyToImage().saveToFile("/tmp/zombies-camera-"+std::to_string(tamanho.x)+".png"));
    }
    // Fundo deve cobrir cada viewport mesmo fora de qualquer limite do mapa.
    sf::Image original; original.create(320,180,sf::Color(71,83,92));
    sf::Texture textura; assert(textura.loadFromImage(original));
    sf::RenderTexture alvo; assert(alvo.create(640,480));
    Interface::CameraDupla camera;
    for(const auto& pontos:std::vector<std::vector<sf::Vector2f>>{
        {{-2000,-3000}},{{9000,7000}},{{-2000,-3000},{9000,-3000}},
        {{100,-3000},{100,7000}},{{100,100},{200,100}}}) {
        const auto vistas=camera.atualizar(pontos,{0,0,2000,1200},190,{640,480});
        alvo.clear(sf::Color::Magenta);
        for(const auto& vista:vistas) { alvo.setView(vista); Recursos::desenhar_cenario(alvo,textura); }
        alvo.display(); const auto imagem=alvo.getTexture().copyToImage();
        for(const auto& vista:vistas) {
            const auto r=alvo.getViewport(vista);
            for(int y=r.top+1;y<r.top+r.height-1;++y)
                for(int x=r.left+1;x<r.left+r.width-1;++x)
                    assert(imagem.getPixel(x,y)==sf::Color(71,83,92));
        }
    }
    Entidades::Personagens::Gigante gigante;
    gigante.receber_dano(10); assert(gigante.get_reacao()==12);
    auto salvo=Persistencia::Serializador::salvar(gigante);
    auto restaurado=Persistencia::Serializador::carregar(salvo);
    assert(restaurado->get_reacao()==12);
    auto legado=salvo; legado.erase("reacao_visual");
    assert(Persistencia::Serializador::carregar(legado)->get_reacao()==0);
    bool rejeitou=false; auto invalido=salvo; invalido["reacao_visual"]=13;
    try { Persistencia::Serializador::carregar(invalido); } catch(...) { rejeitou=true; }
    assert(rejeitou);
    for(int i=0;i<12;++i) gigante.avancar_animacao();
    assert(gigante.get_reacao()==0);
    std::cout << "Apresentacao: camera/HUD em tres resolucoes sem mutar mundo; poses e retomada de reacao OK\n";
}
