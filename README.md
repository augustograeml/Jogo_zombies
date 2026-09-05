# Zombies++
Este projeto é um trabalho para a disciplina de Técnicas de programação realizado pelos alunos Antonio e Augusto (Turma S73).

# Créditos
Os cenários utilizados no projeto:
  - IA do bing: https://www.bing.com/images/create?FORM=IRPGEN

Os códigos utilizados como inpiração para o projeto:
  - Peteco: https://github.com/Nixxye/peteco-tegprog
  - Monitor: https://github.com/Giovanenero/JogoPlataforma2D-Jungle
  - Nolan: https://github.com/nlohmann/json

As sprites utilizadas no projeto foram retiradas dos seguintes sites: 
  - Freepik: https://br.freepik.com/
  - Pngwing: https://www.pngwing.com/
  - Imgbin: https://imgbin.com/

Vídeos consultados para inspiração no projeto:
  - Monitor: https://www.youtube.com/watch?v=gfGE5KY1OQU&list=PLR17O9xbTbIBBoL3lli44N8LdZVvg-_uZ&pp=iAQB
  - Terminal Root: https://youtu.be/h8-Q4eu3Qt4?si=-N-MRAAK-oW_FdWF

# Informações
Informações técnicas do projeto:
  - SO: Windows
  - Compilador g++: 13.2.0 pelo pacote de dados MSYS2
  - Biblioteca Gráfica: SFML 2.6.0

# Versão refatorada: ranking e retomada completa

A análise detalhada está no arquivo local `ANALISE_E_MELHORIAS.md`, que não é versionado. As instruções de compilação, uso e testes estão abaixo.

## Compilar e executar

Requer compilador C++17, GNU Make e **SFML 2.6.x** (a API usada é a da série 2, não da série 3). A biblioteca JSON C++ já está incluída em `Estados/Fases/json.h`. Os fontes locais usam apenas `.cpp` e `.h`.

Com a SFML instalada e disponível ao compilador, execute na raiz do projeto:

```sh
make -j4
./prog
```

No Windows, use um terminal MSYS2 configurado com o compilador, o Make e a SFML 2.6.x compatíveis. O resultado é `prog.exe`. Se a biblioteca estiver em outra pasta, informe seus caminhos, por exemplo:

```sh
make -j4 CPPFLAGS="-IC:/Bibliotecas/SFML-2.6.0/include" LDFLAGS="-LC:/Bibliotecas/SFML-2.6.0/lib"
./prog.exe
```

Para a ligação dinâmica, as DLLs da SFML e do compilador devem estar no PATH ou junto ao executável. Compile novamente: o `prog.exe` antigo que já existia no repositório não contém as melhorias. Se mudar de plataforma ou de compilador, execute `make clean` antes de recompilar.

No Linux Debian/Ubuntu, os pacotes de desenvolvimento usuais são `g++`, `make` e `libsfml-dev`; confira que a versão instalada é 2.6.x. Execute sempre a partir da raiz do projeto para encontrar `Design/` e os arquivos de partida.

O VS Code foi ajustado para compilar o projeto inteiro, em vez de apenas o arquivo aberto. O compilador, o Make e o GDB precisam estar disponíveis no PATH do editor.

## Controles e salvamento

- Jogador 1: `W`, `A`, `S`, `D`; jogador 2: setas.
- Menus: setas e `Enter`; `Esc` volta.
- Durante o jogo: `Esc` pausa e salva. Perder o foco também pausa.
- Na pausa: `Esc` continua; `S` repete o salvamento, útil após um erro de escrita.
- Fechar normalmente a janela salva a partida. Ao abrir novamente, escolha **Continuar**.
- Ranking: setas mudam a fase, `Tab` muda solo/dupla, `Enter` ou `Esc` volta ao menu.

O arquivo `partida.json` mantém um slot completo, incluindo flechas em voo. Um novo jogo substitui esse slot. Os arquivos antigos de partida são preservados, mas não permitem reconstruir os estados ausentes; inicie uma partida nesta versão para usar a retomada completa.

## Testes em C++

```sh
make test
```

Em um Linux sem ambiente gráfico, com Xvfb instalado:

```sh
xvfb-run -a make test
```

Os testes usam pastas temporárias e não modificam o seu ranking ou a sua partida. Para verificar a retomada entre processos, use a mesma pasta temporária nos dois comandos:

```sh
./build/testes --produzir /tmp/zombies-retomada
./build/testes --verificar /tmp/zombies-retomada
```

No Windows, o executável de testes é `build/testes.exe`; substitua a pasta temporária por um caminho adequado ao seu sistema.
