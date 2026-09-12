#include "../Audio/efeitos.h"
#include "../Interface/preferencias.h"
#include "../Interface/tema.h"
#include "../Recursos/catalogo.h"
#include "../Estados/Menus/pause.h"
namespace Estados::Menus {
Gerenciadores::Gerenciador_Grafico* Pause::pGG = Gerenciadores::Gerenciador_Grafico::get_instancia();
Pause::Pause(int id) : Estado(id), fonte(new sf::Font), imagem(new sf::Texture) { inicializa_valores(); }
Pause::~Pause() { delete fonte; delete imagem; }
void Pause::inicializa_valores() {
    imagem->loadFromFile(Recursos::caminho("Design/imagens/pause.png").string());
    fonte->loadFromFile(Recursos::caminho("Design/fonte/sangue_escorrendo.ttf").string());
    opcoes = {"Pausa", "Continuar", "Menu"};
    coordenadas = {{340, 185}, {390, 462}, {470, 605}};
    tamanhos = {150, 50, 50};
    textos.resize(3); pos = 1;
    for (std::size_t i = 0; i < textos.size(); ++i) {
        textos[i].setFont(*fonte); textos[i].setString(opcoes[i]);
        textos[i].setCharacterSize(tamanhos[i]); textos[i].setPosition(coordenadas[i]);
        textos[i].setOutlineColor(sf::Color::Black);
    }
    textos[0].setOutlineThickness(20); textos[1].setOutlineThickness(4);
}
void Pause::tratar_evento(const sf::Event& evento) {
    if (evento.type != sf::Event::KeyPressed) return;
    const auto tecla=evento.key.code;
    if(tecla==sf::Keyboard::F) { preferencias_abertas=!preferencias_abertas; return; }
    if(preferencias_abertas) {
        if(tecla==sf::Keyboard::Escape) { preferencias_abertas=false; return; }
        if(tecla==sf::Keyboard::Up) preferencia=(preferencia+5)%6;
        if(tecla==sf::Keyboard::Down) preferencia=(preferencia+1)%6;
        if(tecla==sf::Keyboard::Enter || tecla==sf::Keyboard::Right || tecla==sf::Keyboard::Left) {
            if(preferencia<3) Interface::configurar(preferencia);
            else Audio::configurar_categoria(static_cast<Audio::Categoria>(preferencia-3),tecla==sf::Keyboard::Left?-10:10);
        }
        return;
    }
    if (evento.key.code == sf::Keyboard::M || evento.key.code == sf::Keyboard::Add ||
        evento.key.code == sf::Keyboard::Equal || evento.key.code == sf::Keyboard::Subtract || evento.key.code == sf::Keyboard::Hyphen) {
        const bool reduzir = evento.key.code==sf::Keyboard::Subtract || evento.key.code==sf::Keyboard::Hyphen;
        Audio::configurar(evento.key.code==sf::Keyboard::M?0:reduzir?-10:10, evento.key.code==sf::Keyboard::M);
        return;
    }
    if (evento.key.code == sf::Keyboard::S) { pGE->salvar_partida(); return; }
    if (evento.key.code == sf::Keyboard::Up || evento.key.code == sf::Keyboard::Down) {
        textos[pos].setOutlineThickness(0); pos = pos == 1 ? 2 : 1; textos[pos].setOutlineThickness(4);
    } else if (evento.key.code == sf::Keyboard::Escape || evento.key.code == sf::Keyboard::Enter) {
        pGE->mensagem.clear();
        pGE->set_estado_atual(evento.key.code == sf::Keyboard::Escape || pos == 1 ? pGE->get_fase() : 0);
    }
}
void Pause::mostrar_menu() {
    pGG->resetarCamera(); pGG->desenharTextura(imagem);
    auto& janela=*pGG->get_Janela();
    if(Interface::Preferencias::instancia().contraste || preferencias_abertas) { sf::RectangleShape fundo({1024,1024}); fundo.setFillColor(sf::Color::Black); janela.draw(fundo); }
    Interface::Tema::painel(janela,{55,55,914,880});
    if(preferencias_abertas) {
        const auto& p=Interface::Preferencias::instancia(); const auto& a=Audio::Preferencias::instancia();
        const std::vector<std::string> linhas={
            "Escala: "+std::to_string(static_cast<int>(p.escala*100))+"%",
            std::string("Fonte legivel: ")+(p.legivel?"sim":"nao"),
            std::string("Alto contraste: ")+(p.contraste?"sim":"nao"),
            "Movimento e coleta: "+std::to_string(static_cast<int>(a.volumes[0]))+"%",
            "Combate: "+std::to_string(static_cast<int>(a.volumes[1]))+"%",
            "Vitoria e derrota: "+std::to_string(static_cast<int>(a.volumes[2]))+"%"};
        auto escrever=[&](std::string valor,float y,unsigned tamanho) { sf::Text t(valor,Interface::fonte_legivel(),tamanho); t.setPosition(90,y); Interface::aplicar_texto(t,840); janela.draw(t); };
        escrever("Preferencias",90,42);
        for(std::size_t i=0;i<linhas.size();++i) Interface::Tema::botao(janela,{80,225+80.f*i,864,60},linhas[i],preferencia==static_cast<int>(i),24);
        escrever("Setas: selecionar/ajustar | Enter: alterar",810,22);
        escrever("Esc ou F: voltar | Preferencias salvas automaticamente",860,20);
        return;
    }
    Interface::Tema::titulo(janela,"Pausa",125);
    Interface::Tema::botao(janela,{320,440,384,60},"Continuar",pos==1,26);
    Interface::Tema::botao(janela,{320,535,384,60},"Menu",pos==2,26);
    const auto& p=Audio::Preferencias::instancia();
    sf::Text ajuda("F: preferencias | S: salvar | M: mudo | +/-: volume " + std::to_string(static_cast<int>(p.volume)) +
        (p.mudo?" (mudo)":""), Interface::fonte_legivel(), 20);
    ajuda.setPosition(80,800); pGG->get_Janela()->draw(ajuda);
}
void Pause::executar() { mostrar_menu(); }
void Pause::loop_evento() {}
}
