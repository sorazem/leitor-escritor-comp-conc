/* Tranalho da disciplina Computação Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Matheus Fernandes e Stephanie Orazem */
/* Codigo: Implementação do problema de leitores e escritores sem inanição */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int compartilhada = 0; // variavel onde vai ser escrita e lida pelas threads
sem_t mutex; // semáforo para mutex da variavel 'compartilhada'
sem_t queue_in, queue_out; // semáforos para fila de entrada e fila de saída no 'working room'
sem_t escr; // semáforo para escritoras
int l_onreading = 0; // quantidade de leitoras lendo 
int e_onwaiting = 0; // quantidade de escritoras esperando
FILE *arq_log; // arquivo de arq_log

void *le(void *tid) {
	int id = *(int*)tid;
	int i = 0;

	while(1) {
		i++;
		sem_wait(&queue_in);
		l_onreading++;
		sem_post(&queue_in);

		sem_wait(&mutex);
		if(l_onreading == 1) sem_wait(&queue_out);
		printf("Leitora %d lendo compartilhada = %d, i = %d\n", id, compartilhada, i);
		l_onreading--;
		if(e_onwaiting && !l_onreading) {
			sem_post(&queue_out);
		}
		sem_post(&mutex);
	}
		
	pthread_exit(NULL);
}

void *escreve(void* tid) {
	int id = *(int*) tid;
	int i = 0;

	while(1) {
		i++;
		sem_wait(&queue_in);
		sem_wait(&queue_out);
		
		sem_wait(&mutex);
		printf("Escritor %d vai escrever seu id, i = %d\n", id, i);
		compartilhada = id;
		sem_post(&mutex);

		sem_post(&queue_out);
		sem_post(&queue_in);
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
	
	sem_init(&mutex, 0, 1);
	sem_init(&queue_in, 0, 1);
	sem_init(&queue_out, 0, 1);
	sem_init(&escr, 0, 0);
	
	tid_sis_e = malloc(e*sizeof(pthread_t));
	if(!tid_sis_e) {
		printf("Erro de malloc no tid_sis_e\n");
		exit(-1);
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

	sem_destroy(&mutex);
	sem_destroy(&queue_in);
	sem_destroy(&queue_out);
	sem_destroy(&escr);
	
	fclose(arq_log);
	
	return 0;
}