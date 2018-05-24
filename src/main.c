/* Contador de palavras
 *
 * Este programa recebera uma serie de caracteres representando palavras em sua
 * entrada. Ao receber um caractere fim de linha ('\n'), deve imprimir na tela o
 * numero de palavras separadas que recebeu e, apos, encerrar.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10000000
/*NUmero maximo de threads permitidas (excluindo a principal*/
#define MAX_THREADS 0

typedef struct{
  int ini;
  int fim;
} argumento;

/*Numeros a serem ordenados*/
int numero[MAX];

/*Tamanho da entrada*/
int tam = 0;

/*Numero de threads ativas (nao contando a inicial)*/
int n_threads_disponiveis = MAX_THREADS;

/*Trava para regiao critica*/
pthread_mutex_t trava;

/*Vetor de threads*/
pthread_t threads[MAX_THREADS];

/*Pilha com indices de threads disponiveis no vetor de threads*/
short threads_disponiveis[MAX_THREADS];

/*TRoca valor dasd variaveis apontadas*/
void troca(int *a, int *b){
  int aux = *a;
  *a = *b;
  *b = aux;
}

/*Particiona vetor*/
int particiona(int ini, int fim){
  /*Randomiza vetor*/
  int j, i = rand()%(fim - ini + 1) + ini;
  troca(numero + i, numero + fim);
  
  int val = numero[fim];
  i = ini - 1;
  for(j = ini; j < fim; j++)
      if(numero[j] <= val){
	 i++;
	 troca(numero + i, numero + j);
      }
  troca(numero + i + 1 , numero + fim);
  
  return i + 1;
}

/*Realiza quicksort no vetor, entre as posições ini e fim, gerando threads se possivel para dividir recursão*/
void quicksort(argumento *args){
  argumento *new_args1, *new_args2;
  new_args1 = (argumento*)malloc(sizeof(argumento));
  new_args2 = (argumento*)malloc(sizeof(argumento));
  
  int ind_thread;
  
  /*Decide se divide em duas threads ou não*/
  int divide = 0;
  
  if(args->ini < args->fim){
    int pivo = particiona(args->ini, args->fim);
    
    /*Decide se divide em duas threads ou não*/
    pthread_mutex_lock(&trava);
    if(n_threads_disponiveis > 0){
      /*desempilha valor da pilha de threads disponiveis*/
      ind_thread = threads_disponiveis[n_threads_disponiveis - 1];
      n_threads_disponiveis--;
      divide = 1;
    }

    pthread_mutex_unlock(&trava);
    
    /*Se for possível dividir sem exeder numero de thrads, a thread atual executa uma instancia da recursao e cria uma cria outra thread para a outra instância*/
    if(divide){
      new_args1->ini = args->ini;
      new_args1->fim = pivo - 1; 
      pthread_create(&(threads[ind_thread]), NULL, quicksort, new_args1);
      new_args2->ini = pivo + 1;
      new_args2->fim = args->fim;
      quicksort(new_args2);
    }
    
    /*Caso contrario, thread atual executa as duas instancias*/
    else{
      new_args1->ini = args->ini;
      new_args1->fim = pivo - 1;
      quicksort(new_args1);
      new_args2->ini = pivo + 1;
      new_args2->fim = args->fim;
      quicksort(new_args2);
    }
  }
  
  
  
  if(divide){
    /*Espera filho terminar*/
    pthread_join(threads[ind_thread], NULL);
    /*Retorna indice para a pilha*/
    pthread_mutex_lock(&trava);
    threads_disponiveis[n_threads_disponiveis] = ind_thread;
    n_threads_disponiveis++;
    pthread_mutex_unlock(&trava);
  }
  
  free(new_args1);
  free(new_args2);
}

int main() {
  srand(0);
  
  int i, aux;
  
  /*Empilha indices disponiveis no vetor de threads*/
  for(i = 0; i < MAX_THREADS; i++)
      threads_disponiveis[i] = i;
  
  /*Le entrada*/
  while(scanf("%d", &aux) == 1)
    numero[tam++] = aux;
  
  /*Cria argumento para passar para o quicksort*/  
  argumento *args = malloc(sizeof(argumento));
  args->ini = 0;
  args->fim = tam - 1;
  
  quicksort(args);
  
  free(args);
  
  for(i = 0; i < tam; i++)
      printf("%d%c", numero[i], " \n"[i == tam - 1]);
  
  return 0;
}
