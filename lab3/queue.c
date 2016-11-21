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
  if(mem == NULL){
    printf("Malloc Error\n");
    exit(1);
  }
  
  return mem;
}


/* Cria um tipo no com valor arbitrario */
Node* newNode(char* val){

  Node* New = (Node*)smalloc(sizeof(Node));
  New->val = val;

  return New;
}


/* Libera (no a no) uma lista da memoria. */
void destroyList(Node** Root){

  Node* Next;
  Node* Iter = *Root;

  
  while(Iter != NULL){
    Next = Iter->next;
    free(Iter);
    Iter = Next;
  }
}


/* Cria umq fila */
Queue* newQueue(){

  Queue* Qe = smalloc(sizeof(Queue));
  Qe->start = NULL;
  Qe->end = NULL;
  return Qe;

}


/* Insere elemento na entrada da fila */
void inQueue(Queue** Qe, char* val){  
  
  Queue* Iter = *Qe;
  
  Node* New = newNode(val);  
  
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
char* outQueue(Queue** Qe){

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
int notNullQueue(Queue** Qe){
  return (*Qe)->end != NULL;
}


/* Libera fila da memoria. */
void destroyQueue(Queue** Qe){
  
  while(notNullQueue(Qe)){
    outQueue(Qe);
  }
  
  free(*Qe);
}

