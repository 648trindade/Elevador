#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lista.h"
#include <time.h>
#include <sched.h>
#define _GNU_SOURCE

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
	int porta;
	pthread_mutex_t m_bt;
	pthread_mutex_t m_esp;
	pthread_cond_t c_bt;
	pthread_cond_t c_esp;
}elevador;

elevador e;
pessoa *p;

void debug(char s[]){
	pthread_mutex_lock(&mutex_print);
	printf("%s\n",s);
	pthread_mutex_unlock(&mutex_print);
}

void chama_elevador(int x){
	pthread_mutex_lock(&mutex_pedidos);
	int sentido;
	if (p[x].pedido > p[x].andar){
		sentido = 0; 	//pessoa vai subir
		print(p[x].id, 'S', p[x].andar);
	}
	else{
		sentido = 1; 	//pessoa vai descer
		print(p[x].id, 'D', p[x].andar);
	}
	l_insere_ord(&(e.pedidos[sentido]), p[x].andar, sentido);
	pthread_mutex_unlock(&mutex_pedidos);
	pthread_cond_signal(&(e.c_bt));
}

void *acao_pessoa(void *arg){
	//pessoa *p = (pessoa *) arg;
	char s[30];
	int x = (int)arg;
	print(p[x].id, 'E', 0);
	struct timespec wait, rest;
	wait.tv_sec = 0;
	while (1) {
		// consulta proximo destino
		p[x].pedido = l_retira(&(p[x].dest));
		// chama o elevador
		chama_elevador(x);
		//espera ser acordado pelo elevador (elevador chegar no andar)
		pthread_mutex_lock(&(p[x].m));
		p[x].status = 2;
		pthread_cond_wait(&(p[x].c),&(p[x].m));
		pthread_mutex_unlock(&(p[x].m));
		//entra elevador
		print(p[x].id, 'N', p[x].andar);
		p[x].status = 1;
		p[x].andar = p[x].pedido;
		print(p[x].id, 'I', p[x].andar);
		////debug("sinalizei");
		pthread_cond_signal(&(e.c_esp));
		//espera ser acordado pelo elevador (elevador chegar no destino)
		pthread_mutex_lock(&(p[x].m));
		pthread_cond_wait(&(p[x].c),&(p[x].m));
		pthread_mutex_unlock(&(p[x].m));
		//entra no andar e visita (espera)
		print(p[x].id, 'V', p[x].andar);
		pthread_cond_signal(&(e.c_esp));
		p[x].status = 0;
		wait.tv_nsec = (long) l_retira(&(p[x].time));
		if (wait.tv_nsec != -1){
			print(p[x].id, 'B', 0);
			nanosleep(&wait, &rest);
			print(p[x].id, 'E', 0);
		}
		else break;
	}
	print(p[x].id, 'M', 0);
	p[x].status = -1;
	pthread_cond_signal(&(e.c_bt));
	pthread_exit(NULL);
}

void define_direcoes(int direcao){
	debug(direcao?"procuro gente pra descer":"procuro gente pra subir");
	int i, j, stat, luz=0;
	lista_t aux;
	l_cria(&aux);
	if (direcao)	e.status = 2;
	else			e.status = 1;
	char s[30];
	while(e.pedidos[direcao].qtd){
		j = l_retira(&(e.pedidos[direcao]));
		if (j == e.andar){
			for(i=0; i<n && e.qtd_p<3; i++)
				if ((p[i].status == 2) && (p[i].andar == e.andar) && (direcao?((p[i].pedido-p[i].andar)<0):((p[i].pedido-p[i].andar)>0))){
					e.qtd_p++;
					e.pessoas[e.qtd_p-1] = i;
					l_insere_ord(&(e.dest),p[i].pedido,direcao);
					if (!(e.porta)){
						e.porta = 1;
						print(-1,'A',e.andar);
					}
					luz=1;
					pthread_cond_signal(&(p[i].c));
					//debug("esperando a pessoa sinalizar");
					pthread_mutex_lock(&(e.m_esp));
					pthread_cond_wait(&(e.c_esp),&(e.m_esp));
					pthread_mutex_unlock(&(e.m_esp));
					//debug("sinalizou");
				}
		}
		else if (direcao?(j < e.andar):(j > e.andar)){
			////debug("alguem chamou na mesma direcao");
			l_insere_ord(&(e.dest), j, direcao);
		}
		else{
			////debug("alguem chamou direcao oposta, ignorei");
			l_insere(&aux, j);
		}
	}
	while (aux.qtd)
		l_insere_ord(&(e.pedidos[direcao]), l_retira(&aux), direcao);
	//l_destroi(&aux);
	sprintf(s,"%d qtd",e.dest.qtd);
	////debug(s);
	for(i=0; i<n; i++)
		if ((p[i].status == 2) && (p[i].andar == e.andar) && (direcao?((p[i].pedido-p[i].andar)<0):((p[i].pedido-p[i].andar)>0)))
			////debug("vou ir sem desligar luz");
			return;
	if (luz)
		print(-1,direcao?'d':'s',e.andar);
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
	int i,desceu, quantia;
	char s[30];
	while(1){
		//coco
		if (e.qtd_p){
			desceu = 0;
			for(i=0;i<e.qtd_p;i++){
				if (p[e.pessoas[i]].pedido == e.andar){
					if (!(e.porta)){
						e.porta = 1;
						print(-1,'A',e.andar);
					}
					pthread_cond_signal(&(p[e.pessoas[i]].c));
					shifta_vetor(i);
					desceu = 1;
					pthread_mutex_lock(&(e.m_esp));
					pthread_cond_wait(&(e.c_esp),&(e.m_esp));
					pthread_mutex_unlock(&(e.m_esp));
				}
			}
			if (desceu)
				print(-1,'i',e.andar);
		}
		pthread_mutex_lock(&mutex_pedidos);
		while (!(e.pedidos[0].qtd) && !(e.pedidos[1].qtd) && !(e.qtd_p)){
			//debug("entrei no laco pq nao tem pedidos");
			pthread_mutex_unlock(&mutex_pedidos);
			for(i=0;i<n;i++)
				if(p[i].status >=0 )
					break;
			if (i==n){
				print(-1,'F',e.andar);
				pthread_exit(0);
			}
			for(i=0;i<n;i++)
				if((p[i].status ==2) && (p[i].andar == e.andar))
					break;
			if (i!=n){
				pthread_mutex_lock(&mutex_pedidos);
				break;
			}
			pthread_mutex_lock(&(e.m_bt));
			pthread_cond_wait(&(e.c_bt),&(e.m_bt));
			pthread_mutex_unlock(&(e.m_bt));
			pthread_mutex_lock(&mutex_pedidos);
		}
		pthread_mutex_unlock(&mutex_pedidos);
		pthread_mutex_lock(&mutex_pedidos);
		// teste
		quantia = e.pedidos[0].qtd;
		for (i=0; i<quantia; i++){
			desceu = l_retira(&(e.pedidos[0]));
			sprintf(s,"== subir %d",desceu);
			//debug(s);
			l_insere(&(e.pedidos[0]),desceu);
		}
		quantia = e.pedidos[1].qtd;
		for (i=0; i<quantia; i++){
			desceu = l_retira(&(e.pedidos[1]));
			sprintf(s,"== descer %d",desceu);
			//debug(s);
			l_insere(&(e.pedidos[1]),desceu);
		}
		if (e.pedidos[0].qtd && !((e.status == 2) && e.dest.qtd))
			define_direcoes(0);
		if (e.dest.qtd == 0)
			e.status = 0;
		if (e.pedidos[1].qtd && !((e.status == 1) && e.dest.qtd))
			define_direcoes(1);
		if (e.dest.qtd == 0)
			e.status = 0;
		sprintf(s,"status antes %d",e.status);
		//debug(s);
		// correcao
		if (!e.status){
			if (e.pedidos[0].qtd){
				quantia = e.pedidos[0].qtd;
				for (i=0; i < quantia-1; i++)
					l_insere(&(e.pedidos[0]), l_retira(&(e.pedidos[0])));
				quantia = l_retira(&(e.pedidos[0]));
				l_insere(&(e.dest), quantia);
				e.status = 1;
			}
			else if (e.pedidos[1].qtd){
				quantia = e.pedidos[1].qtd;
				for (i=0; i < quantia-1; i++)
					l_insere(&(e.pedidos[1]), l_retira(&(e.pedidos[1])));
				quantia = l_retira(&(e.pedidos[1]));
				l_insere(&(e.dest), quantia);
				e.status = 2;
			}
		}
		pthread_mutex_unlock(&mutex_pedidos);
		sprintf(s,"status depois %d",e.status);
		//debug(s);
		if (e.dest.qtd){
			sprintf(s,"porta %d",e.porta);
			//debug(s);
			if (e.porta){
				e.porta = 0;
				print(-1,'F',e.andar);
			}
			sprintf(s,"andar antes %d",e.andar);
			//debug(s);
			e.andar = l_retira(&(e.dest));
			sprintf(s,"andar depois %d",e.andar);
			//debug(s);
		}
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
		pthread_create(&p_thr[i], NULL, acao_pessoa, i);
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
	e.porta = 0;
	l_cria(&(e.dest));
	l_cria(&(e.pedidos[0]));
	l_cria(&(e.pedidos[1]));
	e.status = 0;	// Parado
	pthread_cond_init(&(e.c_bt),0);
	pthread_mutex_init(&(e.m_bt),0);
	pthread_cond_init(&(e.c_esp),0);
	pthread_mutex_init(&(e.m_esp),0);
	pthread_create(&e_thr,NULL,acao_elevador,NULL);
	cria_threads(p, p_thr, n);
	
	for (i=0;i < n; i++)
		pthread_join(p_thr[i],0);
	pthread_join(e_thr,0);
	pthread_cond_destroy(&(e.c_bt));
	pthread_mutex_destroy(&(e.m_bt));
	pthread_cond_destroy(&(e.c_esp));
	pthread_mutex_destroy(&(e.m_esp));
	for (i=0;i < n; i++) {
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