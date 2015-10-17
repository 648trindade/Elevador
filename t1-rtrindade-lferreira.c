#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lista.h"

/*	=== Pessoa ===
	(Objeto privado) Estrutura representando uma pessoa.
	Contém uma variável (status) indicando o estado da pessoa: 
	*	0:	Visitando o andar;
	*	1:	Dentro do elevador;
	*  -1:	Esperando o elevador;
*/
typedef struct{
	int id;				// Número da pessoa;
	int andar;			// Andar que ele se encontra;
	int status;			// Estado atual da pessoa;
	lista_t dest;		// Vetor de destinos da pessoa;
	lista_t time;		// Vetor de tempo de visita em cada andar destino;
}pessoa;

/*	=== Elevador ===
	(Objeto compartilhado) Estrutura representando o elevador.
	Contém um vetor de destinos (dest) que deve ser ordenado a cada novo
	valor inserido, dependendo da direção em que ele está se movimentando:
	*	subindo:	ordenado em ordem crescente (andares mais baixos primeiro)
	*	descendo:	ordenado em ordem decrescente (andares mais altos primeiro)
	O elevador também deve parar nos andares em que alguém pressionou um botão
	para seguir na mesmo direção que o elevador, mas somente se não estiver
	lotado (qtd_p <= 3).
	O elevador também tem uma flag (status) indicando sua movimentação
	*	0:	parado
	*	1:	subindo
	*   2:	descendo
*/
typedef struct{
	int qtd_p;			// Número de pessoas dentro do elevador;
	int andar;			// Andar que ele se encontra;
	lista_t dest		// Lista contendo as direções (possivelmente tenha que ser global)
	lista_t pedidos[2];	// Listas contendo pedidos (0 subir, 1 descer)
	int pessoas[3];		// vetor com número das pessoas dentro
	int status;			// Estado do elevador;
}elevador;


void *func(void *arg){
	pessoa *pes = (pessoa*)arg;
	printf("cocozao\n");
	pthread_exit(NULL);
}

int main(){
	//Lendo
	int n,j;
	unsigned long i;
	scanf("%d",&n);
	pessoa p[n];
	// Descreve elevador
	elevador e;
	e.nmax = 3;
	e.n = 0;
	e.andar = 1;
	e.dest = NULL;
	e.status = 0;	// Parado
	//Lê pessoas
	pthread_t threads[n];
	for (i=0;i<n;i++){
		scanf("%d",&p[i].ndest);
		p[i].andar = 1;
		p[i].status = 0;
		p[i].time = malloc(p[i].ndest*sizeof(int));
		p[i].dest = malloc(p[i].ndest*sizeof(int));
		for(j=0;j<p[i].ndest;j++)
			scanf("%d %d",&p[i].dest[j],&p[i].time[j]);
		pthread_create(&threads[i],NULL,func,(void*)&p[i]);
	}
	for (i=0;i<n;i++){
		pthread_join(threads[i],NULL);
	}
	//pthread_exit(NULL);
	//libera alocacoes
	for (i=0;i<n;i++){
		free(p[i].time);
		free(p[i].dest);
	}
	printf("coco\n");
	return 0;
}