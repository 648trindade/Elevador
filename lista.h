typedef struct elem{
	int valor;
	sstruct elem *prox;
}elem_t;

typedef struct{
	int qtd;
	elem_t *inicio;
}lista_t;

void l_cria(lista_t *l);

void l_insere(lista_t *l, int valor);

void l_insere_ord(lista_t *l, int valor);

int l_retira(lista_t *l);

int l_destroi(lista_t *l);