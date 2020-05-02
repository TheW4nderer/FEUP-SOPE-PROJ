# Gestão de acesso a quartos-de-banho

Este projeto tem como objetivos desenvolver um programa multithread, promovendo a intercomunicação entre processos através de canais com nome (pipes ou FIFOs). É também importante evitar conflitos entre os processos, através de mecanismos de sincronização (a implementar na segunda parte do projeto).

## Programa do utente (Un)

O programa do utente é responsável por efetuar os pedidos de utilização da casa de banho ao servidor (Qn) que os recolhe e processa.
Numa fase inicial, recolhe os argumentos da linha de comandos (número de segundos de execução e nome do FIFO público através do qual se comunica com o servidor).
Durante a execução deste programa, são geradas threads  com intervalo de 20 milissegundos entre a criação de cada thread. O programa termina após um determinado número de segundos dado pelo argumento nsecs, recebido da linha de comandos, ou até que a casa de banho encerre (o programa Qn termina).
Cada thread é responsável por um pedido, executando os seguintes passos:

- Gerar de forma aleatória a duração do pedido, tentando, de seguida, abrir o FIFO público cujo nome foi dado pelo utilizador. Em caso de erro ao abrir o FIFO público, a variável closed será ativada e, portanto o programa Un deixa de gerar novos pedidos e termina (operação CLOSD).
- Em caso de sucesso no estabelecimento da comunicação entre o processo e o servidor, a thread escreve no FIFO público informação acerca do pedido e fecha a ligação ao FIFO público.
- A thread cria um canal privado através do qual se comunica com o servidor. Caso a thread não conseguir obter resposta do servidor, é passado para stdout informação acerca da operação FAILD.
- Em caso de sucesso da comunicação, a thread analisa a informação do servidor relativa à duração e id do lugar atribuídos ao pedido. Caso sejam ambos iguais a -1, o pedido é rejeitado pelo servidor, pois a utilização do servidor ultrapassa o fecho da casa de banho (2LATE).
- Em caso de aceitação do servidor, deve ser mostrado no ecrã a informação do pedido (IAMIN).
- No final, a thread fecha o FIFO e destrói o mesmo.

## Programa do servidor (Qn)

Tal como especificado no enunciado, o programa do servidor lança threads para cada pedido dos clientes.
Cada thread é responsável por executar o seguinte conjunto de passos:

 - Abrir o FIFO público de comunicação entre o servidor e os clientes e extrair uma mensagem (que corresponde a um cliente). Cada mensagem possui informação acerca de: id do cliente (sequencial), pid do cliente, tid da thread do cliente, duração da utilização que foi gerada aleatoriamente pelo cliente e o lugar atribuído ao pedido (que é passado com o valor -1 pelo cliente).
 - O FIFO público é fechado pela thread, que tenta, de seguida, abrir o FIFO privado criado pelo cliente. Em caso de erro, a thread retorna, dado que o cliente desistiu (GAVUP).
 - Em caso de sucesso na comunicação entre a thread e o FIFO privado, o servidor processa o pedido do cliente, verificando se este ultrapassa o período de funcionamento da casa de banho. Se este for o caso o pedido é rejeitado
 - Se o pedido do cliente foi rejeitado pelo servidor, a thread escreve para o FIFO privado uma mensagem no mesmo formato da mensagem recebida, alterando os valores de place e duração para -1, para que a informação acerca da rejeição do pedido seja mostrada pelo cliente
 - Caso o pedido tenha sido aceite, a mensagem é enviada, porém o valor da duração não se altera e o valor do lugar é incrementado.
 - No final da utilização da casa de banho, é enviado para stdout a informação acerca da operação TIMUP.

## Informações adicionais

Existe no diretório entregue um makefile responsável pela compilação simultânea de ambos os programas.


