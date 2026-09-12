//codigo reaproveitado do Peteco

#include "../Gerenciadores/gerenciador_grafico.h"
#include "../ente.h"
#include <iostream>
#include "../Interface/camera.h"

namespace Gerenciadores
{


    Gerenciador_Grafico::Gerenciador_Grafico() :
        camera(sf::Vector2f(512,512),sf::Vector2f(1024,1024)) {}
    Gerenciador_Grafico::~Gerenciador_Grafico() = default;
    void Gerenciador_Grafico::encerrar() { janela.reset(); }
    void Gerenciador_Grafico::desenharEnte(Ente *pE)
    {
        if (pE)
            get_Janela()->draw(*pE->get_corpo());
    }
    void Gerenciador_Grafico::desenharTextura(sf::Texture* pT)
    {
        if (pT)
        {
            sf::Sprite sprite;
            sprite.setTexture(*pT);
            const auto tamanho=pT->getSize();
            if(!tamanho.x || !tamanho.y) return;
            const float escala=std::max(1024.f/tamanho.x,1024.f/tamanho.y);
            sprite.setScale(escala,escala);
            sprite.setPosition((1024-tamanho.x*escala)/2,(1024-tamanho.y*escala)/2);
            get_Janela()->draw(sprite);
        }
    }
    void Gerenciador_Grafico::desenharFundo(sf::RectangleShape* pR)
    {
        if (pR)
        {
            get_Janela()->draw(*pR);
        }
    }
    void Gerenciador_Grafico::desenharTexto(sf::Text* pT)
    {
        if (pT)
        {
            get_Janela()->draw(*pT);
        }
    }
    bool Gerenciador_Grafico::get_JanelaAberta() const
    {
        return janela && janela->isOpen();
    }
    Gerenciador_Grafico *Gerenciador_Grafico::get_instancia()
    {
        static Gerenciador_Grafico instancia;
        return &instancia;
    }
    void Gerenciador_Grafico::mostrar()
    {
        if (get_JanelaAberta())
            janela->display();
    }
    void Gerenciador_Grafico::limpar()
    {
        if (get_JanelaAberta())
            janela->clear();
    }
    void Gerenciador_Grafico::fecharJanela()
    {
        if (get_JanelaAberta())
            janela->close();       
    }
    sf::RenderWindow *Gerenciador_Grafico::get_Janela() const
    {
        if(!janela) {
            janela=std::make_unique<sf::RenderWindow>(sf::VideoMode(LARGURA_TELA,ALTURA_TELA),"zombies++");
            janela->setFramerateLimit(60);
            janela->setKeyRepeatEnabled(false);
            get_Janela()->setView(camera);
        }
        return janela.get();
    }
    sf::View Gerenciador_Grafico::get_view_interface() const {
        return Interface::vista_interface(get_Janela()->getSize());
    }
    void Gerenciador_Grafico::resetarCamera()
    {
        camera=get_view_interface();
        get_Janela()->setView(camera);
    }
    void Gerenciador_Grafico::centralizarCamera(sf::Vector2f p)//Entidades:Personagens:Jogador* pJ1, Entidades:Personagens:Jogador* pJ2)
    {        
        if(p.x < 550 && p.y < 500)
            camera.setCenter(550.f,500.f);
        else if(p.x > 1200 && p.y < 500)
            camera.setCenter(1200.f,500.f);
        else if(p.x < 550 && p.y > 600)
            camera.setCenter(550.f,600.f);
        else if(p.x > 1200 && p.y > 600)
            camera.setCenter(1200.f,600.f);
        else if(p.y > 600)
            camera.setCenter(p.x,600);
        else if(p.y < 500)
            camera.setCenter(p.x,500);
        else if(p.x > 1200)
            camera.setCenter(1200.f,p.y);
        else if(p.x < 550)
            camera.setCenter(550.f,p.y);
        else
            camera.setCenter(p);
           
        get_Janela()->setView(camera);        
    }
}
