/********************************************************************
Programa: queue.c
Autora: Gabriela Surita (RA:139095)
Data: 07/07/2015

Descricao: Implementa um tipo abstrado de dados fila utilizando
listas ligadas circulares.

********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


/* Malloc com verificacao */
void* smalloc(int size){

  void* mem = malloc(size);
  if(mem == NULL)
    exit(1);

  return mem;
}


/* Cria um tipo no com valor arbitrario */
Node* new_node(char* val){

  Node* New = (Node*)smalloc(sizeof(Node));
  New->val = val;

  return New;
}


/* Cria umq fila */
Queue* new_queue(){

  Queue* Qe = smalloc(sizeof(Queue));
  Qe->start = NULL;
  Qe->end = NULL;
  return Qe;

}


/* Insere elemento na entrada da fila */
void in_queue(Queue** Qe, char* val){

  Queue* Iter = *Qe;

  Node* New = new_node(val);

  if(Iter->end){
    New->next = Iter->start;
    Iter->end->next = New;
    Iter->end = New;
  }
  else{
    Iter->start = New;
    Iter->end = New;
    Iter->end->next = New;
  }
}


/* Retira elemento da saida da fila */
char* out_queue(Queue** Qe){

  Node* OutNode;
  char* OutValue;
  Queue* Iter = *Qe;

  OutNode = Iter->start;

  if(Iter->end != Iter->start){
    Iter->start = Iter->start->next;
  }
  else{
    Iter->start = NULL;
    Iter->end = NULL;
  }

  OutValue = OutNode->val;

  free(OutNode);

  return OutValue;
}


/* Verifica se a lista possui elementos */
int not_null_queue(Queue** Qe){
  return (*Qe)->end != NULL;
}


/* Libera fila da memoria. */
void destroy_queue(Queue** Qe){

  while(not_null_queue(Qe)){
    out_queue(Qe);
  }

  free(*Qe);
}

