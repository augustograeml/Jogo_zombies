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
| Espinhos | aproximadamente 24,9 × 32 |
| Cura | aproximadamente 26,7 × 24 |
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
4, impulso vertical 6 e gravidade 0,1 por passo. Neve conserva freio 0,035;
no musgo o modificador de apoio continua dividindo a velocidade por 1,5
(aproximadamente 0,2 de velocidade sustentada no chao).

Simulando a propria funcao `Logica::mover`:

| Saida | Altura maxima | Alcance ao aterrissar 100 unidades acima |
|---|---:|---:|
| Lenta, 0,2 | 183 | 337,7 |
| Velocidade 4 | 183 | 408 |

As ligacoes verificadas limitam a subida a 100 (55% da altura maxima) e o
deslocamento de lancamento/aterrissagem a 250 (74% do alcance lento). Os vaos
principais medem 100 e os patamares principais 500–800 unidades, com uma area
inicial de 600. O teste busca trajetorias reais, com aceleracao e frenagem,
usando o resolvedor do jogo, caixas e espinhos ativos. Nao basta comparar centros.

## Montanha

Percurso principal: superficies em y=900 → 800 → 700 → 800 → 700 → 800,
seguindo x=0 ate 4000. Duas pequenas elevacoes com cura permitem desvios.
O primeiro zumbi fica afastado dos spawns; caixas introduzem obstaculos baixos.
Arqueiros ocupam os patamares elevados, com aproximacao pelos lados e espaco
para salto. Espinhos ficam fora da estreita aterrissagem apos o primeiro desvio.
A arena final tem 800 unidades, cura de preparacao e um unico gigante.

## Pantano

Percurso: y=900 → 800 → 850 → 750 → 650 → 750. Alterna descida, retomada de
altura e dois desvios de cura. O primeiro arqueiro aparece antes do da montanha,
mas ha uma aproximacao larga. A caixa que estreitava a saida do desvio central
foi transferida para a aproximacao da arena. Espinhos ficam apos uma zona de
aterrissagem de mais de 150 unidades. A arena tambem tem apenas um gigante.

Ambas possuem base de recuperacao em y=1000 e degraus de retorno: perder um salto
nao significa automaticamente perder a partida. Inimigos que caem continuam
acessiveis. O objetivo continua eliminar os inimigos, inclusive na base.
A largura util e 4000; o terreno termina em y=1100. A derrota por queda usa essa
borda inferior mais 600. Personagens respeitam as bordas horizontais. O teto e
livre para os saltos; projeteis sao descartados fora da margem do mundo.

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
um gigante por fase e acesso a todos os patamares elevados, do inicio a arena.
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
patamares. Injeta somente controles e exige passar por 15 pontos ate o fim da
arena, sem dano. Mantem caixas e espinhos; isola a IA e desativa curas para nao
mascarar dano. Complementa a prova de saltos isolados com a sequencia completa.
Use `ZOMBIES_CAPTURAR_PERCURSO=1` ao executar `build/percurso-continuo` para gerar
capturas de inicio, meio e fim na pasta temporaria `zombies-percurso-continuo`.

A validacao jogavel e automatizada por trajetorias e encontros; nao representa
uma partida manual continua de ponta a ponta. As capturas foram inspecionadas
visualmente. Os testes nao avaliam subjetivamente o ritmo/dificuldade como um
playtest humano, mas protegem alcance, espaco, colisoes, persistencia e cobertura.
