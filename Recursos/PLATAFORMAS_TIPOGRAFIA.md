# Plataformas e tipografia — segunda etapa visual

## Assets e integração

| Anterior | Atual | Tratamento |
| --- | --- | --- |
| `Design/imagens/plataforma.png` | Mesmo caminho, nova faixa de neve | Concreto frio, gelo e neve, 768×256 PNG |
| `Design/imagens/bloco_musgo.jpeg` | `Design/imagens/bloco_musgo.png` | Pedra úmida e musgo oliva dessaturado, 768×256 PNG |

Originais preservados byte a byte em `Design/imagens/legacy/`. Os backgrounds e os demais sprites não foram alterados.

Cada faixa contém três trechos de 256×256. `Recursos/plataforma.h` escolhe o trecho pela coluna/linha do bloco e espelha a faixa inteira no retorno, mantendo continuidade horizontal. A repetição existe a cada seis blocos, com detalhes discretos; não há novos tipos de entidade ou alteração do mapa. A superfície desenhada ocupa os mesmos 50×50 do bloco original, sem transparências que contradigam a colisão. A reconstrução do save recebe a posição original e seleciona o mesmo recorte. Nenhum número aleatório ou campo de persistência foi acrescentado.

## Geração

Ferramenta integrada de geração de imagens, duas saídas de 2172×724. As duas foram aceitas após inspeção; não houve regeneração. Foram retirados 16 pixels superiores para eliminar irregularidades na margem, recortando o centro para manter proporção 3:1 e reduzindo para 768×256 com Lanczos. Nenhuma geometria do jogo foi modificada.

Prompt da neve:

```text
Use case: stylized-concept. Asset type: production 2D side-view platform tile texture for an indie survival horror game. Output a wide 3:1 PNG, 1536x512. The ENTIRE canvas is one continuous horizontal rectangular platform facade, edge to edge, with NO background, NO padding, NO perspective, NO isometric top plane, NO labels, NO text, NO separate tile grid, NO black outlines. This strip will be split into THREE adjacent square tiles, so maintain consistent walkable top height at y=0, and continuous material across thirds; left and right ends must also join seamlessly when looped. Thin clearly readable light top edge, darker vertical stone face below. Opaque material all the way to the bottom. Deliberate hand-painted grounded realism with restrained grain matching cold desaturated abandoned survival environments. Small subtle variations in wear across the strip, no large repetitive symbols or distinctive holes, no large masonry pattern. No characters, obstacles, grass neon or beige bricks. Material: icy deteriorated cold blue-grey concrete and natural stone in a snowy mountain. Dirty off-white snow cap occupying only the top 12 percent, pale blue ice narrow underneath, dark slate grey structural face with sparse fine cracks, restrained frost and grime. Snow surface has tiny natural undulations but never exposes empty background or breaks rectangular collision silhouette. NO yellow, NO tan, NO warm beige, NO rust except tiny specks. Cold diffuse light from upper left. Important: this is a TILE STRIP TEXTURE, not a picture of a floating platform on a background.
```

Prompt do pântano:

```text
Use case: stylized-concept. Asset type: production 2D side-view platform tile texture for an indie survival horror game. Output a wide 3:1 PNG, 1536x512. The ENTIRE canvas is one continuous horizontal rectangular platform facade, edge to edge, with NO background, NO padding, NO perspective, NO isometric top plane, NO labels, NO text, NO separate tile grid, NO black outlines. This strip will be split into THREE adjacent square tiles, so maintain consistent walkable top height at y=0, and continuous material across thirds; left and right ends must also join seamlessly when looped. Thin clearly readable light top edge, darker vertical stone face below. Opaque material all the way to the bottom. Deliberate hand-painted grounded realism with restrained grain matching cold desaturated abandoned survival environments. Small subtle variations in wear across the strip, no large repetitive symbols or distinctive holes, no large masonry pattern. No characters, obstacles, grass neon or beige bricks. Material: heavy wet cracked grey-green stone and abandoned concrete in a contaminated swamp. Narrow compact layer of dirty desaturated olive moss along the top 10 percent, distinct muted pale sage-grey walkable rim, wet dark grey structural face below with sparse small roots and streaks of mud. NOT a thick grass lawn, no bright green blades. No neon, no yellow-green saturation. Restrained earthy brown and dirty olive accents. Cold diffuse light from upper left. This is a full-canvas texture strip, no floating object and no scenery or ground below it.
```

Saídas originais em `/home/antonio/.codex/generated_images/01a0732a-6184-72a1-8285-da006b244562/`: `exec-adf2c80e-dfb2-4e4d-aa06-58ef94a17c79.png` e `exec-f71047c8-aeae-4d72-8a4a-de1991197eff.png`. Os PNGs tratados estão no projeto, sem dependência dessa pasta externa.

## Sistema tipográfico

- **Teko Bold:** títulos em caixa alta, espaçamento moderado, sombra curta e contorno de um pixel; `ZOMBIES++` continua sendo texto SFML, com os dois sinais legíveis.
- **Barlow Condensed Medium:** opções, instruções, nomes, resultados, slots, avisos e HUD. Textos comuns marfim, secundários cinza dessaturado, seleção oliva clara sobre painel escuro e marcador lateral.
- **DejaVu Sans:** continua disponível pela preferência de fonte legível; alto contraste e escala são preservados.

As fontes novas são distribuídas sob SIL OFL 1.1. Arquivos e atribuições em [créditos das fontes](../Design/fonte/CREDITOS.md). As fontes antigas continuam no projeto. Não houve geração do título como imagem nem modificação dos arquivos das fontes.

Implementação: `src/neve.cpp`, `src/musgo.cpp`, `Recursos/plataforma.h`, `src/tipografia.cpp`, `Interface/preferencias.h`, `Interface/tema.h` e os pontos de apresentação dos menus, preferências, ranking, nome, HUD e avisos. A segunda rodada aumentou legendas pequenas do HUD. A lógica do jogo permanece intacta.

## Validação

A compilação, os testes de lógica e integração e o percurso visual passaram. Comandos para reproduzir:

```sh
make -j4 all build/remaster-visual
make test-logica
ALSOFT_DRIVERS=null xvfb-run -a make -j4 test-integracao
ALSOFT_DRIVERS=null xvfb-run -a -s '-screen 0 1600x1200x24' ./build/remaster-visual
```

O percurso gera capturas das duas fases solo/dupla, menu principal, pausa, ranking, seleção de jogadores/fase, slots, preferências, vitória/derrota e acessibilidade. Verifica também que salvar/carregar conserva a geometria e a seleção dos recortes e que as fontes têm glifos para `+`, números e acentos portugueses. Dados de teste ficam em pasta temporária.

A conferência visual ocorre em janela virtual Linux. Não equivale a uma partida manual completa ou teste nativo Windows. Caixas, espinhos, corações e personagens ainda mantêm suas artes anteriores, conforme o escopo desta etapa.

Capturas finais: `/tmp/zombies-remaster-6566287828690/`. O executável principal também foi aberto por cinco segundos em Xvfb sem falha de carga, encerrado pelo limite de tempo. O ambiente virtual avisa sobre contexto de entrada Unicode e sincronização vertical indisponíveis; os testes de glifos passaram.
