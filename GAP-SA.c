/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Algoritmo: Simulated Annealing.
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
#include <time.h>
#include "gap.h"

int TEMPERATURA = 0;


/*
 * Procedimento que realiza a leitura dos parâmetros de configuração do 
 *	algoritmo.
 */
void Le_Parametros(char * conf) {
	FILE * f;

	f = fopen(conf, "r");

	if (f) {
		fscanf(f, "%d", &TEMPERATURA);
		fscanf(f, "%d", &MAXIteracoes);
		
		fclose(f);
		
	} else {
		printf("Erro ao ler Configuração!\n");
		exit(-1);
	}
}



/*
 * Procedimento que realiza a alteração da temperatura por meio de método 
 *	logaritmo.
 */
void Atualiza_Temperatura(double * t) {
	*t = 0.995 * *t;
}



/*
 * Procedimento de recozimento simulado, baseado no Lopes 2008.
 */
Solucao * Simulated_Annealing() {
	int iteracoes = 0;
	double fator_Boltzmann = 0, temperatura = 0, delta = 0, condicao_parada = 0, aceitacao_aleatoria = 0;
	Solucao * melhor_s = 0, * atual_s = 0, * possivel_s = 0;
	
	// Define valores iniciais
	temperatura     = TEMPERATURA;
	condicao_parada = 0.2;
	
	// Instancia soluções aleatórias para início de execução
	melhor_s   = Instancia_Solucao_Aleatoria();
	atual_s    = Instancia_Solucao_Aleatoria();
	possivel_s = Instancia_Solucao_Aleatoria();
	
	// verifica se alguma solução aleatória gerada é boa
	Teste_Aceita_Nova_Solucao(0, &melhor_s, atual_s);
	Teste_Aceita_Nova_Solucao(0, &melhor_s, possivel_s);
	
	// Enquanto tiver temperatura suficiente
	while (temperatura > condicao_parada) {
		
		// Aperfeiçoa a solução desta temperatura.
		while(iteracoes++ < MAXIteracoes) {
			// Gera um novo vizinho
			Gera_Vizinho(atual_s, &possivel_s);

			delta = possivel_s->avaliacao - atual_s->avaliacao;
			
			// verifica se a solução atual é válida
			if (delta < 0) {
				// Se sim aceita.
				Aceita_Nova_Solucao(&atual_s, possivel_s);
				
				// Verifica se é melhor que a melhor
				Teste_Aceita_Nova_Solucao(temperatura, &melhor_s, possivel_s);
				
			} else {
				// calcula fator Boltzmann
				aceitacao_aleatoria = random() / ((double)(RAND_MAX));
				fator_Boltzmann = exp(- (delta / (double) temperatura));
				
				if (aceitacao_aleatoria < fator_Boltzmann) {
					Aceita_Nova_Solucao(&atual_s, possivel_s);
				} 
			}
		}
		
		// Atualiza temperatura
		Atualiza_Temperatura(&temperatura);

		//Imprime_Status(temperatura, melhor_s);

		iteracoes = 0;
	}

	// Desaloca soluções
	Desaloca_Solucao(&atual_s);
	Desaloca_Solucao(&possivel_s);
	
	return melhor_s;
}


/*
 * Procedimento principal
 */
int main(int argc, char** argv) {
	Solucao * solve;
	
	printf("\n/*");
	printf("\n * Trabalho de Projeto e Análise de Algoritmo");
 	printf("\n * Período 16.1");
 	printf("\n * ");
 	printf("\n * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada");
 	printf("\n * ");
 	printf("\n * Algoritmo: Simulated Annealing.");
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

	out = fopen ("out_simulated_annealing.txt", "a");
	
	solve = Simulated_Annealing();

	Imprime_Solucao(solve);
	
	Desaloca_Solucao(&solve);
	fclose(out);

	return (EXIT_SUCCESS);
}