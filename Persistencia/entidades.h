#pragma once
#include "arquivo.h"
#include <memory>
namespace Entidades { class Entidade; }
namespace Persistencia {
// Acesso restrito ao adaptador de persistencia, sem setters publicos para cada detalhe.
class Serializador {
public:
    static Json salvar(const Entidades::Entidade& entidade);
    static std::unique_ptr<Entidades::Entidade> carregar(const Json& dados);
};
}
