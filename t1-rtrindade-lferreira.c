#include <stdio.h>
#include <stdlib.h>
#include <pthreads.h>

/*	=== Pessoa ===
	(Objeto privado) Estrutura representando uma pessoa.
	Contém uma variável (status) indicando o estado da pessoa: 
	*	0:	Visitando o andar;
	*	1:	Dentro do elevador;
	*	-1:	Esperando o elevador;
*/
typedef struct{
	int andar;			// Andar que ele se encontra;
	int status;			// Estado atual da pessoa;
	int *dest;			// Vetor de destinos da pessoa;
	int *time;			// Vetor de tempo de visita em cada andar destino;
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
	*	-1:	descendo
*/
typedef struct{
	int nmax;			// Número máximo de pessoas;
	int n;				// Número de pessoas dentro do elevador;
	int pos;			// Andar que ele se encontra;
	int *dest;			// Vetor contendo as direções (possivelmente tenha que ser global)
	int status;			// Estado do elevador;
}elevador;

int main(){
	return 0;
}