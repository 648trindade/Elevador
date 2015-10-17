#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lista.h"
#include <time.h>

pthread_mutex_t mutex_pedidos;
pthread_cond_t cond;
clock_t ini;
int bi = 1e9;

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
		printf("%d %d S %d\n",p->id,(int)((clock()-ini)*bi/CLOCKS_PER_SEC),p->andar);
	}
	else{
		sentido = 1; 	//pessoa vai descer
		printf("%d %d D %d\n",p->id,(int)((clock()-ini)*bi/CLOCKS_PER_SEC),p->andar);
	}
	l_insere_ord(&(e.pedidos[sentido]), dest, sentido);
	pthread_mutex_unlock(&mutex_pedidos);
}

void *acao_pessoa(void *arg){
	pessoa *p = (pessoa *) arg;
	int p_dest, p_temp;
	while (1) {
		// consulta proximo destino
		p_dest = l_retira(&(p->dest));
		p_temp = l_retira(&(p->time));
		// chama o elevador
		chama_elevador(p, p_dest);
		//TODO
		//pthread_cond_
		
		
		
		if (p->dest.qtd == 0)
			break;
	}
	pthread_exit(NULL);
}

int main(){
	pthread_mutex_init(&mutex_pedidos, 0);
	pthread_cond_init(&cond, 0);
	//Lendo
	int n,j,i,d,a,t;
	scanf("%d",&n);
	pessoa p[n];
	// Descreve elevador
	e.qtd_p = 0;
	e.andar = 0;
	l_cria(&(e.dest));
	l_cria(&(e.pedidos[0]));
	l_cria(&(e.pedidos[1]));
	e.status = 0;	// Parado
	//Lê pessoas
	pthread_t p_thr[n], e_thr;
	ini = clock();
	for (i=0;i<n;i++){
		scanf("%d",&d);
		p[i].id = i+100;
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
		printf("%d %d E 0\n",p[i].id,(int)((clock()-ini)*bi/CLOCKS_PER_SEC));
	}
	
	for (i=0;i < n; i++){
		pthread_join(p_thr[i],0);
	}
	printf("JOIN!\n");
	
	t = e.pedidos[0].qtd;
	printf("%d\n",t);
	for (i=0;i < t; i++){
		printf("%d ",l_retira(&(e.pedidos[0])));
	}printf("\n");
	pthread_mutex_destroy(&mutex_pedidos);
	pthread_cond_destroy(&cond);
	pthread_exit(0);
	return 0;
}