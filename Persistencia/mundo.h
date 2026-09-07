#pragma once
#include "arquivo.h"
#include "../Logica/mundo.h"
namespace Persistencia {
Json capturar_mundo(Logica::Mundo& mundo);
// Valida listas temporarias antes de substituir o mundo; preserva RNG em falhas.
void restaurar_mundo(Logica::Mundo& mundo, const Json& dados);
}
