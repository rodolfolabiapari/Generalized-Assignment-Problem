/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Algoritmo: GRASP.
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
#include <time.h>
#include "gap.h"


/*
 * Procedimento que realiza a leitura dos parâmetros de configuração do 
 *	algoritmo.
 */
void Le_Parametros(char * config) {
	FILE * f;

	f = fopen(config, "r");

	if (f) {
		fscanf(f, "%d", &SECONDS);
		fscanf(f, "%d", &MAXIteracoes);
		
		fclose(f);
		
	} else {
		printf("Erro ao ler Configuração!\n");
		exit(-1);
	}
}



/*
 * Método Guloso Randômico
 */
void GreedyRandomizedConstruction(Solucao ** s, float alfa) {
	int i = 0, j = 0, min = 0, max = 0, fator = 0, sum_recursos = 0;
	char solucao_invalida = 0;

	for (i = 0; i < QUANT_AGENTES; i++)
		(*s)->excesso[i] = 0;

	(*s)->custo = 0;
	
	// Para cada tarefa
	for (i = 0; i < QUANT_TAREFAS; i++) {
		min = max = 0;

		// Encontra os valores máximos e mínimos dos agentes
		for (j = 1; j < QUANT_AGENTES; j++) {
			if (RECURSOS_A_T[j][i] + CUSTO_A_T[j][i] < RECURSOS_A_T[min][i] + CUSTO_A_T[min][i])
				min = j;
			
			if (RECURSOS_A_T[j][i] + CUSTO_A_T[j][i] > RECURSOS_A_T[max][i] + CUSTO_A_T[max][i])
				max = j;
		}
		
		// Calcula um fator de acordo com o valor alfa
		fator = RECURSOS_A_T[min][i] + alfa * (RECURSOS_A_T[max][i] - RECURSOS_A_T[min][i]);

		// procura o agente que tem maior proximidade com o fator
		min = 0;
		for (j = 1; j < QUANT_AGENTES; j++) {
			if (abs(RECURSOS_A_T[j][i] - fator) < abs(RECURSOS_A_T[min][i] - fator))
				min = j;
		}
		
		// Atribui a esta tarefa
		(*s)->tarefas[i] = min;

		// Calcula a quantidade de recusto utilizado ao atribuir a nova tarefa ao agente.
		(*s)->excesso[min] += RECURSOS_A_T[min][i];

		// Calcula o custo daquela tarefa
		(*s)->custo += CUSTO_A_T[min][i];
	}

	// Verifica se a solução gerada é válida
	for (j = 0; j < QUANT_AGENTES; j++) {
		sum_recursos += (*s)->excesso[j];

		if ((*s)->excesso[j] > CAPAC_AGENTES[j]) {
			solucao_invalida = 1;
		}
	}

	// Calcula o fator avaliação 
	if (solucao_invalida) 
		(*s)->avaliacao = ((double) sum_recursos ) * 1000000;
	else {
		(*s)->avaliacao = ((double) (*s)->custo);
	}
}
	

/*
 * Método GRASP
 */
Solucao * GRASP() {
	int i = 0;
	Solucao * melhor_global = 0, * atual_s = 0, * vizinha_s = 0;
	
	// Instancia soluções
	melhor_global  = Instancia_Solucao();
	atual_s        = Instancia_Solucao();
	vizinha_s      = Instancia_Solucao();
	
	// Inicia o contador de tempo
	start   = time(NULL);
    endwait = start + SECONDS;
	
	do {
		// Gera um novo indivíduo
		GreedyRandomizedConstruction(&atual_s, random() / RAND_MAX);
		
		// Verifica sua avaliação
		Teste_Aceita_Nova_Solucao(endwait - start, &melhor_global, atual_s);
		
		// Gera N vizinhos e verifica suas avaliações
		for (i = 0; i < MAXIteracoes; i++) {
			Gera_Vizinho(atual_s, &vizinha_s);
			
			if (vizinha_s->avaliacao < atual_s->avaliacao)
				Aceita_Nova_Solucao(&atual_s, vizinha_s);
		}
		
		// Testa se é a melhor solução encontrada
		Teste_Aceita_Nova_Solucao(endwait - start, &melhor_global, atual_s);	
		
		// Verifica se ainda possui tempo
		start = time(NULL);
	} while (start < endwait);
	
	Desaloca_Solucao(&atual_s);
	Desaloca_Solucao(&vizinha_s);

	return melhor_global;
}




/*
 * 
 */
int main(int argc, char** argv) {
	Solucao * solve;

	printf("\n/*");
	printf("\n * Trabalho de Projeto e Análise de Algoritmo");
 	printf("\n * Período 16.1");
 	printf("\n * ");
 	printf("\n * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada");
 	printf("\n * ");
 	printf("\n * Algoritmo: GRASP.");
 	printf("\n * Data: 01/08/2016.");
 	printf("\n * Distribuição Livre, desde que referenciando o autor.");
 	printf("\n * ");
 	printf("\n * Professor: Haroldo Santos");
 	printf("\n * ");
 	printf("\n * Autor do Trabalho: Rodolfo Labiapari Mansur Guimarães");
 	printf("\n */");
	
	if (argc != 4) {
		printf("\n\nErro nos parâmetros! Quantidade lida: %d\t Quantidade requerida: %d.", argc, 4);
		printf("\nnome_programa arq_configuracao arq_instancia seed\n\n");
		exit(-1);
	}
 	//printf("\n\nExecutando...\n");
	
	char * instancia = argv[2];
	char * configuracao = argv[1];
	srand(atoi(argv[3]));
	
	Le_Instancia(instancia);
	
	Le_Parametros(configuracao);

	out = fopen ("out_grasp.txt", "a");
	
	solve = GRASP();

	Imprime_Solucao(solve);
	Desaloca_Solucao(&solve);
	fclose(out);

	return (EXIT_SUCCESS);
}