#include "../Estados/Fases/fase1.h"
#include "../Interface/preferencias.h"
#include "../Animacao/pose.h"
#include "../Persistencia/entidades.h"
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
