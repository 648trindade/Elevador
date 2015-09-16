# Elevador
Trabalho de Sistemas Operacionais "A" - UFSM 2015

**Data de entrega:** ASD

**Método de entrega:** coloque os arquivos fonte, entradas e saídas em um diretório chamado t1-fulano-sicrano, comprima o diretório (de preferência .tgz), anexe o resultado a um mail e envie para benhur+so@inf.ufsm.br. O nome do arquivo deve ser t1-fulano-sicrano.c e o assunto do mail deve ser t1-fulano-sicrano, com fulano-sicrano substituído pelo login do(s) aluno(s). O trabalho pode ser feito em grupos de até dois alunos.

Implemente um programa na linguagem C que simula um elevador. O programa deve usar threads (biblioteca pthreads), no mínimo uma para simular o comportamento do elevador e uma para simular o comportamento de cada pessoa. A sincronização das threads deve sem implementada com algo próximo de monitores (com mutex e variáveis de condição). O programa deve ler um arquivo no formato descrito a seguir, que contém o comportamento de cada pessoa. O programa deve produzir sua saída conforme descrito a seguir.

O elevador encontra-se em um prédio com 5 andares (o térreo e mais quatro andares). O elevador tem capacidade para transportar no máximo 3 pessoas por vez. No interior do elevador tem 5 botões com luzinha, usados para que as pessoas que estão no interior do elevador indiquem o andar para onde querem ir. Em cada andar, no exterior do elevador, existem dois botões com luzinha, para que as pessoas que estão no exterior do elevador possam chamá-lo, para descer ou subir. O elevador tem uma porta, que permanece aberta enquanto ele não está em movimento. Cada vez que o elevador abre a porta abre, permanece com ela aberta por um tempo mínimo (não fecha a porta antes que esse tempo mínimo tenha transcorrido, mas pode esperar mais caso não tenha clientes). O elevador se movimenta em resposta aos botões. Um movimento envolve fechar a porta, escolher um andar destino, movimentar-se até lá, apagar as luzes dos botões do andar destino, abrir a porta nesse andar. O elevador tem uma direção de movimento (subindo ou descendo). O andar destino é o mais próximo ao andar corrente que seja maior ou igual ao andar corrente se estiver subindo ou que seja menor ou igual ao andar corrente se estiver descendo, que tenha o botão correspondente (interno ou externo) aceso. Caso não haja andar destino no sentido corrente, o elevador inverte seu sentido. Caso não haja nenhum botão acionado, o elevador permanece parado sem direção definida. Com o elevador parado sem direção definida, o primeiro botão acionado define sua direção.

São simuladas N pessoas (identificadas por números entre 1 e N). Inicialmente, todas as pessoas estão no térreo, e têm uma lista de andares (que não inclui o térreo) a visitar. Para cada andar a visitar, a pessoa tem um certo tempo de duração da visita. Para cada visita, a pessoa aproxima-se do elevador, aperta o botão para chamar o elevador na direção correta, espera ele abrir a porta na direção correta, entra no elevador (respeitando o limite de carga), aperta o botão do andar destino, aguarda a porta abrir nesse andar, sai do elevador, aguarda o tempo da visita. Após concluir sua lista de visitas, volta ao térreo e morre. Um arquivo contém as informações de visitas de cada pessoa. A primeira linha do arquivo contém o número de pessoas. As demais (uma para cada pessoa) contêm o número de visitas que essa pessoa deve realizar seguido por pares de números que representam cada visita, com o andar a visitar e o tempo mínimo (em ns) que a visita deve durar. Os números em uma mesma linha são separados por espaço.

Cada thread (elevador e pessoas) deve registrar suas ações em um buffer. Para evitar a contenção na disputa pelo acesso a um buffer compartilhado, deve existir um para cada thread. Esses buffers devem ser gravados em um arquivo no final da execução do programa (quando todas as threads das pessoas tiverem terminado sua execução). Cada registro contém a data em que ocorreu, o tipo do registro e o andar.

A data é o número de nanosegundos desde um ponto no início da execução do programa.

O tipo do registro é um caractere:

| Letra   | Descrição                                                          |
| ------- | ------------------------------------------------------------------ |
| **A**   | a porta foi aberta no andar especificado                           |
| **F**   | a porta foi fechada no andar especificado                          |
| **B**   | a thread bloqueou (o andar é ignorado)                             |
| **E**   | a thread acordou (o andar é ignorado)                              |
| **M**   | a thread morreu (o andar é ignorado)                               |
| **S**   | foi apertado botão para subir no andar especificado                |
| **D**   | foi apertado botão para descer no andar especificado               |
| **I**   | foi apertado o botão interno correspondente ao andar especificado  |
| **sdi** | foi apagado o botão (como em SDI acima)                            |
| **N**   | pessoa entrou no elevador no andar especificado                    |
| **V**   | a pessoa saiu do elevador no andar especificado                    |

No início da execução de cada thread deve ter um evento E; no final, um M.

O andar é um número entre 0 e 4

Os registros são gravados um por linha no arquivo de saída. Cada linha inicia com a identificação da thread que registrou o evento (0 para o elevador, 1 a N para as pessoas), seguido da data, do tipo e do andar, separados por espaço.
