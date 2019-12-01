compartilhada = 0; // variavel onde vai ser escrita e lida pelas threads
leit_esp = 0, escr_esp = 0;
fila_chegada = 0, fila_recurso = 0
recurso_ocupado = 0

def EscritoraEntrouNaFila(id):
	global fila_chegada
	fila_chegada++
	
def EscritoraPedindoAcessoRecurso(id):
	global fila_chegada, fila_recurso
	fila_chegada--
	fila_recurso++
	
def LeitoraEntrouNaFila(id):
	global fila_chegada
	fila_chegada++
	
def LeitoraPedindoAcessoRecurso(id)
	global fila_chegada, fila_recurso
	fila_chegada--
	fila_recurso++

def EscritoraEstaNaBarreira(id):
	global escr_esp
	escr_esp++

def EscritoraSaiuDaBarreira(id):
	global escr_esp
	escr_esp--

def LeitoraEstaNaBarreira(id):
	global lei_esp
	leit_esp++

def LeitoraSaiuDaBarreira(id):
	global lei_esp
	leit_esp--

def BroadcastLeitoras(l):
	global leit_esp
	if leit_esp != l:
		print("ERRO--- Broadcast. Thread sinalizou no momento errado.")
	else:
		leit_esp = 0

def BroadcastEscritoras(e):
	global escr_esp
	if escr_esp != e:
		print("ERRO--- Broadcast. Thread sinalizou no momento errado.")
	else:
		escr_esp = 0
	

def Escreve(x):
	global compartilhada, fila_recurso, recurso_ocupado
	if recurso_ocupado == 1:
		print("ERRO--- Thread acessou o recurso quando não devia.")
	else:
		fila_recurso--
		recurso_ocupado = 1
		compartilhada = x
	recurso_ocupado = 0

def Le(x):
	global compartilhada, fila_recurso, recurso_ocupado
	if recurso_ocupado == 1:
		print("ERRO--- Thread acessou o recurso quando não devia.")
	else:
		fila_recurso--
		recurso_ocupado = 1
	
	if compartilhada != x:
		print("ERRO--- Leitora leu o valor errado.")
	recurso_ocupado = 0