# Escala, percursos e camera

## Unidade e representacao

O tile continua com 50 unidades. A referencia humana tem 80 unidades (1,6 tiles).
`Recursos/escala.h` centraliza os tamanhos de novas entidades.

| Entidade | Caixa fisica (largura × altura) |
|---|---:|
| Jogador 1 e Luigi | 40 × 80 |
| Zumbi comum | 48 × 88 |
| Arqueiro | 38,4 × 84 |
| Gigante | 88 × 140 |
| Caixa | aproximadamente 38,7 × 38,4 |
| Espinhos | aproximadamente 28 × 36 |
| Cura | aproximadamente 40 × 36 |
| Flecha | 24 × 5 |

Personagens usam o recorte visivel dos quadros existentes, escala uniforme e
alinhamento pelos pes. A caixa cobre o tronco e a altura; maos, armas e poses
podem ultrapassar sua largura. Os efeitos de inclinacao permanecem, mas as poses
nao comprimem/esticam os eixos separadamente. Objetos estaticos recebem recorte
alpha, dimensoes proporcionais e uma caixa correspondente ao recorte.

Marcadores do mapa representam a celula acima do apoio. O construtor centraliza
cada entidade nessa celula e assenta seus pes em sua borda inferior, mesmo para
o gigante. `1` e `2` agora definem os pontos iniciais. A cura e um sensor: coleta
sem empurrar personagens ou bloquear flechas. Pisoes, contato e projeteis usam as
novas caixas; as flechas partem da altura do arco e do lado de disparo.

Saves existentes mantem suas dimensoes, coordenadas e estados serializados.
Nao ha migracao destrutiva de partidas para o mapa novo. Novas partidas usam as
novas dimensoes/mapas; ao carregar, os limites sao derivados do terreno salvo.
Nenhuma arte foi substituida ou apagada nesta etapa.

## Base fisica do desenho

A simulacao continua a 60 passos/s, aceleracao horizontal 0,1, velocidade maxima
4, impulso vertical 6 e gravidade 0,1 por passo. Neve usa freio 0,02 e aceleracao/reversao 0,07;
no musgo o modificador de apoio continua dividindo a velocidade por 1,5
(aproximadamente 0,2 de velocidade sustentada no chao).

Simulando a propria funcao `Logica::mover`:

| Saida | Altura maxima | Alcance ao aterrissar 100 unidades acima |
|---|---:|---:|
| Lenta, 0,2 | 183 | 337,7 |
| Velocidade 4 | 183 | 408 |

As ligacoes verificadas limitam a subida a 100 (55% da altura maxima) e o
deslocamento de lancamento/aterrissagem a 250 (74% do alcance lento). Os vaos
principais medem 100 e os patamares principais 350–800 unidades, com uma area
inicial de 700. O teste busca trajetorias reais, com aceleracao e frenagem,
usando o resolvedor do jogo, caixas e espinhos ativos. Nao basta comparar centros.

## Fases ampliadas

Cada mapa passa de 4000 para 40000 unidades de largura. Sao 69 patamares
principais, 14 sacadas opcionais e 68 fundos de recuperacao nos vaos.
Ha 2970 tiles de terreno por fase (antes 244), 43 inimigos (antes 6),
30 curas (antes 4), 18 espinhos e 20 caixas. Os inimigos se dividem em
30 comuns, 10 arqueiros e 3 gigantes, distribuídos ao longo de dez regioes.

A montanha alterna subida gradual, cristas, descidas e retomada ate a arena.
As faixas geladas possuem espaco para reconhecer a derrapagem e frear; a
frenagem passiva desde velocidade maxima percorre aproximadamente 398 unidades.
O pantano alterna bacias, subidas entre ruinas e trechos elevados. O musgo
mantem o comportamento anterior. Os perfis verticais das fases sao distintos.

As sacadas ficam 150 unidades acima do piso, deixando 100 livres sob seus
50 de espessura: 25% de folga acima do jogador de 80. Os vaos de 100 possuem
fundo recuperavel 100 abaixo do menor piso adjacente. Nao ha corredor inferior
continuo que substitua a travessia principal. Caixas ficam fora da aterrissagem,
e os gigantes ocupam plataformas largas nas regioes 4, 7 e 10.

O objetivo continua eliminar os inimigos. Os limites vem do terreno salvo;
partidas antigas preservam seu mapa, entidades e dimensoes. Os novos mapas
cabem nos limites existentes de entidades e tamanho do arquivo de save.

## Camera e background

A causa da faixa preta era o desenho de um retangulo fixo usado tambem como
limite de camera. Agora `desenhar_cenario` desenha a textura original para cada
view, com escala uniforme **cover**, centralizada na camera. O excesso e
recortado pela viewport. Nao depende de tamanho/posicao do level e nao usa uma
cor de preenchimento para disfarcar falta de imagem.

Os limites preferenciais da camera vem do terreno. Uma margem de enquadramento
prioriza mostrar o corpo inteiro e permite sair suavemente desses limites.
Compartilhamento, histerese e orientacoes da camera dupla sao preservados.
As barras de enquadramento fora da area de gameplay em janelas nao quadradas
continuam intencionais; nao sao falta de background dentro de uma viewport.

## Validacao reproduzivel

- `make clean` e `make -j4`.
- `make test-logica`.
- `ALSOFT_DRIVERS=null xvfb-run -a make test-integracao`.
- `make build/mundo-visual` e `ALSOFT_DRIVERS=null xvfb-run -a ./build/mundo-visual`.

`escala-mundo`, incluido em `test-integracao`, verifica proporcoes, save/load,
coleta sem deslocamento, pisoes nos tres inimigos, spawns sem sobreposicao,
tres gigantes por fase e acesso a todos os patamares elevados, do inicio a arena.
Os saltos devem chegar sem dano, em solo e com dois jogadores lado a lado.
A base inferior e uma area de recuperacao, nao uma plataforma obrigatoria de
aterrissagem em cada ligacao. A prova de alcance isola a IA dos inimigos; os
ensaios de combate e as suites de comportamento cobrem os encontros separadamente.

`apresentacao` compara pixels de um background de referencia em todas as
viewports, inclusive muito acima/abaixo/lateralmente fora do level. `camera`
verifica enquadramento do corpo no topo e continuidade ao atravessar a borda.
`mundo-visual` abre SFML real, captura ambas as fases em solo/dupla, inicio,
meio, arena, separacoes horizontal/vertical, altura extrema e resolucoes
1024×1024, 1280×720 e 720×1280. Tambem gera duas comparacoes de escala.
As capturas ficam em `/tmp/zombies-mundo-visual` e o desenho nao pode alterar o save.

`percurso-continuo`, tambem incluido na integracao, percorre cada fase inteira
em uma unica execucao, em solo e dupla, sem reposicionar os jogadores entre
patamares. Injeta somente controles e exige passar por 152 pontos ate o fim da
arena, sem dano. Mantem caixas e espinhos; isola a IA e desativa curas para nao
mascarar dano. Complementa a prova de saltos isolados com a sequencia completa.
Use `ZOMBIES_CAPTURAR_PERCURSO=1` ao executar `build/percurso-continuo` para gerar
capturas de inicio, meio e fim na pasta temporaria `zombies-percurso-continuo`.

A validacao jogavel e automatizada por trajetorias e encontros; nao representa
uma partida manual continua de ponta a ponta. As capturas foram inspecionadas
visualmente. Os testes nao avaliam subjetivamente o ritmo/dificuldade como um
playtest humano, mas protegem alcance, espaco, colisoes, persistencia e cobertura.

## Resultados da ampliacao

A travessia continua automatizada percorreu ambas as fases em solo e dupla,
sem dano, em aproximadamente 18,6 mil passos por percurso (cerca de dez vezes
o percurso anterior). Esse tempo isola navegacao; nao estima uma partida com combate.
A auditoria verifica todos os intervalos verticais entre terrenos, saltos dos
69 patamares, sacadas, retorno dos vaos e caminhada sob uma passagem de 100.

`expansao`, na integracao, salva e retoma os quatro modos perto de x=39500,
incluindo flecha em voo, inimigo morto, cura coletada, pontuacao e tempo.
Compara a evolucao original/restaurada e a grade espacial com a busca completa.
Saves medidos: aproximadamente 1,68 MB. Neste ambiente de teste, a simulacao
levou 0,36–1,16 ms por passo e o desenho estabilizado ficou em 70–101 FPS.
Essas medicoes variam com o equipamento; nao sao garantia em outros computadores.
A grade evitou mais de 99% dos pares teoricos. Todos os inimigos continuam
simulados; nao foi necessario introduzir congelamento de entidades distantes.
