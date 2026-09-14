# Jogabilidade e controles

## Objetivo da fase

O HUD informa quantos inimigos continuam vivos. Quando restam até três, mostra
a direção do inimigo mais próximo de cada jogador, incluindo acima/abaixo.
A indicação considera cada jogador separadamente, inclusive em tela dividida.

## Movimento

O salto aceita comandos até 100 ms antes de aterrissar ou após deixar uma borda.
A janela é consumida no salto: não concede um segundo salto no ar. Seus contadores
fazem parte do save. O impulso e a gravidade permanecem iguais.

No musgo, a velocidade horizontal apoiada é limitada a 2,2 unidades por passo,
contra máximo normal de 4. Aceleração e frenagem continuam funcionando; o
personagem não sofre mais a divisão contínua de velocidade que o deixava quase parado.
A física do gelo continua com aceleração 0,07 e freio 0,02.

Inimigos apoiados verificam piso e paredes à frente antes de avançar. Arqueiros
só preparam tiros para alvos sem obstáculos sólidos na trajetória horizontal.
Essa percepção usa a grade existente e é recalculada a cada passo, sem congelar
inimigos distantes. Quedas durante saltos continuam sujeitas à física.

## Checkpoints

A cada nova região de 4000 unidades, o jogo procura uma oportunidade segura
para gravar um checkpoint. A verificação ocorre uma vez por segundo: todos os
jogadores precisam estar vivos, apoiados, na mesma região e longe de inimigos,
flechas e espinhos. Um aviso temporário confirma a gravação. Não há garantia
de checkpoint imediatamente ao cruzar a fronteira durante combate.

O snapshot completo do checkpoint acompanha o save do slot. Pausar e continuar
preservam a posição exata atual; fechar o jogo não perde o checkpoint já salvo.
Saves antigos continuam carregáveis e passam a ganhar checkpoints durante a partida.

Após a derrota, F2 na tela de nome recupera o último checkpoint, antes de confirmar
o resultado. A recuperação restaura entidades, itens e pontos daquele instante,
mas preserva o tempo gasto. A tentativa passa a ser assistida e não participa de
nenhum ranking. Isso impede acumular novamente recompensas para competir.
A condição assistida também acompanha a próxima fase cooperativa.

Se a recuperação não puder ser gravada, o estado anterior é preservado e o erro
é mostrado. Checkpoints inválidos são rejeitados antes de substituir o mundo.

## Teclado e controles físicos

Durante a pausa, pressione C para abrir Controles. Use setas e Enter para escolher
uma ação, depois pressione a nova tecla. Esc cancela a captura ou volta à pausa.
Teclas já utilizadas por outra ação e Esc são recusadas. A última opção restaura
o padrão. As preferências ficam em `controles.json`, ignorado pelo Git.

Padrão solo: WASD, setas e espaço. Remapear J1 desabilita os atalhos extras do solo,
para que o novo mapeamento seja inequívoco. No modo dupla, J1 usa WASD e J2 usa setas.

Os controles físicos de índices 1 e 2 comandam J1 e J2 respectivamente. O eixo
horizontal ou direcional move, o eixo vertical para baixo acelera a descida,
e um botão pula. O botão de pulo de cada controle pode ser escolhido na mesma tela.
A zona morta é de 25%. Menus continuam operados pelo teclado.

## Estrutura e validação

A interface lê dispositivos e entrega `Logica::Comandos` por passo. Jogador e
simulação não consultam mais o teclado, permitindo reproduzir entradas em testes.
Apenas E01 foi aplicada da lista estrutural; Singleton e padrões existentes permanecem.

`make test-logica` e `make test-integracao` incluem controles, comandos,
percepção de terreno e checkpoints, além das suítes anteriores. Os testes verificam
persistência, falhas de escrita, proteção dos rankings, navegação e combate.
`make build/gameplay-visual` gera o executável de capturas do HUD, pausa e controles;
executá-lo grava imagens na pasta temporária `zombies-gameplay-visual`.

Os testes de controle físico usam entradas simuladas; uma sessão com hardware
real ainda é necessária para avaliar dispositivos específicos. A travessia integral
automatizada verifica navegabilidade; não substitui avaliação humana de dificuldade.
