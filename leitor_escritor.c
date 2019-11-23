/* Tranalho da disciplina Computação Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Matheus Fernandes e Stephanie Orazem */
/* Codigo: Implementação do problema de leitores e escritores sem inanição */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int e = 0, l = 0; // quantidade de escritores e leitores trabalhando
int compartilhada; // variavel onde vai ser escrita e lida pelas threads
sem_t mutex_e, mutex_l, escr, leit; // semáforos
FILE *arq_log; // arquivo de arq_log

void *le(void *tid) {
	int id = *(int*)tid;

	while(1) {
		sem_wait(&leit);
		sem_wait(&mutex_l);
		l++;
		if(l==1) sem_wait(&escr);
		sem_post(&mutex_l);
		sem_post(&leit);

		sem_wait(&mutex_l);
		printf("compartilhada = %d\n", compartilhada);
		sem_post(&mutex_l);

		sem_wait(&mutex_l);
		l--;
		if(l==0) sem_post(&escr);
		sem_post(&mutex_l);
	}
		
	pthread_exit(NULL);
}

void *escreve(void* tid) {
	int id = *(int*) tid;

	while(1) {
		sem_wait(&mutex_e);
		e++;
		if(e==1) sem_wait(&leit);
		sem_post(&mutex_e);
		sem_wait(&escr);
		
		printf("Escritor %d vai escrever\n", id);
		compartilhada = id;
		printf("%d\n", compartilhada);

		sem_post(&escr);
		sem_wait(&mutex_e); 
		e--;
		if(e==0) sem_post(&leit);
		sem_post(&mutex_e);
	}
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t *tid_sis_e, *tid_sis_l;
	int e, l, num_escritas, num_leituras, *tid;
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
	sem_init(&mutex_l, 0, 1);
	
	tid_sis_e = malloc(e*sizeof(pthread_t));
	if(!tid_sis_e) {
		printf("Erro de malloc no tid_sis_e\n");
		exit(-1);
	}
	
	for(i = 0; i < e; i++) {
		tid = malloc(sizeof(int));
		if(!tid) {
			printf("Erro de malloc no tid\n");
			exit(-1);
		}
		*tid = i;
		
		if(pthread_create(&tid_sis_e[i], NULL, escreve, (void*)tid)){
			printf("Erro ao criar a thread %d.\n", i);
			exit(-1);
		}
	}

	tid_sis_l = malloc(l*sizeof(pthread_t));
	if(!tid_sis_l) {
		printf("Erro de malloc no tid_sis_l\n");
		exit(-1);
	}

	for(i = 0; i < l; i++) {
		tid = malloc(sizeof(int));
		if(!tid) {
			printf("Erro de malloc na tid\n");
			exit(-1);
		}
		*tid = i;

		if(pthread_create(&tid_sis_l[i], NULL, le, (void*)tid)) {
			printf("Erro ao criar a thread %d.\n", l);
			exit(-1);
		}
	}
	
	for(i = 0; i < e; i++) {
		if(pthread_join(tid_sis_e[i], NULL)) {
			printf("Erro join de tid_sis_e\n");
			exit(-1);
		}
	}

	for(i = 0; i < l; i++) {
		if(pthread_join(tid_sis_l[i], NULL)) {
			printf("Erro join de tid_sis_l\n");
			exit(-1);
		}
	}

	sem_destroy(&mutex_e);
	sem_destroy(&mutex_l);
	
	fclose(arq_log);
	
	return 0;
}