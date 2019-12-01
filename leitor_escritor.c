/* Tranalho da disciplina Computação Concorrente */
/* Prof.: Silvana Rossetto */
/* Alunos: Matheus Fernandes e Stephanie Orazem */
/* Codigo: Implementação do problema de leitores e escritores sem inanição */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int e, l;	// quantidade de escritoras e leitoras
int num_escritas, num_leituras; // quantidade de numero de escritas e leituras para cada thread
								// de seu respectivo tipo

int compartilhada = 0;	// variavel onde vai ser escrita e lida pelas threads
pthread_mutex_t mutex_leit, fila_chegada, fila_recurso; // mutex para a variavel leitoras,
														// mutex para a fila de chegada de threads,
														// mutex para a fila de acesso ao Working Room
int leitoras = 0;	// conta a quantidade de leitoras que estão lendo

pthread_cond_t bar_leit, bar_escr;	// variavel de condição para as barreiras de leitora e escritora
pthread_mutex_t mutex_bar_leit, mutex_bar_escr; // mutex para as barreiras de leitora e escritora
int leit_esp = 0, escr_esp = 0; // conta a quantidade de leitoras e escritoras que estão esperando
								// em suas respectivas barreiras

FILE *arq_log; // arquivo de log da execução

/** 
 * Barreira das threads leitoras. Uma thread leitora só pode fazer leitura da variável compartilhada 
 * após todas as outras threads leitoras também lerem. Se não for a última thread a passar pela barreira,
 * então fica bloqueada. Se sim, então desbloqueia todas as outras threads.
 * 
 * @param int	numero identificador da thread que está passando pela barreira
 */
void barreiraLeitora(int id) {
	pthread_mutex_lock(&mutex_bar_leit);
	fprintf(arq_log, "LeitoraEstaNaBarreira(%d)\n", id);
	leit_esp++;
	if(leit_esp < l) {
		pthread_cond_wait(&bar_leit, &mutex_bar_leit);
	} else {
		leit_esp = 0;
		pthread_cond_broadcast(&bar_leit);
		fprintf(arq_log, "BroadcastLeitoras(%d)\n", l);
	}
	pthread_mutex_unlock(&mutex_bar_leit);
}

/**
 * Função para a thread leitora garantir acesso ao Working Room para si e outras threads leitoras. Uma
 * thread leitora só pode acessar se não tiver nenhuma thread escritora trabalhando. Se não tiver 
 * nenhuma thread no Working Room, então a leitora garante acesso para si e outras leitoras. Se tiver
 * uma escritora e ninguém pediu acesso, então pede e espera pelo término da escritora, caso contrário
 * espera na fila de chegada. Se tiver leitoras e nenhuma escritora pediu acesso, então acessa, caso
 * contrário espera na fila de chegada.
 * 
 * @param int	numero identificador da thread que está passando
 */
void entraLeitora(int id) {
	fprintf(arq_log, "LeitoraEntrouNaFila(%d)\n", id);
	pthread_mutex_lock(&fila_chegada);
	pthread_mutex_lock(&mutex_leit);
	if (leitoras == 0){
		fprintf(arq_log, "LeitoraPedindoAcessoRecurso(%d)\n", id);
		pthread_mutex_lock(&fila_recurso);
	}     
	leitoras++;
	pthread_mutex_unlock(&fila_chegada);
	pthread_mutex_unlock(&mutex_leit);
}

/**
 * Função da saída do Working Room da thread leitora. Se for a última leitora trabalhando, então
 * libera o acesso do Working Room para a escritora.
 * 
 * @param int	numero identificador da thread que está passando
 */
void saiLeitora(int id) {
	pthread_mutex_lock(&mutex_leit);
	leitoras--;
	if (leitoras == 0)
		pthread_mutex_unlock(&fila_recurso);
	//fprintf(arq_log, "SaiLeitora(%d)\n", id);
	pthread_mutex_unlock(&mutex_leit);
}

/**
 * Função principal da thread leitora. Ela lê o conteúdo da variável compartilhada e escreve em seu
 * respectivo arquivo de log. Ela escreve uma quantidade de [num_leituras] vezes, definida na entrada
 * do programa.
 * 
 * @param int	numero identificador da thread leitora
 */
void *Le(void *tid) {
	int i = 0;
	int id = *(int *) tid;
	
	FILE *arq_leitor; // arquivo para o leitor
	char nome[] = "X.txt"; // padrão de nome
	nome[0] = id + '0'; // muda o nome de acordo com o id da thread
	
	arq_leitor = fopen(nome, "a"); // Abriu arquivo para gravação no final do arquivo.
	if(!arq_leitor){
		printf("Erro ao abrir o arquivo do leitor.\n");
		exit(-1);
	}

	while(i < num_leituras) {
		i++;

		entraLeitora(id);

        // Working Room
		fprintf(arq_log, "Le(%d)\n", compartilhada);
		putc(compartilhada + '0', arq_leitor);

		saiLeitora(id);

		barreiraLeitora(id);
	}
	
	fclose(arq_leitor);
		
	pthread_exit(NULL);
}

/** 
 * Barreira das threads escritoras. Uma thread escritora só pode escrever na variável compartilhada 
 * após todas as outras threads escritoras também escreverem. Se não for a última thread a passar 
 * pela barreira, então fica bloqueada. Se sim, então desbloqueia todas as outras threads.
 * 
 * @param int	numero identificador da thread que está passando pela barreira
 */
void barreiraEscritora(int id) {
	pthread_mutex_lock(&mutex_bar_escr);
	fprintf(arq_log, "EscritoraEstaNaBarreira(%d)\n", id);
	escr_esp++;
	if(escr_esp < e) {
		pthread_cond_wait(&bar_escr, &mutex_bar_escr);
	} else {
		escr_esp = 0;
		pthread_cond_broadcast(&bar_escr);
		fprintf(arq_log, "BroadcastEscritoras(%d)\n", e);
	}
	pthread_mutex_unlock(&mutex_bar_escr);
}

/**
 * Função para a thread escritora garantir acesso ao Working Room para si. Uma thread escritora só 
 * pode acessar se e somente se não tiver nenhuma outra thread trabalhando. Se não tiver nenhuma 
 * thread no Working Room, então a escritora garante acesso para si. Se tiver uma escritora ou
 * leitoras e ninguém pediu acesso, então pede e espera pelo término da thread que estiver
 * trabalhando, caso contrário espera na fila de chegada.
 * 
 * @param int	numero identificador da thread que está passando
 */
void entraEscritora(int id) {
	fprintf(arq_log, "EscritoraEntrouNaFila(%d)\n", id);
	pthread_mutex_lock(&fila_chegada);
	fprintf(arq_log, "EscritoraPedindoAcessoRecurso(%d)\n", id);
	pthread_mutex_lock(&fila_recurso);
	pthread_mutex_unlock(&fila_chegada);
}

/**
 * Função da saída do Working Room da thread escritora. Libera o acesso do Working Room para qualquer
 * thread.
 * 
 * @param int	numero identificador da thread que está passando
 */
void saiEscritora(int id) {
	//fprintf(arq_log, "SaiEscritora(%d)\n", id);
	pthread_mutex_unlock(&fila_recurso);
}

/**
 * Função principal da thread escritora. Ela escreve na variável compartilhada o seu número
 * identificador. Ela escreve uma quantidade de [num_escritas] vezes, definida na entrada do programa.
 * 
 * @param int	numero identificador da thread leitora
 */
void *Escreve(void* tid) {
	int id = *(int*) tid;
	int i = 0;

	while(i < num_escritas) {
        i++;

		entraEscritora(id);
        
        // Working Room
		fprintf(arq_log, "Escreve(%d)\n", id);
		compartilhada = id;

		saiEscritora(id);

		barreiraEscritora(id);
	}
	
	pthread_exit(NULL);
}


/**
 * Para execução do leitor-escritor com ausência de inanição entre as threads, utilizamos um mutex 
 * para uma 'fila de chegada' que indica as threads que tem interesse em acessar ao recurso e um outro
 * mutex para garantir o acesso exclusivo ao working room, onde ocorre a execução dos trabalhos das 
 * threads, preservando os requisitos do problema. O fluxo da execução está descrito nas documentações
 * de cada função. 
 */
int main(int argc, char *argv[]) {
	pthread_t *tid_sis_e, *tid_sis_l; // threads escritoras e leitoras, respectivamente
	int *tid; // numero identificador de cada thread
	int i; // variavel iteradora

	char ch; // variavel para copiar cada char do auxiliar.py no arquivo de log
	FILE *source; // arquivo auxiliar.py
	
	if(argc < 5) {
		fprintf(stderr, "Digite: %s <quantidade de leitores> <quantidade de escritores> <numero de leituras> <numero de escrituras> <arquivo de log>.\n", argv[0]);
		return 1;
   }

   	source = fopen("auxiliar.py", "r");
	if(!source) {
		fprintf(stderr, "Erro ao abrir o auxiliar.py.\n");
		return 1;
	}

	arq_log = fopen(argv[5], "w+");
	if(!arq_log) {
		fprintf(stderr, "Erro ao abrir o arquivo de log.\n");
		return 1;
	}

	while( ( ch = fgetc(source) ) != EOF )
		fputc(ch, arq_log);

	fclose(source);
	
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