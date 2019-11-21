/* Tranalho da disciplina Computação Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Matheus Fernandes e Stephanie Orazem */
/* Codigo: Implementação do problema de leitores e escritores sem inanição */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int e = 0, l = 0, compartilhada; // variáveis globais
sem_t mutex_e, escr, leit; // semáforos
FILE *arq_log; // arquivo de arq_log

void *le() {
	
}

void *escreve(void* tid) {
	int id = *(int*) tid;
	sem_wait(&mutex_e);
	printf("Escritor %d vai escrever\n", id);
	compartilhada = id;
	printf("%d\n", compartilhada);
	sem_post(&mutex_e);
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t *tid_sis_e;
	int e, l, num_escritas, num_leituras, *tid_e;
	int i;
	
	if(argc < 5) {
		fprintf(stderr, "Digite: %s <quantidade de leitores> <quantidade de escritores> <numero de leituras> <numero de escrituras> <arquivo de log>.\n", argv[0]);
		return 1;
   }

	arq_log = fopen(argv[5], "r");
	if(!arq_log) {
		fprintf(stderr, "Erro ao abrir o arquivo de log.\n");
		return 1;
	}
	
	l = atoi(argv[1]); // número de leitores
	e = atoi(argv[2]); // número de escritores
	num_leituras = atoi(argv[3]); // quantidade de leituras
	num_escritas = atoi(argv[4]); // quantidade de escritas
	
	sem_init(&mutex_e, 0, 1);
	
	tid_sis_e = malloc(e*sizeof(pthread_t));
	if(!tid_sis_e) {
		printf("Erro de malloc no tid_sis_e\n");
	}
	
	for(i = 0; i < e; i++) {
		tid_e = malloc(sizeof(int));
		if(!tid_e) {
			printf("Erro de malloc no tid_e\n");
			exit(-1);
		}
		*tid_e = i;
		
		if(pthread_create(&tid_sis_e[i], NULL, escreve, (void*)tid_e)){
			printf("Erro ao criar a thread %d.\n", i);
		}
	}
	
	for(i = 0; i < e; i++) {
		if(pthread_join(tid_sis_e[i], NULL)) {
			printf("Erro join\n");
		}
	}
	
	fclose(arq_log);
	
	return 0;
}