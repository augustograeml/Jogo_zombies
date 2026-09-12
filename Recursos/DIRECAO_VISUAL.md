# Zombies++ — direção visual da remasterização

## Bíblia visual

Mundo: áreas de evacuação abandonadas após um surto, com concreto, aço oxidado e vegetação invasiva. Horror ambiental sem grotesco. Ilustração 2D de formas legíveis, textura discretamente granulada e detalhes econômicos; perspectiva plausível e luz fria difusa.

Paleta compartilhada: carvão #101715, concreto #87948A, oliva #91A77A, marfim #E5E9DF; ferrugem #854D43 usada com moderação. Neve: branco sujo e azul ardósia. Pântano: musgo, lama e cinza esverdeado. Sem neon, personagens de fundo, letras, plataformas falsas ou UI pintada nas imagens.

Composição: fundos de gameplay com região central pouco contrastada e detalhes nas camadas distantes. Menus quadrados com espaço negativo central. Telas de leitura têm ambientes mais escuros e painéis desenhados em C++.

Tipografia (segunda etapa): Black Ops One para títulos stencil e Barlow Semi Condensed Medium para leitura funcional, ambas SIL OFL. DejaVu permanece como alternativa de acessibilidade. Seleção com marcador lateral e texto oliva claro; opções inativas carvão/marfim. Alto contraste e escala continuam disponíveis.

## Produção

Oito fundos gerados pela ferramenta integrada de imagens. Prompts usam a bíblia acima e especificam, por tela: montanha nevada isolada; pântano contaminado; acesso industrial de evacuação; duas posições de equipamento; díptico neve/pântano; interior de abrigo escuro; arquivo de evacuação; sala de comunicação neutra para resultados.

Os arquivos originais ficam em `Design/imagens/legacy/`. Os fundos novos são PNG. O desenho usa recorte proporcional, sem distorcer a imagem. Sprites e colisões não são redesenhados nesta etapa.

## Segunda etapa possível

Jogadores, inimigos e objetos ainda têm estilos de origens distintas. Uma futura revisão de sprites poderia unificá-los com os fundos; isso não faz parte desta entrega.
