# Recursos do jogo

O código continua exclusivamente em arquivos `.cpp` e `.h`. Imagens, fontes e mapas permanecem como dados em `Design`; nenhuma arte foi removida ou substituída.

## Localização

`Recursos::caminho` aceita caminhos absolutos e caminhos relativos, com ou sem o prefixo `Design/`. A busca considera, nesta ordem:

1. `ZOMBIES_RECURSOS`, quando definida, apontando para a pasta do projeto ou diretamente para `Design`;
2. a pasta atual e seus ancestrais;
3. a pasta do executável e seus ancestrais, usando o caminho do processo no Linux ou no Windows.

A busca sobe no máximo oito níveis. Assim, o executável em `build` ou na raiz pode ser iniciado por outra pasta. Ao distribuir o jogo, mantenha `Design` junto ao executável. A resolução dos recursos não muda a pasta atual nem redireciona os arquivos de partidas e ranking. Caminhos inexistentes geram erros com o nome do arquivo procurado; uma variável de ambiente inválida não é silenciosamente ignorada.

`Recursos::textura` mantém um cache forte por caminho: a textura continua residente e reutilizável entre entidades. Fontes e fundos acessam o mesmo resolvedor de caminhos. Os quadros de corrida são carregados na primeira criação do jogador 1, antes da simulação, e não são relidos a cada passo.

## Personagens e animação

| Uso | Arquivos | Observações |
| --- | --- | --- |
| Corrida do jogador 1 | `imagens/op1.png` até `imagens/op27.png` | Ordem numérica explícita; os 27 quadros foram conferidos em conjunto. |
| Repouso do jogador 1 | `imagens/op10.png` | Pose fixa da mesma sequência; mantém a última direção. |
| Jogador 2 | `imagens/luigiDireita.png` | Anima pernas e braço com malha articulada em C++; preserva a imagem original e espelha a direção. |
| Artes alternativas | `imagens/andando.png`, `imagens/parado.png` | Pertencem a outro personagem e não entram na sequência. |
| Caminhada dos inimigos | `imagens/zumbi_corrida_atlas.png`, `imagens/arqueiro_corrida_atlas.png`, `imagens/gigante_corrida_atlas.png` | Oito poses por personagem, carregadas e isoladas uma vez. |
| Inimigos originais | `imagens/zumbi_bateu_morreu.png`, `imagens/zumbi_atirador.png`, `imagens/zumbi_gigante.png` | Zumbi, arqueiro e gigante. |
| Obstáculos e cura | `imagens/plataforma.png`, `imagens/bloco_musgo.jpeg`, `imagens/espinho.png`, `imagens/caixa.png`, `imagens/saude.png` | Texturas reutilizadas pelas entidades do mapa. |

O catálogo calcula o retângulo dos pixels visíveis de cada quadro uma única vez. O desenho normaliza a altura, centraliza os pés na base da caixa física e espelha o sprite para a esquerda. A caixa de colisão continua independente: não muda ao trocar quadro, parar ou virar.

`Animacao::Corrida` é um controlador sem dependência gráfica. Ele avança exclusivamente pelos passos da simulação; desenhar ou permanecer no menu de pausa não o faz avançar. O estado persistido contém `correndo`, `quadro`, `passos` e `direita`. A carga valida seus limites. Partidas antigas sem esses campos recebem a pose parada padrão.

## Cenários

Os mapas ativos são `cenario/cenario_fase1.txt` e `cenario/cenario_fase2.txt`; os fundos correspondentes são `imagens/cenario_op11.png` e `imagens/cenario_op22.png`.

| Símbolo | Significado |
| --- | --- |
| Espaço | Vazio |
| `0` | Plataforma de neve |
| `1`, `2` | Marcadores reservados dos jogadores; o construtor define o nascimento |
| `3` | Zumbi |
| `4` | Arqueiro |
| `5` | Espinho |
| `6` | Coração |
| `7` | Plataforma de musgo |
| `8` | Caixa |
| `9` | Gigante |

`Recursos::validar_mapa` preserva espaços e linhas, aceita finais de linha Linux e Windows e rejeita caracteres desconhecidos com linha e coluna. Arquivos vazios, erros de leitura e mapas com mais de 4096 linhas ou colunas são rejeitados. Os arquivos auxiliares históricos em `Design/cenario` permanecem preservados.

## Fontes e interface

As fontes ficam em `Design/fonte`, incluindo `fonte_simas.ttf` e `sangue_escorrendo.ttf`. As imagens de menu, ranking, pausa e entrada de nome ficam em `Design/imagens`. Os arquivos históricos `rankingfase1.txt` e `rankingfase2.txt` servem à importação de resultados antigos; não são imagens.

## Parâmetros em C++

`Recursos/configuracao.h` concentra aceleração, freio, velocidade máxima, gravidade, impulso de salto e cadência da corrida. A configuração inicial usa 60 passos por segundo e dois passos por quadro, formando um ciclo de 27 quadros em 0,9 segundo. Alterações de cadência ou quantidade de quadros exigem revisar a compatibilidade do estado de animação persistido; não basta apenas trocar o número da constante.

A altura desenhada acompanha a caixa física do jogador. Mudar imagens não deve alterar dimensões físicas, parâmetros de movimento ou resultados de colisão.

## Origem das novas sequências

Os três arquivos `*_corrida_atlas.png` foram gerados com a ferramenta de imagens da OpenAI para esta atualização. O pedido foi manter o personagem de referência, suas cores e acessórios, com oito poses de caminhada organizadas em quatro colunas e duas linhas, sem texto. O arqueiro recebeu uma edição adicional de espaçamento: personagens completos, incluindo arco e flechas, separados por margens generosas.

`src/atlas.cpp` isola as oito maiores silhuetas e mantém as texturas em cache. Para exportações RGB com fundo neutro quadriculado, remove somente o fundo conectado às bordas durante a carga. O arquivo de imagem permanece intacto.

Luigi não recebeu imagem gerada: `src/animacao_articulada.cpp` transforma uma malha sobre `luigiDireita.png`. A pose parada permanece original; braços e pernas alternam durante a caminhada, sem mudar a colisão.
