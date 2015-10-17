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
	int andar;			// Andar que ele se encontra;
	int status;			// Estado atual da pessoa;
	int *dest;			// Vetor de destinos da pessoa;
	int *time;			// Vetor de tempo de visita em cada andar destino;
	int ndest;			// Quantia de visitas a realizar (também tamanho do vetor)
}pessoa;

/*	=== Elevador ===
	(Objeto compartilhado) Estrutura representando o elevador.
	Contém um vetor de destinos (dest) que deve ser ordenado a cada novo
	valor inserido, dependendo da direção em que ele está se movimentando:
	*	subindo:	ordenado em ordem crescente (andares mais baixos primeiro)
	*	descendo:	ordenado em ordem decrescente (andares mais altos primeiro)
	O elevador também deve parar nos andares em que alguém pressionou um botão
	para seguir na mesmo direção que o elevador, mas somente se não estiver
	lotado (n < nmax).
	O elevador também tem uma flag (status) indicando sua movimentação
	*	0:	parado
	*	1:	subindo
	*  -1:	descendo
*/
typedef struct{
	int nmax;			// Número máximo de pessoas;
	int n;				// Número de pessoas dentro do elevador;
	int andar;			// Andar que ele se encontra;
	int *dest;			// Vetor contendo as direções (possivelmente tenha que ser global)
	int status;			// Estado do elevador;
}elevador;

/*	=== Quick Sort ===
	Parametros:
	*v		:	Vetor a ser ordenado
	low		:	Indice mais baixo
	high	:	Indice mais alto
	ordem	:	Ordem (1 Crescente, 0 Decrescente)
*/
void quick(int *v, int low, int high, int ordem){
	int i = low, j = high, swap;
	int mid = v[(i+j)/2];
	while (i<=j){
		while (((ordem)? v[i]<mid : v[i]>mid ) && i<high)
			i++;
		while (((ordem)? v[j]>mid : v[j]<mid ) && j>low)
			j--;
		if ((ordem)? v[i]>=v[j] : v[i]<=v[j]){
			swap = v[i];
			v[i] = v[j];
			v[j] = swap;
			i++; j--;
		}
	}
	if(i<high)
		quick(v,i,high,ordem);
	if(j>low)
		quick(v,low,j,ordem);
}



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