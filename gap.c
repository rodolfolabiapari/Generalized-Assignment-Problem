
/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Funções Genéricas do Problam GAP.
 * Data: 01/08/2016.
 * Distribuição Livre, desde que referenciando o autor.
 * 
 * Professor: Haroldo Santos
 * 
 * Autor do Trabalho: Rodolfo Labiapari Mansur Guimarães
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "gap.h"


/*
 * Procedimento que avalia a solução atual.
 * 
 * Se a solução é factível, retorna seu custo real.
 * Se infactível retorna o custo encontrado * 1000000
 */
double Avalia_Solucao(Solucao * sol) {
	int i = 0, capacidade_agentes[QUANT_AGENTES], sum_recursos = 0;
	double custo = 0; char solucao_invalida = 0;
	
	// Define a capacidade inicial utilizada de cada agente com 0
	for (i = 0; i < QUANT_AGENTES; i++)
		capacidade_agentes[i] = 0;

	// Realiza os cálculos de custo e capacidade
	for (i = 0; i < QUANT_TAREFAS; i++) {
		custo += CUSTO_A_T[sol->tarefas[i]][i];
		capacidade_agentes[sol->tarefas[i]] += RECURSOS_A_T[sol->tarefas[i]][i];
	}

	// Verifica se algum agente passou sua capacidade máxima
	for (i = 0; i < QUANT_AGENTES; i++) {
		sol->excesso[i] = capacidade_agentes[i];
		sum_recursos += capacidade_agentes[i];
	
		// Se sim define esta solução como inválida
		if (capacidade_agentes[i] > CAPAC_AGENTES[i])
			solucao_invalida = 1;
	}
	
	sol->custo = custo;
	
	// Caso a solução foi excedida, altera a avaliação do indivíduo tornando-o
		// pior.
	if (solucao_invalida) 
		sol->avaliacao = ((double) sum_recursos ) * 1000000;
	else {
		sol->avaliacao = ((double) custo);
	}
	
	return sol->avaliacao;
}



/*
 * Procedimento de geração de indivíduos por meio do procedimento shift. 
 * 
 * Um detalhe a se atentar é que é realizado shift em somente 1 tarefa da 
 *	solução.
 */
void Gera_Vizinho(Solucao * atual, Solucao ** proxima) {
	int i = 0, j = 0, agente_atual = 0, agente_novo = 0, 
	tarefa_escolhida1 = 0, sum_recursos = 0, 
	quant_alteracoes = 0;
	char solucao_invalida = 0;
	
	// Copia os valores do atual
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*proxima)->tarefas[i] = atual->tarefas[i];
		if (i < QUANT_AGENTES)
			(*proxima)->excesso[i] = atual->excesso[i];
	}

	(*proxima)->custo = atual->custo;

	
	// Define uma quantidade de alterações pra gerar o vizinho
	quant_alteracoes = 2;
	
	// Altera o indivíduo
	for (i = 0; i < quant_alteracoes; i++) {

		// Escolhe a tarefa que será alterada
		tarefa_escolhida1 = random() % QUANT_TAREFAS;
		agente_atual = (*proxima)->tarefas[tarefa_escolhida1]; 

		// Gera um novo agente pra ela e certifica que ele é diferente do anterior.
		do {
			agente_novo = random() % QUANT_AGENTES;
		} while (agente_novo == agente_atual);

		// Atribui o novo agente à tarefa
		(*proxima)->tarefas[tarefa_escolhida1] = agente_novo;

		// Procedimento Otimizado:
			// A cada alteração, realiza-se a alteração dos valores da nova geração.
			// Como este novo vizinho não é feito do zero e sim sobre cópia de um anterior,
			// basta alterar os valores herdados do seu anterior, atualiando em O(1)

		// Sendo assim, atualiza o excesso de cada agente
			// Retira recurso do agente que ficou livre
		(*proxima)->excesso[agente_atual] -= RECURSOS_A_T[agente_atual][tarefa_escolhida1];
			// Acrescenta recurso do agente que recebeu a tarefa atual
		(*proxima)->excesso[agente_novo ] += RECURSOS_A_T[agente_novo ][tarefa_escolhida1]; 

		// Calcula o custo atual desta solução
		(*proxima)->custo += CUSTO_A_T[agente_novo ][tarefa_escolhida1] - CUSTO_A_T[agente_atual][tarefa_escolhida1];
	}

	// Verifica se a solução gerada é válida
	for (j = 0; j < QUANT_AGENTES; j++) {
		sum_recursos += (*proxima)->excesso[j];

		if ((*proxima)->excesso[j] > CAPAC_AGENTES[j]) {
			solucao_invalida = 1;
		}
	}

	// Calcula o fator avaliação 
	if (solucao_invalida) 
		(*proxima)->avaliacao = ((double) sum_recursos ) * 1000000;
	else {
		(*proxima)->avaliacao = ((double)  (*proxima)->custo);
	}
}



/*
 * Procedimento que instancia uma nova solução com seus valores totalmente 
 *	aleatórios.
 */
Solucao * Instancia_Solucao() {
	Solucao * s;
	
	s = calloc(1, sizeof(Solucao));
	
	s->tarefas = calloc(QUANT_TAREFAS, sizeof(int));
	s->excesso = calloc(QUANT_AGENTES, sizeof(int));
	
	// Defini-o como uma solução inicial ruim
	s->avaliacao = INT_MAX;
	s->avaliacao = INT_MAX;
	
	// Retorna a solução
	return s;
}


/*
 * Procedimento que instancia uma nova solução com seus valores totalmente 
 *	aleatórios.
 */
void Gera_Solucao_Aleatoria(Solucao ** s) {
	int i;
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*s)->tarefas[i] = random() % QUANT_AGENTES;
	}
	
	Avalia_Solucao(*s);
}


/*
 * Procedimento que copia as informações de uma solução para a outra de forma
 *	a aceitar aquela solução.
 */
char Teste_Aceita_Nova_Solucao(double temp, Solucao ** atual, Solucao * proxima) {
	int i;
	
	// Verifica se a solução atual é melhor que a atual.
	if ((*atual)->avaliacao > 2 * 1000000 || 
				((proxima->avaliacao < 2 * 1000000) && 
				(proxima->custo < (*atual)->custo))) {
		
		//Imprime_Status(temp, *atual);

		// Copia a solução
		for (i = 0; i < QUANT_TAREFAS; i++) {
			(*atual)->tarefas[i] = proxima->tarefas[i];
			if (i < QUANT_AGENTES)
				(*atual)->excesso[i] = proxima->excesso[i];
		}

		(*atual)->avaliacao = proxima->avaliacao;
		(*atual)->custo     = proxima->custo;
	
		// Retorna true se tiver aceitado.		
		return 1;
	} else {
		return 0;
	}
}


/*
 * Procedimento que copia as informações de uma solução para a outra de forma
 *	a aceitar aquela solução.
 */
void Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) {
	int i = 0;
	
	// Copia a solução 
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*atual)->tarefas[i] = proxima->tarefas[i];
		if (i < QUANT_AGENTES)
			(*atual)->excesso[i] = proxima->excesso[i];
	}

	(*atual)->avaliacao = proxima->avaliacao;
	(*atual)->custo     = proxima->custo;
}




/*
 * Procedimento que realiza a leitura da instância problema.
 */
void Le_Instancia(char * path) {
	int i, j;
	FILE * f = fopen(path, "r");

	if (f) {
		fscanf(f, "%d", &QUANT_AGENTES);
		fscanf(f, "%d", &QUANT_TAREFAS);

		if (QUANT_AGENTES < 1 || QUANT_TAREFAS < 1) {
			printf("Valores da Instância Negativos!");
			exit(2);
		}

		CAPAC_AGENTES = calloc(QUANT_AGENTES, sizeof (int));

		RECURSOS_A_T = calloc(QUANT_AGENTES, sizeof (int*));
		CUSTO_A_T = calloc(QUANT_AGENTES, sizeof (int*));

		for (i = 0; i < QUANT_AGENTES; i++) {
			RECURSOS_A_T[i] = calloc(QUANT_TAREFAS, sizeof (int));
			CUSTO_A_T[i] = calloc(QUANT_TAREFAS, sizeof (int));
		}

		for (i = 0; i < QUANT_AGENTES; i++) {
			for (j = 0; j < QUANT_TAREFAS; j++) {
				fscanf(f, "%d", &CUSTO_A_T[i][j]);
			}
		}

		for (i = 0; i < QUANT_AGENTES; i++) {
			for (j = 0; j < QUANT_TAREFAS; j++) {
				fscanf(f, "%d", &RECURSOS_A_T[i][j]);
			}
		}

		for (i = 0; i < QUANT_AGENTES; i++)
			fscanf(f, "%d", &CAPAC_AGENTES[i]);
		
		fclose(f);

	} else {
		printf("Erro ao ler Instância!\n");
		exit(-2);
	}

}




/*
 * Procedimento que informa o resultado obtido.
 */
void Imprime_Solucao(Solucao * ind) {
	
	fprintf(out, "\n\t%10lf\t", ind->custo);
	printf("\n\t%10lf\t", ind->custo);	
	
	/*for (i = 0; i < QUANT_TAREFAS; i++) {
		fprintf(out, "%3d ", ind->tarefas[i]);
		printf("%3d ", ind->tarefas[i]);
	}*/
}



/*
 * Procedimento de impressão do status atual da execução.
 */
void Imprime_Status(double t, Solucao * m) {
	int i, sum = 0;
	
	for (i = 0; i < QUANT_AGENTES; i++)
		sum += m->excesso[i];

	//printf("I:%7d\t", it);
	printf("T:%7.4lf\t", t);
	printf("Best(c,a,sum):%9.0lf|%9.0lf|%d\t", m->custo, m->avaliacao, sum);
	
	//printf("AVA(c,a):At:%9.0lf|%9.0lf\tProx:%9.0lf|%9.0lf", a->custo, a->avaliacao, p->custo, p->avaliacao);
	printf("\n");
}



/*
 * Procedimento que instancia uma nova solução com seus valores totalmente 
 *	aleatórios.
 */
Solucao * Instancia_Solucao_Aleatoria() {
	int i;
	Solucao * s;
	
	s = calloc(1, sizeof(Solucao));
	
	s->tarefas = calloc(QUANT_TAREFAS, sizeof(int));
	s->excesso = calloc(QUANT_AGENTES, sizeof(int));
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		s->tarefas[i] = random() % QUANT_AGENTES;
	}
	
	Avalia_Solucao(s);
	
	return s;
}



/*
 * Procedimento que libera a memória da solução utilizada.
 */
void Desaloca_Solucao(Solucao ** p) {
	
	free((*p)->tarefas);
	free((*p)->excesso);
	
	free(*p);
}



/*
 * Procedimento que realiza liberação de memória completa da população P
 */
void Desaloca_Populacao(Solucao *** p) {
	int i = 0;
	
	// Desaloca uma população inteira
	for (i = 0; i < TAM_POP; i++) {
		if ((*p)[i] != NULL) {
			free((*p)[i]->tarefas);
			free((*p)[i]->excesso);
		}
	}
	
	free(*p);
}



/*
 * Imprime a Instância lida
 */
void Imprime_Instancia() {
	int i = 0, j;

	printf("%d %d\n", QUANT_AGENTES, QUANT_TAREFAS);

	for (i = 0; i < QUANT_AGENTES; i++) {
		for (j = 0; j < QUANT_TAREFAS; j++) 
			printf("%d ", CUSTO_A_T[i][j]);
		printf("\n");
	}
	printf("\n");

	for (i = 0; i < QUANT_AGENTES; i++) {
		for (j = 0; j < QUANT_TAREFAS; j++) 
			printf("%d ", RECURSOS_A_T[i][j]);
		printf("\n");
	}
	
	printf("\n");

	for (i = 0; i < QUANT_AGENTES; i++)
		printf("%d ", CAPAC_AGENTES[i]);

	
	printf("\n");
}