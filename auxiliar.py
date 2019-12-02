compartilhada = 0
leit_esp = 0
escr_esp = 0
fila_chegada = 0
fila_recurso = 0
recurso_ocupado = 0

def EscritoraEntrouNaFila(id):
	global fila_chegada
	fila_chegada = fila_chegada + 1
	
def EscritoraPedindoAcessoRecurso(id):
	global fila_chegada, fila_recurso
	fila_chegada = fila_chegada - 1
	fila_recurso = fila_recurso + 1
	print("Escritora pediu acesso e deve ser a proxima a executar")
	
def LeitoraEntrouNaFila(id):
	global fila_chegada
	fila_chegada = fila_chegada + 1
	
def LeitoraPedindoAcessoRecurso(id):
	global fila_chegada, fila_recurso
	fila_chegada = fila_chegada - 1
	fila_recurso = fila_recurso + 1
	print("Leitora pediu acesso e deve ser a proxima a executar")

def EscritoraEstaNaBarreira(id):
	global escr_esp
	escr_esp = escr_esp + 1

def LeitoraEstaNaBarreira(id):
	global leit_esp
	leit_esp = leit_esp + 1

def BroadcastLeitoras(l):
	global leit_esp
	if leit_esp != l:
		print("ERRO--- Broadcast. Thread leitora sinalizou no momento errado.")
		exit(1)
	else:
		leit_esp = 0

def BroadcastEscritoras(e):
	global escr_esp
	if escr_esp != e:
		print("ERRO--- Broadcast. Thread escritora sinalizou no momento errado.")
		exit(1)
	else:
		escr_esp = 0
	

def Escreve(x):
	global compartilhada, fila_recurso, recurso_ocupado
	if recurso_ocupado == 1:
		print("ERRO--- Thread escritora acessou o recurso quando nao devia.")
		exit(1)
	else:
		fila_recurso = fila_recurso - 1
		recurso_ocupado = 1
		compartilhada = x
		print("Escritora escreveu com sucesso")
	recurso_ocupado = 0

def Le(x):
	global compartilhada, fila_recurso, recurso_ocupado
	if recurso_ocupado == 1:
		print("ERRO--- Thread leitora acessou o recurso quando nao devia.")
		exit(1)
	else:
		fila_recurso = fila_recurso - 1
		recurso_ocupado = 1
	
	if compartilhada != x:
		print("ERRO--- Leitora leu o valor errado.")
		exit(1)
	else:
		print("Leitora leu com sucesso")
	recurso_ocupado = 0
	
