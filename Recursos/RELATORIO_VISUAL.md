# Remasterização visual do Zombies++

## Entrega

Oito fundos foram gerados e integrados: evacuação abandonada, materiais deteriorados, luz fria, carvão, oliva e ferrugem. A neve mantém montanhas e pinheiros; o pântano mantém água parada, árvores e névoa. Nenhuma imagem contém texto ou controles. Os originais estão em `Design/imagens/legacy/`, comparados byte a byte com a versão anterior no Git.

Todos os caminhos abaixo são relativos a `Design/imagens`. Nomes iguais representam conteúdo novo.

| Asset antigo | Asset novo | Uso |
| --- | --- | --- |
| `cenario_op11.png` | `cenario_op11.png` | Montanha nevada |
| `cenario_op22.png` | `cenario_op22.png` | Pântano |
| `menu_zombies++.jpg` | `menu_zombies++.png` | Principal, Novo Jogo e Continuar |
| `menu2jogadores.png` | `menu2jogadores.png` | Solo/dupla |
| `menu2fases.png` | `menu2fases.png` | Neve/pântano |
| `pause.png` | `pause.png` | Pausa |
| `rankingzombies++.png` | `rankingzombies++.png` | Ranking |
| `pegar_nome.png` | `pegar_nome.png` | Vitória, derrota e nome |

Os PNGs de gameplay têm 1619×971 pixels; menus, 1254×1254. O C++ aplica recorte central proporcional, com escala uniforme nos menus e proporção 5:3 nos cenários. As faixas de enquadramento da janela continuam preservadas. Nenhuma imagem precisou ser regenerada após inspeção. A [bíblia visual](DIRECAO_VISUAL.md) e os [prompts e saídas da geração](PROMPTS_VISUAIS.md) documentam a produção.

## Interface e código

DejaVu Sans, já licenciada no projeto, passa a ser o padrão. Painéis carvão e texto marfim substituem cores dispersas. A seleção usa oliva, texto escuro, marcador lateral e `>`; alto contraste usa preto/branco. Títulos têm hierarquia clara; ranking comporta oito linhas; entrada do nome tem campo delimitado. HUD, slots e preferências seguem a mesma paleta.

Arquivos de produção: `Interface/tema.h` (novo), `src/menu.cpp`, `src/menu_principal.cpp`, `src/pause.cpp`, `src/Ranking.cpp`, `src/nome.cpp`, `src/painel.cpp`, `src/jogo.cpp`, `src/gerenciador_grafico.cpp` e `src/fundo.cpp`. O makefile ganhou um alvo de inspeção visual, implementado em `tests/remaster_visual.cpp`.

Colisões, física, animações, mapas, áudio e formatos de persistência não receberam alterações. Não foi adicionado parallax: os fundos continuam seguindo a apresentação existente, evitando mudanças na câmera nesta etapa.

## Validação

- `make clean` e `make -j4`: concluídos sem erros.
- `make test-logica`: passou.
- `ALSOFT_DRIVERS=null xvfb-run -a make -j4 test-integracao`: passou.
- `make build/remaster-visual`: passou. O executável percorreu telas reais com eventos de teclado, usando dados temporários, incluindo Novo Jogo, slots, continuar, seleções, ambas as fases solo/dupla, pausa, preferências, ranking com oito resultados e resultado de vitória/derrota com nome. Resultados finais foram preparados por fixture; não houve uma partida manual completa.
- Foram inspecionadas capturas em 1024×1024 e menu em 1280×720, incluindo alto contraste e escala 130%. Capturas da execução final: `/tmp/zombies-remaster-2331372634715/`.
- `prog` foi aberto em Xvfb por cinco segundos, sem falha de carregamento, encerrado pelo limite de tempo (código 124 esperado).
- `git diff --check`: sem problemas.

Para repetir as capturas em Linux:

```sh
make build/remaster-visual
ALSOFT_DRIVERS=null xvfb-run -a -s '-screen 0 1600x1200x24' ./build/remaster-visual
```

Limitações: a conferência ocorreu em janela virtual Linux; não houve teste em Windows nem avaliação de áudio físico. Xvfb informou ausência de contexto de entrada Unicode e de sincronização vertical. As capturas são temporárias e podem ser recriadas pelo comando acima.

## Segunda etapa possível

Jogadores, inimigos, plataformas, caixas, espinhos, corações e projéteis conservam as artes existentes. Especialmente Luigi em pixel art, as plataformas de neve e os corações coloridos destoam do ambiente mais sóbrio. Uma etapa futura pode unificar esses sprites, respeitando silhuetas, poses e caixas físicas; nenhuma dessas artes foi refeita nesta entrega.
