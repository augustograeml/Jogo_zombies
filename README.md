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

No Linux Debian/Ubuntu, os pacotes de desenvolvimento usuais são `g++`, `make` e `libsfml-dev`; confira que a versão instalada é 2.6.x. O catálogo encontra `Design/` pela pasta atual ou pela pasta do executável. `ZOMBIES_RECURSOS` pode indicar outra raiz de recursos. Saves e preferências ficam na pasta de onde o programa foi iniciado. A cópia local `SFML-local/usr` é reconhecida automaticamente pelo Make e permanece ignorada pelo Git; num clone novo, instale SFML 2.6.x ou informe `SFML_ROOT`. O módulo de áudio também é necessário.

O VS Code foi ajustado para compilar o projeto inteiro, em vez de apenas o arquivo aberto. O compilador, o Make e o GDB precisam estar disponíveis no PATH do editor.

## Controles e salvamento

- Solo: `WASD` ou setas; `W`, seta para cima ou espaço pulam.
- Dupla: jogador 1 usa `WASD`; jogador 2 usa setas.
- Ao soltar as teclas horizontais, o jogador desacelera até parar, inclusive nas plataformas de neve. Teclas opostas também freiam.
- Menus: setas e `Enter`; `Esc` volta.
- Durante o jogo: `Esc` pausa e salva. Perder o foco também pausa.
- Na pausa: `Esc` continua; `S` repete o salvamento, útil após um erro de escrita.
- Fechar normalmente a janela salva a partida. Ao abrir novamente, escolha **Continuar**.
- Os saves aparecem somente em **Continuar**. Nessa tela, setas ou `1`, `2`, `3` selecionam; `Enter` carrega; `R` recupera a cópia anterior; `Esc` volta. Novo Jogo usa o slot selecionado.
- Ranking: setas mudam a fase, `Tab` muda solo/dupla, `P` alterna tempo/pontos, `Enter` ou `Esc` volta ao menu.
- Pausa: `M` silencia; `+`/`-` ajustam o volume. Preferências persistem em `preferencias.json`.

Há três slots completos: `partida.json`, `partida-slot2.json` e `partida-slot3.json`. Novo Jogo substitui o slot selecionado e guarda a versão anterior válida em `.bak`. A recuperação por `R` preserva o arquivo substituído em `.corrompido-N`. Arquivos corrompidos nunca são sobrescritos silenciosamente. Saves completos de versão 1 migram para a versão 2; os antigos arquivos separados por entidade continuam sem informação suficiente para uma retomada completa.

O painel superior mostra tempo, recorde de conclusão com nome, vida de cada jogador, pontos da equipe e recorde de pontos da fase/modalidade. O cronômetro começa em zero em **Novo Jogo**, para durante a pausa e é preservado ao continuar. No avanço automático para a fase 2, mantém o tempo total da partida; o ranking continua medindo cada fase separadamente.

O jogo pede o nome de cada participante tanto na vitória quanto na derrota. Os nomes da derrota são salvos, mas apenas fases concluídas entram no ranking de tempos. Se fechar antes de confirmar os nomes, **Continuar** recupera a tela pendente, também após uma derrota.

## Testes em C++

```sh
make test-logica
make test-integracao
```

Em um Linux sem ambiente gráfico, com Xvfb instalado:

```sh
make test-logica
ALSOFT_DRIVERS=null xvfb-run -a make test-integracao
```

Os testes usam pastas temporárias e não modificam o seu ranking ou a sua partida. Para verificar a retomada entre processos, use a mesma pasta temporária nos dois comandos:

```sh
./build/testes --produzir /tmp/zombies-retomada
./build/testes --verificar /tmp/zombies-retomada
```

No Windows, o executável de testes é `build/testes.exe`; substitua a pasta temporária por um caminho adequado ao seu sistema.

## Melhorias implementadas

- Corrida de 27 quadros para o jogador 1, pose parada e direção independente da colisão. Luigi caminha com articulação da arte original em C++; os três tipos de zumbi usam sequências de oito poses. A animação acompanha a simulação e é salva.
- Colisões por canto superior esquerdo, chão sem rebote e efeito de superfície aplicado uma vez por passo. O gelo freia menos, mas permite parar.
- Pisões após uma queda maior que 300 unidades (seis blocos de 50) causam dano dobrado; acima de 500, triplo. O bônus é consumido no impacto, e a altura acumulada também é salva.
- Proteção de 45 passos (0,75 s) após dano, reação visual e cura limitada a 20 vidas.
- Arqueiro detecta o jogador vivo mais próximo até 650 unidades (diferença vertical até 120), sinaliza por 30 passos e dispara; recarga de 100 passos. Sem alvo, patrulha. Zumbis e gigantes mantêm suas diferenças existentes.
- Grade espacial de obstáculos com ordem determinística e reconsulta após deslocamento; apenas o desenho é descartado fora da câmera. Entidades e flechas fora da tela continuam simulando.
- Salto, disparo, dano no jogador, coleta, impacto no inimigo, morte do inimigo, vitória e derrota têm oito sons distintos sintetizados inteiramente em C++. Pausa interrompe sons; carregar não repete eventos antigos. Música de fundo não faz parte desta entrega.
- Catálogo, legenda de mapas e parâmetros documentados em [Recursos/README.md](Recursos/README.md).

### Pontuação

O ranking de tempo continua exclusivo de conclusões; `ranking-pontos.json` é independente. Pontos são compartilhados na dupla e medidos por fase: zumbi 100, arqueiro 150, gigante 300, coração 25 e conclusão 500. Não há pontos por esperar. Qualquer resultado positivo pode disputar as oito posições visíveis, incluindo derrotas; empate é decidido por menor tempo e depois ordem de registro. Novo Jogo zera pontos e relógio. Avançar automaticamente de fase mantém o relógio total e inicia a pontuação da nova fase.

O save guarda o total e os estados dos objetos consumidos/derrotados. Recarregar não concede recompensas antigas. O identificador da partida impede registros duplicados, inclusive ao repetir uma confirmação após falha de gravação.

### Cinco padrões de projeto

| Padrão | Uso real no código |
| --- | --- |
| Singleton | Gerenciadores existentes e `Persistencia::Slots::instancia()` compartilham serviços/slot. O Singleton foi preservado. |
| State | `Estado` e suas telas/fases implementam execução e eventos diferentes; `Gerenciador_Estados` delega ao estado ativo. `Estados::Tela` nomeia os IDs persistidos. |
| Observer | `Logica::Eventos` notifica `Pontuacao` e `Audio::ObservadorSom` a partir de fatos da simulação. |
| Strategy | `CriterioPontos` e `CriterioTempo` implementam políticas de classificação; os repositórios usam essas políticas. |
| Iterator | O iterador da lista encadeada acadêmica percorre entidades sem expor seus nós e continua usado na simulação, colisão e persistência. |

`Fase` coordena os módulos: `ConstrutorCenario` interpreta mapas, `Sessao` controla passos/tempo, `ResultadoPartida` reúne o resultado, `PainelPartida` apresenta os indicadores e os repositórios gravam os dados. Não foi substituída a lista encadeada exigida pela disciplina.

### Validação e limites

`make test-logica` roda animação, AABB, eventos, IA, sessão, slots e classificação sem SFML nem servidor gráfico. `make test-integracao` cobre o fluxo anterior, snapshots, falhas de escrita, recursos, superfícies, ranking e comparações de grade. Em ambiente sem áudio físico, `ALSOFT_DRIVERS=null` permite testar a integração sonora sem alto-falantes.

A validação executada nesta entrega é Linux com GCC 14 e SFML 2.6.2. O caminho de compilação Windows foi mantido, mas esta revisão não foi executada no Windows. O binário Windows antigo do repositório precisa ser recompilado.

Os testes imprimem tempos de colisão/simulação/desenho e candidatos consultados; são medições locais, não promessa de FPS em outro computador. O teste compara snapshots com grade ligada/desligada. Música de fundo e autosave periódico continuam como possibilidades futuras. Luigi usa a imagem existente articulada, sem uma nova folha de sprites. O plano local atualizado está em `PLANO_MELHORIAS_JOGO.md`, ignorado pelo Git.
