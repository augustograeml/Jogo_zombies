CXX ?= g++
CPPFLAGS ?=
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
ifeq ($(OS),Windows_NT)
EXE := .exe
endif
# SFML instalada no sistema ou copia local ignorada pelo Git.
SFML_ROOT ?= $(firstword $(wildcard SFML-local/usr SFML*/ SFML*/usr))
ifneq ($(SFML_ROOT),)
CPPFLAGS += -I$(SFML_ROOT)/include
SFML_LIB := $(firstword $(wildcard $(SFML_ROOT)/lib/x86_64-linux-gnu $(SFML_ROOT)/lib))
LDFLAGS += -L$(SFML_LIB)
ifneq ($(OS),Windows_NT)
LDFLAGS += -Wl,--disable-new-dtags,-rpath,$(abspath $(SFML_LIB))
endif
endif
TARGET := prog$(EXE)
SOURCES := $(wildcard src/*.cpp)
HEADERS := $(shell find . -name '*.h')
OBJECTS := $(patsubst src/%.cpp,build/%.o,$(SOURCES))
TEST_OBJECTS := $(filter-out build/main.o build/jogo.o,$(OBJECTS))

.PHONY: all test clean
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@
build/testes$(EXE): tests/regressao.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) tests/regressao.cpp $(TEST_OBJECTS) $(LDLIBS) -o $@
test: build/testes$(EXE)
	./build/testes$(EXE)
clean:
	rm -rf build $(TARGET)
-include $(OBJECTS:.o=.d)

# Regras puras: nao ligam SFML, nao precisam de janela nem Xvfb.
build/teste-animacao$(EXE): tests/animacao.cpp Animacao/corrida.h Recursos/configuracao.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@
build/teste-fisica$(EXE): tests/fisica.cpp src/aabb.cpp Fisica/aabb.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) tests/fisica.cpp src/aabb.cpp -o $@
build/teste-persistencia$(EXE): tests/persistencia_extra.cpp src/arquivo.cpp src/slots.cpp src/pontos.cpp Persistencia/slots.h Persistencia/pontos.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) tests/persistencia_extra.cpp src/arquivo.cpp src/slots.cpp src/pontos.cpp -o $@
build/teste-logica$(EXE): tests/logica.cpp Audio/sintese.h Logica/entrada.h Logica/eventos.h Logica/comportamento.h Logica/sessao.h Logica/movimento.h Recursos/configuracao.h Estados/identificador.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@
build/novidades$(EXE): tests/novidades.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) tests/novidades.cpp $(TEST_OBJECTS) $(LDLIBS) -o $@
.PHONY: test-logica test-integracao
test-logica: build/teste-animacao$(EXE) build/teste-fisica$(EXE) build/teste-persistencia$(EXE) build/teste-logica$(EXE)
	./build/teste-animacao$(EXE)
	./build/teste-fisica$(EXE)
	./build/teste-persistencia$(EXE)
	./build/teste-logica$(EXE)
test-integracao: build/caminhada$(EXE) build/testes$(EXE) build/novidades$(EXE) build/superficies$(EXE) build/animacao-integracao$(EXE) build/resultado-falha$(EXE) build/menu-saves$(EXE)
	./build/testes$(EXE)
	./build/novidades$(EXE)
	./build/superficies$(EXE)
	./build/animacao-integracao$(EXE)
	./build/resultado-falha$(EXE)
	./build/menu-saves$(EXE)
	./build/caminhada$(EXE)

build/superficies$(EXE): tests/superficies.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) tests/superficies.cpp $(TEST_OBJECTS) $(LDLIBS) -o $@

build/animacao-integracao$(EXE): tests/animacao_integracao.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(TEST_OBJECTS) $(LDLIBS) -o $@
build/resultado-falha$(EXE): tests/resultado_falha.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(TEST_OBJECTS) $(LDLIBS) -o $@

build/menu-saves$(EXE): tests/menu_saves.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(TEST_OBJECTS) $(LDLIBS) -o $@

build/caminhada$(EXE): tests/caminhada.cpp $(TEST_OBJECTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< $(TEST_OBJECTS) $(LDLIBS) -o $@
