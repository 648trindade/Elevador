#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lista.h"
#include <time.h>

pthread_mutex_t mutex_pedidos, mutex_print;

clock_t ini;
int bi = 1e9, n;

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
	*  -1: 	Morta;
	*	0:	Visitando o andar;
	*	1:	Dentro do elevador;
	*   2:	Esperando o elevador;
*/
typedef struct{
	int id;				// Número da pessoa;
	int andar;			// Andar que ele se encontra;
	int pedido;
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
pessoa *p;

void chama_elevador(pessoa *p){
	pthread_mutex_lock(&mutex_pedidos);
	int sentido;
	if (p->pedido > p->andar){
		sentido = 0; 	//pessoa vai subir
		print(p->id, 'S', p->andar);
	}
	else{
		sentido = 1; 	//pessoa vai descer
		print(p->id, 'D', p->andar);
	}
	l_insere_ord(&(e.pedidos[sentido]), p->pedido, sentido);
	pthread_mutex_unlock(&mutex_pedidos);
	p->status = 2;
}

void *acao_pessoa(void *arg){
	pessoa *p = (pessoa *) arg;
	print(p->id, 'E', 0);
	struct timespec wait, rest;
	wait.tv_sec = 0;
	while (1) {
		// consulta proximo destino
		p->pedido = l_retira(&(p->dest));
		// chama o elevador
		chama_elevador(p);
		//espera ser acordado pelo elevador (elevador chegar no andar)
		pthread_mutex_lock(&(p->m));
		pthread_cond_wait(&(p->c),&(p->m));
		pthread_mutex_unlock(&(p->m));
		//entra elevador
		//TODO
		print(p->id, 'N', p->andar);
		//p->status = 1;
		p->andar = p->pedido;
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
	print(p->id, 'M', 0);
	p->status = -1;
	pthread_exit(NULL);
}

void define_direcoes(int direcao){
	int i,j, status;
	lista_t aux;
	l_cria(&aux);
	if (direcao)	status = 2;
	else			status = 1;
	e.status = status;
	while(e.pedidos[direcao].qtd){
		j = l_retira(&(e.pedidos[direcao]));
		if (j == e.andar){
			for(i=0; i<n || e.qtd_p<3; i++)
				if (p[i].status == 2 && p[i].andar == e.andar && direcao?((p->pedido-p->andar)>0):((p->pedido-p->andar)<0)){
					pthread_cond_signal(&(p[i].c));
					e.qtd_p++;
					p[i].status = 1;
					e.pessoas[e.qtd_p-1] = i;
					//TODO verificar se ainda resta pessoas esperando pra subir nesse andar
					//se nao tiver, apagar o botao externo
				}
		}
		else if (direcao?(j > e.andar):(j < e.andar))
			l_insere_ord(&(e.dest), j, direcao);
		else
			l_insere(&aux, j);
	}
	while (aux.qtd)
		l_insere_ord(&(e.pedidos[0]), l_retira(&aux), direcao);
	l_destroi(&aux);
	for(i=0; i<n; i++)
		if (p[i].status == 2 && p[i].andar == e.andar)
			return;
	print(0,direcao?'d':'s',e.andar);
}

void shifta_vetor(int pos){
	if (pos==0){
		e.pessoas[0] = e.pessoas[1];
		e.pessoas[1] = e.pessoas[2];
	}
	else if (pos==1)
		e.pessoas[1] = e.pessoas[2];
	e.qtd_p--;
}

void *acao_elevador(void* args){
	// Espera enquanto nao houver destinos nem pedidos
	int i,j,fim;
	while(1){
		print(0,'A',e.andar);
		if (e.qtd_p){
			for(i=0;i<e.qtd_p;i++){
				if (p[e.pessoas[i]].pedido == e.andar){
					pthread_cond_signal(&(p[e.pessoas[i]].c));
					shifta_vetor(i);
				}
				print(0,'i',e.andar);
			}
		}
		if (e.dest.qtd == 0){
			pthread_mutex_lock(&mutex_pedidos);
			while (e.pedidos[0].qtd == 0 || e.pedidos[1].qtd==0){
				pthread_mutex_unlock(&mutex_pedidos);
				// testa se todas ja morreram
				for(i=0;i<n;i++)
					if(p[i].status >=0 )
						break;
				if (i==n){
					print(0,'F',e.andar);
					pthread_exit(0);
				}
				pthread_yield();
				pthread_mutex_lock(&mutex_pedidos);
			}
			pthread_mutex_unlock(&mutex_pedidos);
		}
		pthread_mutex_lock(&mutex_pedidos);
		if (e.status !=2 && e.pedidos[0].qtd)
			define_direcoes(0);
		else if (e.status !=1 && e.pedidos[1].qtd)
			define_direcoes(1);
		pthread_mutex_unlock(&mutex_pedidos);
		print(0,'F',e.andar);
		e.andar = l_retira(&(e.dest));
	}
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
	int j,i;
	scanf("%d",&n);
	p = malloc(n*sizeof(pessoa));
	//Lê pessoas
	pthread_t p_thr[n], e_thr;
	ini = clock();
	
	// Descreve elevador
	e.qtd_p = 0;
	e.andar = 0;
	l_cria(&(e.dest));
	l_cria(&(e.pedidos[0]));
	l_cria(&(e.pedidos[1]));
	e.status = 0;	// Parado
	pthread_create(&e_thr,NULL,acao_elevador,NULL);
	cria_threads(p, p_thr, n);
	
	for (i=0;i < n; i++){
		pthread_join(p_thr[i],0);
		pthread_cond_destroy(&(p[i].c));
		pthread_mutex_destroy(&(p[i].m));
	}
	free(p);
	printf("JOIN!\n");
	
	j= e.pedidos[0].qtd;
	for (i=0;i < j; i++){
		printf("%d ",l_retira(&(e.pedidos[0])));
	}printf("\n");
	pthread_mutex_destroy(&mutex_pedidos);
	pthread_exit(0);
	return 0;
}