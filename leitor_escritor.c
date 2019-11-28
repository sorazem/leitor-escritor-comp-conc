/* Tranalho da disciplina Computação Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Matheus Fernandes e Stephanie Orazem */
/* Codigo: Implementação do problema de leitores e escritores sem inanição */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int e, l;

int compartilhada = 0; // variavel onde vai ser escrita e lida pelas threads
pthread_mutex_t mutex_leit, fila_chegada, fila_recurso;
int leitoras = 0;

pthread_cond_t bar_leit, bar_escr;
pthread_mutex_t mutex_bar_leit, mutex_bar_escr;
int leit_esp = 0, escr_esp = 0;

FILE *arq_log; // arquivo de arq_log

void barreiraLeitora() {
	pthread_mutex_lock(&mutex_bar_leit);
	leit_esp++;
	if(leit_esp < l) {
		pthread_cond_wait(&bar_leit, &mutex_bar_leit);
	} else {
		leit_esp = 0;
		pthread_cond_broadcast(&bar_leit);
	}
	pthread_mutex_unlock(&mutex_bar_leit);
}

void entraLeitora(int id) {
	pthread_mutex_lock(&fila_chegada);
	pthread_mutex_lock(&mutex_leit);
	if (leitoras == 0)
		pthread_mutex_lock(&fila_recurso);        
	leitoras++;
	pthread_mutex_unlock(&fila_chegada);
	printf("EntraLeitora(%d)\n", id);
	pthread_mutex_unlock(&mutex_leit);
}

void saiLeitora(int id) {
	pthread_mutex_lock(&mutex_leit);
	leitoras--;
	if (leitoras == 0)
		pthread_mutex_unlock(&fila_recurso);
	printf("SaiLeitora(%d)\n", id);
	pthread_mutex_unlock(&mutex_leit);
}

void *Le(void *tid) {
	int id = *(int*)tid;
	int i = 0;

	while(1) {
		i++;

		entraLeitora(id);

        //printf("Leitora %d lendo compartilhada = %d, i = %d\n", id, compartilhada, i);
		printf("Le(%d)\n", compartilhada);

		saiLeitora(id);

		barreiraLeitora();
	}
		
	pthread_exit(NULL);
}

void barreiraEscritora() {
	pthread_mutex_lock(&mutex_bar_escr);
	escr_esp++;
	if(escr_esp < e) {
		pthread_cond_wait(&bar_escr, &mutex_bar_escr);
	} else {
		escr_esp = 0;
		pthread_cond_broadcast(&bar_escr);
	}
	pthread_mutex_unlock(&mutex_bar_escr);
}

void entraEscritora(int id) {
	pthread_mutex_lock(&fila_chegada);
	pthread_mutex_lock(&fila_recurso);
	pthread_mutex_unlock(&fila_chegada);
	printf("EntraEscritora(%d)\n", id);
}

void saiEscritora(int id) {
	printf("SaiEscritora(%d)\n", id);
	pthread_mutex_unlock(&fila_recurso);
}

void *Escreve(void* tid) {
	int id = *(int*) tid;
	int i = 0;

	while(1) {
        i++;

		entraEscritora(id);
        
        //printf("Escritor %d vai escrever seu id, i = %d\n", id, i);
		printf("Escreve(%d)\n", id);
		compartilhada = id;

		saiEscritora(id);

		barreiraEscritora();
	}
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t *tid_sis_e, *tid_sis_l;
	int num_escritas, num_leituras, *tid;
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
	
	pthread_mutex_init(&mutex_leit, NULL);
	pthread_mutex_init(&fila_chegada, NULL);
	pthread_mutex_init(&fila_recurso, NULL);

	pthread_cond_init(&bar_leit, NULL);
	pthread_cond_init(&bar_escr, NULL);
	pthread_mutex_init(&mutex_bar_leit, NULL);
	pthread_mutex_init(&mutex_bar_escr, NULL);

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

		if(pthread_create(&tid_sis_l[i], NULL, Le, (void*)tid)) {
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
		
		if(pthread_create(&tid_sis_e[i], NULL, Escreve, (void*)tid)){
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

	pthread_mutex_destroy(&mutex_leit);
	pthread_mutex_destroy(&fila_chegada);
	pthread_mutex_destroy(&fila_recurso);
	
	fclose(arq_log);
	
	return 0;
}