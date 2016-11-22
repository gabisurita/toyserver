/********************************************************************
Programa: queue.c
Autora: Gabriela Surita (RA:139095)
Data: 07/07/2015

Descricao: Define um tipo abstrado de dados fila utilizando listas
ligadas circulares.
********************************************************************/

/* Define um tipo no com valor inteiro. */
typedef struct node {
  char* val;
  struct node* next;
} Node;


/* Define um tipo fila */
typedef struct queue {
  Node* start;
  Node* end;
} Queue;


/* Malloc com verificacao */
void* smalloc(int Size);

/* Cria uma fila */
Queue* new_queue();

/* Insere elemento na entrada da fila */
void in_queue(Queue** Qe, char* val);

/* Retira elemento da saida da fila */
char* out_queue(Queue** Queue);

/* Verifica se a lista possui elementos */
int not_null_queue(Queue** Qe);

/* Libera fila da memoria. */
void destroy_queue(Queue** Qe);

/* Cria um tipo no com valor arbitrario */
Node* new_node(char* val);

