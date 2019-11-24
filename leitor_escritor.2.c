/* Tranalho da disciplina Computação Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Matheus Fernandes e Stephanie Orazem */
/* Codigo: Implementação do problema de leitores e escritores sem inanição */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int e_onwrite = 0, l_onread = 0; // quantidade de escritores e leitores trabalhando
int e_onwaiting = 0, l_onwaiting = 0; // quantidade de escritores e leitores trabalhando
int queue = 0; // fila de espera. se 0 tem ninguém na fila, se 1 é um leitor e se 2 é um escritor
int compartilhada = 30; // variavel onde vai ser escrita e lida pelas threads
FILE *arq_log; // arquivo de arq_log

pthread_cond_t cond_leit, cond_escr;
pthread_mutex_t mutex, mutex_comp;  // mutex para leitura de 'e' e 'l' e para 
                                    // variavel compartilhada respectivamente

void EntraLeitura() {
    pthread_mutex_lock(&mutex);
    while(e_onwrite > 0 || e_onwaiting > 0) {
		l_onwaiting++;
        pthread_cond_wait(&cond_leit, &mutex);
		l_onwaiting--;
    }
    l_onread++;
    pthread_mutex_unlock(&mutex);
}

void SaiLeitura() {
    pthread_mutex_lock(&mutex);
    l_onread--;
    if(l_onread == 0) pthread_cond_signal(&cond_escr);
    pthread_mutex_unlock(&mutex);
}

void EntraEscrita() {
    pthread_mutex_lock(&mutex);
    while((l_onread > 0) || (e_onwrite > 0)) {
		e_onwaiting++;
		if(!queue) {
			queue = 2;
		}
        pthread_cond_wait(&cond_escr, &mutex);
		if(queue) {
			queue = 0;
		}
		e_onwaiting++;
    }
    e_onwrite++;
    pthread_mutex_unlock(&mutex);
}

void SaiEscrita() {
    pthread_mutex_lock(&mutex);
    e_onwrite--;
    pthread_cond_signal(&cond_escr);
    pthread_cond_broadcast(&cond_leit);
    pthread_mutex_unlock(&mutex);
}

void *le(void *tid) {
	int id = *(int*)tid;

	while(1) {
		EntraLeitura();

        pthread_mutex_lock(&mutex_comp);
		printf("Leitora %d lendo compartilhada = %d\n", id, compartilhada);
        pthread_mutex_unlock(&mutex_comp);

        SaiLeitura();
	}
		
	pthread_exit(NULL);
}

void *escreve(void* tid) {
	int id = *(int*) tid;

	while(1) {
        EntraEscrita();
		
        pthread_mutex_lock(&mutex_comp);
		printf("Escritor %d vai escrever seu id\n", id);
		compartilhada = id;
        pthread_mutex_unlock(&mutex_comp);

		SaiEscrita();
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
	
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_comp, NULL);
    pthread_cond_init(&cond_escr, NULL);
    pthread_cond_init(&cond_leit, NULL);
	
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

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_escr);
    pthread_cond_destroy(&cond_leit);
	
	fclose(arq_log);
	
	return 0;
}