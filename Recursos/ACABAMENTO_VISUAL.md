# Acabamento: encaixes, ranking e tipografia

## Blocos horizontais e verticais

`src/encaixe_plataformas.cpp` compõe os tiles em memória a partir das texturas existentes. Uma máscara de quatro vizinhos seleciona 16 combinações de topo, miolo, laterais, base e cantos para neve e musgo. Blocos cobertos não exibem neve/vegetação entre as linhas. Bordas externas têm luz e sombra discretas; bordas internas são misturadas para suavizar as emendas verticais.

O encaixe é calculado ao criar ou restaurar a fase. As texturas são compartilhadas em cache por material, trecho e máscara, sem composições por quadro e sem consumir números aleatórios. A consulta usa a posição existente dos blocos; não altera mapas, caixas físicas ou regras de superfície. Saves com transformações fora da grade regular mantêm sua apresentação individual. A escolha visual é reconstruída após a carga, sem novos campos no save.

As imagens originais desta etapa não foram alteradas. As versões anteriores continuam preservadas no diretório `legacy` da etapa anterior.

## Ranking

Painel de registro de sobreviventes com cabeçalho, categoria, regra de classificação e colunas de posição, nome/equipe, pontos e tempo. Os números são alinhados à direita. Linhas alternadas, separadores e destaque no primeiro colocado organizam a lista. A altura do painel acompanha a quantidade de registros, com área mínima para o estado vazio.

As consultas e critérios de ordenação permanecem os mesmos. O desenho tem limites independentes por coluna para comportar nomes longos, pontuações altas, até oito resultados e escala 130%. Estado vazio e falha de consulta têm mensagens distintas; não são criados resultados fictícios na interface.

## Tipografia

Black Ops One dá aos títulos uma linguagem stencil industrial, sem sangue ou efeitos deformados. Barlow Semi Condensed Medium substitui a versão mais estreita nos textos funcionais. O texto selecionado continua oliva claro; sombra curta e contorno de meio pixel ficam reservados aos títulos.

A opção de acessibilidade continua disponível: texto funcional DejaVu, títulos Teko sem stencil. A aplicação genérica da preferência não substitui mais o título pela fonte de corpo. Fontes novas sob SIL OFL 1.1, com [licença e créditos](../Design/fonte/CREDITOS.md). Todos os títulos continuam renderizados por SFML.

## Validação

- `make -j4 all build/remaster-visual`: passou.
- `make test-logica`: passou.
- `ALSOFT_DRIVERS=null xvfb-run -a make -j4 test-integracao`: passou.
- Percurso visual em Xvfb: ambas as fases solo/dupla, principal, pausa, preferências, jogadores, fases, slots, resultado e ranking.
- Teste dos 16 encaixes em cada material: estado serializado intacto e mesma textura compartilhada após salvar/carregar.
- Ranking conferido vazio, com um e oito registros, por tempo/pontos, dupla com nomes longos e pontuação de dez dígitos, inclusive alto contraste e escala 130%.
- Fontes verificadas com números, sinais `+` e acentos portugueses; modo legível conserva títulos Teko.

Capturas finais em `/tmp/zombies-remaster-8826197737545/`. O teste pode recriá-las com `ALSOFT_DRIVERS=null xvfb-run -a -s '-screen 0 1600x1200x24' ./build/remaster-visual`. A segunda rodada suavizou emendas dos blocos após inspeção.

A verificação ocorreu em janela virtual Linux, sem uma partida manual completa nem teste nativo Windows. Xvfb informa indisponibilidade de contexto de entrada Unicode e sincronização vertical; as capturas e os testes de glifos foram concluídos. Demais sprites, backgrounds, controles e lógica das fases permanecem preservados.
