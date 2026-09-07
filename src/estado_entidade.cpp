#include "../Persistencia/entidades.h"
#include "../Estados/Fases/fase.h"

namespace Persistencia {
namespace {
sf::Vector2f vetor(const Json& j, double minimo = -1000000, double maximo = 1000000) {
    if (!j.is_array() || j.size() != 2) throw std::runtime_error("Vetor invalido.");
    return {static_cast<float>(numero(j[0], minimo, maximo)),
            static_cast<float>(numero(j[1], minimo, maximo))};
}
Json vetor(sf::Vector2f v) { return Json::array({v.x, v.y}); }
Json corpo_a_corpo(const Logica::EstadoCorpo& c) {
    return {{"acao",static_cast<int>(c.acao)},{"alvo",c.alvo},{"passos",c.passos},
        {"direita",c.direita},{"origem",c.origem},{"iniciado",c.iniciado}};
}
Logica::EstadoCorpo corpo_a_corpo(const Json& c, bool gigante) {
    const int acao=inteiro(c.at("acao"),0,4), alvo=inteiro(c.at("alvo"),-1,2);
    if(alvo==0 || (gigante?acao==1:acao>1)) throw std::runtime_error("Comportamento invalido.");
    const int limite=acao==2?41:acao==3?11:acao==4?59:0;
    const unsigned passos=static_cast<unsigned>(inteiro(c.at("passos"),0,limite));
    if((acao==1 || acao==2 || acao==3) != (alvo!=-1)) throw std::runtime_error("Alvo incoerente.");
    return {static_cast<Logica::AcaoCorpo>(acao),alvo,passos,c.at("direita").get<bool>(),
        static_cast<float>(numero(c.at("origem"),-1000000,1000000)),c.at("iniciado").get<bool>()};
}

}

Json Serializador::salvar(const Entidades::Entidade& e) {
    using namespace Entidades;
    using namespace Personagens;
    using namespace Obstaculos;
    std::string tipo;
    Json extra = Json::object();
    if (auto p = dynamic_cast<const Jogador*>(&e)) {
        tipo = "jogador";
        const auto& a = p->get_animacao();
        const auto& m = p->get_movimento();
        extra = {{"jogador2", p->jogador2}, {"nome", p->nome}, {"tempo", p->tempo},
                 {"poder", p->poder}, {"leu_fase", p->leu_fase},
                 {"movimento",{{"origem_queda",m.origem_queda},{"queda_ativa",m.queda_ativa},{"gelo",m.gelo}}},
                 {"animacao", {{"correndo",a.correndo},{"quadro",a.quadro},{"passos",a.passos},{"direita",a.direita}}}};
    } else if (auto p = dynamic_cast<const Arqueiro*>(&e)) {
        tipo = "arqueiro";
        extra = {{"recarga", p->recarregar}, {"atirando", p->atirando}, {"projeteis", Json::array()}};
        const auto& c = p->get_comportamento();
        extra["comportamento"] = {{"acao",static_cast<int>(c.acao)},{"alvo",c.alvo},
            {"preparacao",c.preparacao},{"direita",c.direita}};
        for (const auto& flecha : p->vetor_projeteis) extra["projeteis"].push_back(salvar(flecha));
    } else if (auto p = dynamic_cast<const Zumbi*>(&e)) {
        tipo = "zumbi"; extra["pulo"] = p->pulo;
        extra["comportamento"]=corpo_a_corpo(p->get_comportamento());
    } else if (auto p = dynamic_cast<const Gigante*>(&e)) {
        tipo = "gigante"; extra = {{"ja_inc", p->ja_inc}, {"tempo_pulo", p->tempo_pulo}};
        extra["comportamento"]=corpo_a_corpo(p->get_comportamento());
    } else if (auto p = dynamic_cast<const Projetil*>(&e)) {
        tipo = "projetil"; extra = {{"dano", p->dano}, {"direcao", p->direcao}};
    } else if (auto p = dynamic_cast<const Coracao*>(&e)) {
        tipo = "coracao"; extra["cura"] = p->cura;
    } else if (auto p = dynamic_cast<const Espinho*>(&e)) {
        tipo = "espinho"; extra["dano"] = p->dano;
    } else if (auto p = dynamic_cast<const Caixa*>(&e)) {
        tipo = "caixa"; extra["atrapalha"] = p->atrapalha;
    } else if (auto p = dynamic_cast<const Neve*>(&e)) {
        tipo = "neve"; extra["escorrega"] = p->escorrega;
    } else if (auto p = dynamic_cast<const Musgo*>(&e)) {
        tipo = "musgo"; extra["gosmento"] = p->gosmento;
    } else throw std::runtime_error("Tipo de entidade desconhecido.");

    if (auto p = dynamic_cast<const Personagem*>(&e)) extra["forca"] = p->forca;
    if (auto p = dynamic_cast<const Inimigo*>(&e)) {
        extra["direcao"] = p->direcao;
        extra["maldade"] = p->maldade;
        const auto& a=p->get_animacao();
        extra["animacao"]={{"correndo",a.correndo},{"quadro",a.quadro},{"passos",a.passos},{"direita",a.direita}};
    }
    return {{"tipo", tipo}, {"posicao", vetor(e.corpo.getPosition())},
            {"velocidade", vetor(e.velocidade)}, {"vida", e.vida}, {"vivo", e.vivo},
            {"protecao", e.get_protecao()}, {"nochao", e.nochao}, {"pausado", e.pausado},
            {"tamanho", vetor(e.corpo.getSize())}, {"escala", vetor(e.corpo.getScale())},
            {"origem", vetor(e.corpo.getOrigin())}, {"rotacao", e.corpo.getRotation()},
            {"cor", e.corpo.getFillColor().toInteger()}, {"extra", extra}};
}

std::unique_ptr<Entidades::Entidade> Serializador::carregar(const Json& j) {
    using namespace Entidades;
    using namespace Personagens;
    using namespace Obstaculos;
    const auto tipo = j.at("tipo").get<std::string>();
    const auto& x = j.at("extra");
    const auto pos = vetor(j.at("posicao")), vel = vetor(j.at("velocidade"));
    std::unique_ptr<Entidade> e;
    if (tipo == "jogador") {
        auto p = std::make_unique<Jogador>(pos, vel, x.at("jogador2").get<bool>());
        p->nome = nome_valido(x.at("nome").get<std::string>());
        p->tempo = numero(x.at("tempo"), 0, 1e12);
        p->poder = static_cast<float>(numero(x.at("poder"), 0, 1000000));
        p->leu_fase = x.at("leu_fase").get<bool>();
        if (x.contains("movimento")) {
            const auto& m=x.at("movimento");
            p->restaurar_movimento({static_cast<float>(numero(m.at("origem_queda"),-1000000,1000000)),
                m.at("queda_ativa").get<bool>(),m.at("gelo").get<bool>()});
        } else p->restaurar_movimento({pos.y,false,false});
        if (x.contains("animacao")) {
            const auto& a = x.at("animacao");
            p->restaurar_animacao({a.at("correndo").get<bool>(),
                static_cast<unsigned>(inteiro(a.at("quadro"),0,26)),
                static_cast<unsigned>(inteiro(a.at("passos"),0,1)),a.at("direita").get<bool>()});
        }
        e = std::move(p);
    } else if (tipo == "arqueiro") {
        auto p = std::make_unique<Arqueiro>(pos, vel);
        p->recarregar = inteiro(x.at("recarga"), 0, TEMPO_RECARGA);
        p->atirando = x.at("atirando").get<bool>();
        if (x.contains("comportamento")) {
            const auto& c=x.at("comportamento");
            const int alvo=inteiro(c.at("alvo"),-1,2);
            if (alvo==0) throw std::runtime_error("Alvo invalido.");
            p->restaurar_comportamento({static_cast<Logica::AcaoArqueiro>(inteiro(c.at("acao"),0,2)),
                alvo,static_cast<unsigned>(inteiro(c.at("preparacao"),0,29)),c.at("direita").get<bool>()});
        }
        const auto& flechas = x.at("projeteis");
        if (!flechas.is_array() || flechas.size() > 4096) throw std::runtime_error("Lista de flechas invalida.");
        p->vetor_projeteis.reserve(flechas.size());
        for (const auto& f : flechas) {
            if (f.at("tipo") != "projetil") throw std::runtime_error("Tipo de flecha invalido.");
            auto flecha = carregar(f);
            p->vetor_projeteis.push_back(*static_cast<Projetil*>(flecha.get()));
        }
        e = std::move(p);
    } else if (tipo == "zumbi") {
        auto p = std::make_unique<Zumbi>(pos, vel);
        p->pulo = inteiro(x.at("pulo"), 0, 9);
        if(x.contains("comportamento")) p->restaurar_comportamento(corpo_a_corpo(x.at("comportamento"),false));
        e = std::move(p);
    } else if (tipo == "gigante") {
        auto p = std::make_unique<Gigante>(pos, vel);
        p->ja_inc = x.at("ja_inc").get<bool>();
        p->tempo_pulo = numero(x.at("tempo_pulo"), 0, 1e12);
        if(x.contains("comportamento")) p->restaurar_comportamento(corpo_a_corpo(x.at("comportamento"),true));
        e = std::move(p);
    } else if (tipo == "projetil") {
        auto p = std::make_unique<Projetil>(pos, x.at("direcao").get<bool>());
        p->dano = inteiro(x.at("dano"), 0, 1000000); e = std::move(p);
    } else if (tipo == "coracao") {
        auto p = std::make_unique<Coracao>(pos);
        p->cura = inteiro(x.at("cura"), 0, 1000000); e = std::move(p);
    } else if (tipo == "espinho") {
        auto p = std::make_unique<Espinho>(pos);
        p->dano = inteiro(x.at("dano"), 0, 1000000); e = std::move(p);
    } else if (tipo == "caixa") {
        auto p = std::make_unique<Caixa>(pos);
        p->atrapalha = x.at("atrapalha").get<bool>(); e = std::move(p);
    } else if (tipo == "neve") {
        auto p = std::make_unique<Neve>(pos);
        p->escorrega = x.at("escorrega").get<bool>(); e = std::move(p);
    } else if (tipo == "musgo") {
        auto p = std::make_unique<Musgo>(pos);
        p->gosmento = x.at("gosmento").get<bool>(); e = std::move(p);
    } else throw std::runtime_error("Tipo de entidade desconhecido.");

    e->corpo.setPosition(pos);
    e->posicao = pos;
    e->velocidade = vel;
    e->set_protecao(static_cast<unsigned>(inteiro(j.value("protecao",Json(0)),0,600)));
    e->vida = inteiro(j.at("vida"), -1000000, 1000000);
    e->vivo = j.at("vivo").get<bool>();
    e->nochao = j.at("nochao").get<bool>();
    e->pausado = j.at("pausado").get<bool>();
    e->corpo.setSize(vetor(j.at("tamanho"), 0.01, 10000));
    e->corpo.setScale(vetor(j.at("escala"), -100, 100));
    e->corpo.setOrigin(vetor(j.at("origem")));
    e->corpo.setRotation(static_cast<float>(numero(j.at("rotacao"), 0, 360)));
    const auto cor = j.at("cor");
    if (!cor.is_number_integer()) throw std::runtime_error("Cor invalida.");
    e->corpo.setFillColor(sf::Color(static_cast<sf::Uint32>(numero(cor, 0, 4294967295.0))));
    if (auto p = dynamic_cast<Personagem*>(e.get())) p->forca = inteiro(x.at("forca"), 0, 1000000);
    if (auto p = dynamic_cast<Inimigo*>(e.get())) {
        p->direcao = x.at("direcao").get<bool>();
        p->maldade = x.at("maldade").get<bool>();
        if(x.contains("animacao")) {
            const auto& a=x.at("animacao");
            p->restaurar_animacao({a.at("correndo").get<bool>(),static_cast<unsigned>(inteiro(a.at("quadro"),0,26)),
                static_cast<unsigned>(inteiro(a.at("passos"),0,1)),a.at("direita").get<bool>()});
        }
    }
    return e;
}
}
