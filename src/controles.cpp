#include "../Interface/controles.h"
#include "../Interface/tema.h"
#include "../Interface/preferencias.h"
#include "../Persistencia/arquivo.h"
#include <set>
namespace Interface {
void ConfiguracaoControles::validar() const {
    std::set<int> usadas;
    for(const auto& jogador:teclas)for(int t:jogador)
        if(t<0 || t>=sf::Keyboard::KeyCount || t==sf::Keyboard::Escape || !usadas.insert(t).second)
            throw std::runtime_error("Tecla reservada ou ja usada por outra acao.");
    for(auto b:botoes)if(b>=sf::Joystick::ButtonCount)throw std::runtime_error("Botao invalido.");
}
ConfiguracaoControles ConfiguracaoControles::carregar() {
    ConfiguracaoControles c;
    if(std::filesystem::exists("controles.json")) {
        auto j=Persistencia::ler_json("controles.json");
        c.teclas=j.at("teclas").get<decltype(c.teclas)>();c.botoes=j.at("botoes").get<decltype(c.botoes)>();
        c.personalizado=j.at("personalizado").get<decltype(c.personalizado)>();c.validar();
    }
    return c;
}
ConfiguracaoControles& ConfiguracaoControles::instancia() {
    static ConfiguracaoControles c=[] {try{return carregar();}catch(const std::exception&){return ConfiguracaoControles{};}}();return c;
}
void ConfiguracaoControles::salvar() const {
    validar();Persistencia::escrever_json("controles.json",{{"teclas",teclas},{"botoes",botoes},{"personalizado",personalizado}});
}
void ConfiguracaoControles::atribuir(unsigned jogador,unsigned acao,int tecla) {
    auto copia=*this;copia.teclas.at(jogador).at(acao)=tecla;copia.personalizado.at(jogador)=true;
    copia.validar();copia.salvar();*this=copia;
}
Logica::Controle combinar_controle(Logica::Controle c,float x,float y,bool pulo) {
    c.esquerda|=x<-25;c.direita|=x>25;c.descer|=y>25;c.pular|=pulo;return c;
}
Logica::Comandos ler_controles(bool solo) {
    using K=sf::Keyboard;Logica::Comandos comandos{};
    const auto& c=ConfiguracaoControles::instancia();sf::Joystick::update();
    for(unsigned j=0;j<(solo?1u:2u);++j) {
        const auto& t=c.teclas[j];
        auto pressionada=[&](int i){return K::isKeyPressed(static_cast<K::Key>(t[i]));};
        auto& a=comandos[j];a={pressionada(0),pressionada(1),pressionada(2),pressionada(3)};
        if(solo && !c.personalizado[0]) {
            a.esquerda|=K::isKeyPressed(K::Left);a.direita|=K::isKeyPressed(K::Right);
            a.pular|=K::isKeyPressed(K::Up)||K::isKeyPressed(K::Space);a.descer|=K::isKeyPressed(K::Down);
        }
        if(sf::Joystick::isConnected(j)) {
            auto eixo=[&](sf::Joystick::Axis analogico,sf::Joystick::Axis digital) {
                float v=sf::Joystick::hasAxis(j,analogico)?sf::Joystick::getAxisPosition(j,analogico):0;
                if(sf::Joystick::hasAxis(j,digital)) {float d=sf::Joystick::getAxisPosition(j,digital);if(std::abs(d)>25)v=d;}
                return v;
            };
            // POV vertical do SFML aponta positivo para cima.
            const float y=sf::Joystick::hasAxis(j,sf::Joystick::Y)?sf::Joystick::getAxisPosition(j,sf::Joystick::Y):0;
            a=combinar_controle(a,eixo(sf::Joystick::X,sf::Joystick::PovX),y,
                sf::Joystick::isButtonPressed(j,c.botoes[j]));
        }
    }
    return comandos;
}
namespace {
std::string nome_tecla(int t) {
    using K=sf::Keyboard;
    if(t>=K::A && t<=K::Z)return std::string(1,'A'+t-K::A);
    if(t>=K::Num0 && t<=K::Num9)return std::string(1,'0'+t-K::Num0);
    if(t>=K::F1 && t<=K::F15)return "F"+std::to_string(t-K::F1+1);
    switch(t) {case K::Left:return "Esquerda";case K::Right:return "Direita";case K::Up:return "Cima";
    case K::Down:return "Baixo";case K::Space:return "Espaco";case K::Enter:return "Enter";
    case K::LShift:return "Shift esquerdo";case K::RShift:return "Shift direito";
    case K::LControl:return "Ctrl esquerdo";case K::RControl:return "Ctrl direito";case K::Tab:return "Tab";
    default:return "Tecla "+std::to_string(t);}
}
}
bool MenuControles::tratar(const sf::Event& e) {
    auto& c=ConfiguracaoControles::instancia();
    try {
        if(aguardando) {
            if(e.type==sf::Event::KeyPressed && e.key.code==sf::Keyboard::Escape){aguardando=false;return true;}
            if(opcao<8 && e.type==sf::Event::KeyPressed) {c.atribuir(opcao/4,opcao%4,e.key.code);aguardando=false;mensagem="Tecla salva.";}
            if(opcao>=8 && e.type==sf::Event::JoystickButtonPressed && e.joystickButton.joystickId==opcao-8) {
                auto copia=c;copia.botoes[opcao-8]=e.joystickButton.button;copia.salvar();c=copia;aguardando=false;mensagem="Botao salvo.";
            }
            return true;
        }
        if(e.type!=sf::Event::KeyPressed)return true;
        if(e.key.code==sf::Keyboard::Escape)return false;
        if(e.key.code==sf::Keyboard::Up)opcao=(opcao+10)%11;
        if(e.key.code==sf::Keyboard::Down)opcao=(opcao+1)%11;
        if(e.key.code==sf::Keyboard::Enter) {
            if(opcao==10){ConfiguracaoControles novo;novo.salvar();c=novo;mensagem="Padrao restaurado.";}
            else {aguardando=true;mensagem=opcao<8?"Pressione a nova tecla (Esc cancela).":"Pressione o botao de pulo no controle indicado.";}
        }
    }catch(const std::exception& erro){mensagem=erro.what();}
    return true;
}
void MenuControles::desenhar(sf::RenderWindow& janela) const {
    Tema::titulo(janela,"CONTROLES",80,64);
    const auto& c=ConfiguracaoControles::instancia();const char* a[]={"Esquerda","Direita","Pular","Descer"};
    for(unsigned i=0;i<11;++i) {
        std::string linha=i<8?"J"+std::to_string(i/4+1)+" / "+a[i%4]+": "+nome_tecla(c.teclas[i/4][i%4]):
            i<10?"Controle "+std::to_string(i-7)+" / Pular: botao "+std::to_string(c.botoes[i-8]+1):"Restaurar padrao";
        Tema::botao(janela,{85,190+49.f*i,854,43},linha,opcao==i,20);
    }
    auto escrever=[&](std::string t,float y) {sf::Text texto(t,fonte_interface(),19);texto.setPosition(90,y);aplicar_texto(texto,840);janela.draw(texto);};
    escrever(aguardando?mensagem:"Setas: escolher | Enter: alterar | Esc: voltar",755);
    if(!aguardando)escrever(mensagem,788);
    escrever("Padrao solo: WASD + setas + espaco. Remapear J1 remove os atalhos extras.",825);
    escrever("Controles 1/2: analogico ou direcional horizontal; analogico para descer.",860);
}
}
