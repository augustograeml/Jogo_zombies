#include "../Persistencia/pontos.h"
#include "../Interface/preferencias.h"
#include "../Interface/tema.h"
#include "../Recursos/catalogo.h"
#include "../Estados/Menus/ranking.h"
#include "../Persistencia/ranking.h"
#include <iomanip>
#include <sstream>
namespace Estados::Menus {
Ranking::Ranking(int id) : Estado(id) {
    imagem.loadFromFile(Recursos::caminho("Design/imagens/rankingzombies++.png").string());
}
void Ranking::atualizar() {
    linhas.clear(); erro=false;
    try {
        std::vector<Persistencia::ResultadoPontos> resultados;
        if (por_pontos) resultados=Persistencia::RepositorioPontos().consultar(fase,jogadores);
        else for (const auto& r : Persistencia::RepositorioRanking().consultar(fase,jogadores))
            resultados.push_back({r.id,r.fase,r.jogadores,r.nomes,0,r.segundos,true});
        for (std::size_t i=0;i<resultados.size() && i<8;++i) {
            const auto& r=resultados[i];
            std::string nomes;
            for(const auto& nome:r.nomes) { if(!nomes.empty()) nomes+=" + "; nomes+=nome; }
            std::ostringstream tempo; tempo<<std::fixed<<std::setprecision(3)<<r.segundos<<" s";
            linhas.push_back({nomes,tempo.str(),std::to_string(r.pontos),r.vitoria});
        }
    } catch(const std::exception& e) { erro=true; pGE->mensagem=std::string("Erro no ranking: ")+e.what(); }
}
void Ranking::ao_entrar() { atualizar(); }
void Ranking::tratar_evento(const sf::Event& evento) {
    if (evento.type != sf::Event::KeyPressed) return;
    if (evento.key.code == sf::Keyboard::Escape || evento.key.code == sf::Keyboard::Enter) pGE->set_estado_atual(Estados::Tela::Principal);
    else if (evento.key.code == sf::Keyboard::P) { por_pontos=!por_pontos; atualizar(); }
    else if (evento.key.code == sf::Keyboard::Left || evento.key.code == sf::Keyboard::Right) {
        fase = fase == 1 ? 2 : 1; atualizar();
    } else if (evento.key.code == sf::Keyboard::Tab) { jogadores = jogadores == 1 ? 2 : 1; atualizar(); }
}
void Ranking::executar() {
    auto* grafico=Gerenciadores::Gerenciador_Grafico::get_instancia();
    grafico->resetarCamera(); grafico->desenharTextura(&imagem);
    auto& janela=*grafico->get_Janela();
    const bool contraste=Interface::Preferencias::instancia().contraste;
    if(contraste) { sf::RectangleShape fundo({1024,1024}); fundo.setFillColor(sf::Color::Black); janela.draw(fundo); }
    const float rodape=330+58*std::max<std::size_t>(3,linhas.size());
    Interface::Tema::painel(janela,{96,116,832,rodape-116+90});
    auto texto=[&](const std::string& valor,float x,float y,unsigned tamanho,float largura,
                  sf::Color cor,bool direita=false,bool titulo=false) {
        sf::Text t(sf::String::fromUtf8(valor.begin(),valor.end()),Interface::fonte_interface(),tamanho);
        if(titulo) Interface::Tema::estilo_titulo(t);
        t.setFillColor(contraste?sf::Color::White:cor);
        Interface::aplicar_texto(t,largura);
        const auto r=t.getLocalBounds();
        t.setOrigin(direita?r.left+r.width:r.left,r.top);
        t.setPosition(x,y); janela.draw(t);
    };
    const auto tinta=Interface::Tema::tinta(), secundario=Interface::Tema::secundaria(), acento=Interface::Tema::acento();
    texto("RANKING",128,144,46,450,tinta,false,true);
    texto("REGISTRO DE SOBREVIVENTES",130,201,18,440,secundario);
    texto("FASE "+std::to_string(fase)+(jogadores==1?" / SOLO":" / DUPLA"),894,151,24,250,acento,true);
    texto(por_pontos?"CLASSIFICACAO POR PONTOS":"CLASSIFICACAO POR TEMPO",894,202,17,300,secundario,true);
    texto(por_pontos?"Maior pontuacao; desempate pelo tempo":"Menor tempo de conclusao vence",130,241,20,760,secundario);
    sf::RectangleShape faixa({768,36}); faixa.setPosition(128,280);
    faixa.setFillColor(contraste?sf::Color(30,30,30):sf::Color(39,52,35)); janela.draw(faixa);
    texto("POS.",140,290,16,50,acento);
    texto(jogadores==1?"SOBREVIVENTE":"EQUIPE",208,290,16,por_pontos?340:470,acento);
    if(por_pontos) texto("PONTOS",718,290,16,130,acento,true);
    texto("TEMPO",880,290,16,142,acento,true);
    for(std::size_t i=0;i<linhas.size();++i) {
        const float y=326+58*i; const auto& linha=linhas[i];
        sf::RectangleShape linha_fundo({768,56}); linha_fundo.setPosition(128,y);
        linha_fundo.setFillColor(i==0?sf::Color(44,57,37,210):sf::Color(24,32,27,i%2?180:90));
        if(!contraste) janela.draw(linha_fundo);
        if(i==0) { sf::RectangleShape marca({3,56}); marca.setPosition(128,y); marca.setFillColor(acento); janela.draw(marca); }
        texto(i<9?"0"+std::to_string(i+1):std::to_string(i+1),140,y+17,24,48,i==0?acento:secundario);
        texto(linha.nomes,208,y+12,25,por_pontos?342:488,tinta);
        if(por_pontos) {
            texto(linha.pontos,718,y+12,25,134,acento,true);
            texto(linha.vitoria?"CONCLUIDA":"ENCERRADA",208,y+39,12,330,secundario);
        }
        texto(linha.tempo,880,y+17,24,142,tinta,true);
        sf::RectangleShape separador({768,1}); separador.setPosition(128,y+56);
        separador.setFillColor({65,77,65}); janela.draw(separador);
    }
    if(linhas.empty()) {
        texto(erro?"REGISTRO INDISPONIVEL":"NENHUM RESULTADO AINDA",150,371,28,724,tinta);
        texto(erro?"Volte ao menu e tente novamente.":"Jogue esta categoria para registrar sua primeira marca.",150,422,22,724,secundario);
    }
    texto("SETAS: fase   |   TAB: solo/dupla   |   P: tempo/pontos",130,rodape+18,18,760,secundario);
    texto("ENTER / ESC: voltar",130,rodape+52,20,450,tinta);
    texto(std::to_string(linhas.size())+" / 8 registros",894,rodape+52,18,250,secundario,true);
}
}
