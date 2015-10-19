#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lista.h"
#include <time.h>


pthread_mutex_t mutex_pedidos, mutex_print;

clock_t ini;
int bi = 1e9;

int tempo(){
	return (int)(clock()-ini);
}

void print(int id, char c, int a){
	pthread_mutex_lock(&mutex_print);
	printf("%d %d %c %d\n",id+1, tempo(), c, a);
	pthread_mutex_unlock(&mutex_print);
}

/*	=== Pessoa ===
	(Objeto privado) Estrutura representando uma pessoa.
	Contém uma variável (status) indicando o estado da pessoa: 
	*	0:	Visitando o andar;
	*	1:	Dentro do elevador;
	*   2:	Esperando o elevador;
*/
typedef struct{
	int id;				// Número da pessoa;
	int andar;			// Andar que ele se encontra;
	int status;			// Estado atual da pessoa;
	lista_t dest;		// Vetor de destinos da pessoa;
	lista_t time;		// Vetor de tempo de visita em cada andar destino;
	pthread_cond_t c;
	pthread_mutex_t m;
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
	lista_t dest;		// Lista contendo as direções
	lista_t pedidos[2];	// Listas contendo pedidos (0 subir, 1 descer)
	int pessoas[3];		// vetor com número das pessoas dentro
	int status;			// Estado do elevador;
}elevador;

elevador e;

void chama_elevador(pessoa *p, int dest){
	pthread_mutex_lock(&mutex_pedidos);
	int sentido;
	if (dest > p->andar){
		sentido = 0; 	//pessoa vai subir
		print(p->id, 'S', p->andar);
	}
	else{
		sentido = 1; 	//pessoa vai descer
		print(p->id, 'D', p->andar);
	}
	l_insere_ord(&(e.pedidos[sentido]), dest, sentido);
	pthread_mutex_unlock(&mutex_pedidos);
	p->status = 2;
}

void *acao_pessoa(void *arg){
	pessoa *p = (pessoa *) arg;
	print(p->id, 'E', 0);
	int p_dest, p_temp;
	struct timespec wait, rest;
	wait.tv_sec = 0;
	while (1) {
		// consulta proximo destino
		p_dest = l_retira(&(p->dest));
		// chama o elevador
		chama_elevador(p, p_dest);
		//espera ser acordado pelo elevador (elevador chegar no andar)
		pthread_mutex_lock(&(p->m));
		pthread_cond_wait(&(p->c),&(p->m));
		pthread_mutex_unlock(&(p->m));
		//entra elevador
		//TODO
		print(p->id, 'N', p->andar);
		p->status = 1;
		p->andar = p_dest;
		print(p->id, 'I', p->andar);
		//espera ser acordado pelo elevador (elevador chegar no destino)
		pthread_mutex_lock(&(p->m));
		pthread_cond_wait(&(p->c),&(p->m));
		pthread_mutex_unlock(&(p->m));
		//entra no andar e visita (espera)
		print(p->id, 'V', p->andar);
		p->status = 0;
		wait.tv_nsec = (long) l_retira(&(p->time));
		if (wait.tv_nsec != -1){
			print(p->id, 'B', 0);
			nanosleep(&wait, &rest);
			print(p->id, 'E', 0);
		}
		else break;
	}
	pthread_exit(NULL);
}

void *acao_elevador(void* args){
	//TODO
}

void cria_threads(pessoa p[], pthread_t p_thr[], int n){
	int i, j, d, a, t;
	for (i=0;i<n;i++){
		scanf("%d",&d);
		pthread_cond_init(&(p[i].c), 0);
		pthread_mutex_init(&(p[i].m), 0);
		p[i].id = i;
		p[i].andar = 0;
		p[i].status = 0;
		l_cria(&(p[i].time));
		l_cria(&(p[i].dest));
		for(j=0;j<d;j++){
			scanf("%d %d",&a,&t);
			l_insere(&(p[i].dest),a);
			l_insere(&(p[i].time),t);
		}
		l_insere(&(p[i].dest), 0);
		l_insere(&(p[i].time),-1);
		pthread_create(&p_thr[i], NULL, acao_pessoa, (void*)&p[i]);
	}
}

int main(){
	pthread_mutex_init(&mutex_pedidos, 0);
	//Lendo
	int n,j,i;
	scanf("%d",&n);
	pessoa p[n];
	//Lê pessoas
	pthread_t p_thr[n], e_thr;
	ini = clock();
	cria_threads(p, p_thr, n);
	
	// Descreve elevador
	e.qtd_p = 0;
	e.andar = 0;
	l_cria(&(e.dest));
	l_cria(&(e.pedidos[0]));
	l_cria(&(e.pedidos[1]));
	e.status = 0;	// Parado
	
	
	
	for (i=0;i < n; i++){
		pthread_join(p_thr[i],0);
		pthread_cond_destroy(&(p[i].c));
		pthread_mutex_destroy(&(p[i].m));
	}
	printf("JOIN!\n");
	
	j= e.pedidos[0].qtd;
	for (i=0;i < j; i++){
		printf("%d ",l_retira(&(e.pedidos[0])));
	}printf("\n");
	pthread_mutex_destroy(&mutex_pedidos);
	pthread_exit(0);
	return 0;
}