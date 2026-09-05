CXX ?= g++
CPPFLAGS ?=
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?= -lsfml-graphics -lsfml-window -lsfml-system
ifeq ($(OS),Windows_NT)
EXE := .exe
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
