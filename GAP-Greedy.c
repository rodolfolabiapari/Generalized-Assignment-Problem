
/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Algoritmo: Guloso.
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
   Método de reinício:
      1 - Gera uma solução aleatória S
      2 - Pesquisa em uma vizinhança N(S) por uma solução melhor.
         Se a melhor solução S' pertencente a N(S) é melhor do que S, então S = S', volte para passo 2.
      3 - Atualize a melhor solução encontrada até o momento (Solução inculbente)
      4 - Se houver tempo, volta para passo 1.
 */
Solucao * Metodo_reinicio() {
	int i = 0;
	Solucao * melhor_global = 0, * atual_s = 0, * vizinha_s = 0;
	
	melhor_global  = Instancia_Solucao();
	atual_s        = Instancia_Solucao();
	vizinha_s      = Instancia_Solucao();
	
	// Inicia o contador de tempo
	start   = time(NULL);
    endwait = start + SECONDS;
	
	do {
		// Gera soluções aleatórias
		Gera_Solucao_Aleatoria(&atual_s);
		//Gera_Solucao_Aleatoria(&vizinha_s);
	
		// Testa se a solução é melhor que a atual
		Teste_Aceita_Nova_Solucao(endwait - start, &melhor_global, atual_s);
		//Teste_Aceita_Nova_Solucao(endwait - start, &melhor_global, vizinha_s);
		
		// Realiza MAXIteracoes de vizinhos a procura de soluções
			// melhoras que a atual.
		for (i = 0; i < MAXIteracoes; i++) {
			Gera_Vizinho(atual_s, &vizinha_s);
			
			if (vizinha_s->avaliacao < atual_s->avaliacao)
				Aceita_Nova_Solucao(&atual_s, vizinha_s);
		}
		
		// Verifica se na procura de soluções vizinhas, algoma é boa
		Teste_Aceita_Nova_Solucao(endwait - start, &melhor_global, atual_s);
		
		// Atualiza o tempo
		start = time(NULL);

		//Imprime_Status((double) endwait - start, melhor_global);

	} while (start < endwait);
	
	// Desaloca solução
	Desaloca_Solucao(&atual_s);
	Desaloca_Solucao(&vizinha_s);
	
	return melhor_global;
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
 	printf("\n * Algoritmo: Guloso.");
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

	out = fopen ("out_reinicio.txt", "a");
	
	solve = Metodo_reinicio();

	Imprime_Solucao(solve);
	Desaloca_Solucao(&solve);
	fclose(out);

	return (EXIT_SUCCESS);
}