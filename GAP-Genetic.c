/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Algoritmo: Genético.
 * Data: 01/08/2016.
 * Distribuição Livre, desde que referenciando o autor.
 * 
 * Professor: Haroldo Santos
 * 
 * Autor do Trabalho: Rodolfo Labiapari Mansur Guimarães
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gap.h"



/*
 * Procedimento que realiza a alocação de memória para novas instâncias.
 * 
 * O procedimento instancia a população, mas não os indivíduos.
 */
void Instancia_Populacoes(Solucao *** pop) {
	(*pop) = calloc(TAM_POP, sizeof (Solucao*));
}



/*
 * Procedimento que imprime os dados de toda a população.
 */
void Imprime_Populacao (Solucao ** p) {
	int i = 0, j = 0;
	if (IMPRIMIR) {
		
		printf("\n\n");

		for (i = 0; i < TAM_POP; i++) {
			printf("[%3d] - ", i);
			if (p[i] != NULL) {
				for (j = 0; j < QUANT_TAREFAS; j++) {
					printf("%1d  ", p[i]->tarefas[j]);
				}

				printf("\t%10.1lf\n", p[i]->avaliacao);
			} else 
				printf("\n");

			fflush(stdout);
		}

		fprintf(out, "\n\n");

		for (i = 0; i < TAM_POP; i++) {
			fprintf(out, "[%3d] - ", i);
			if (p[i] != NULL) {
				for (j = 0; j < QUANT_TAREFAS; j++) {
					fprintf(out, "%1d  ", p[i]->tarefas[j]);
				}

				fprintf(out, "\t%10.1lf\n", p[i]->avaliacao);
			} else 
				fprintf(out, "\n");

			fflush(out);
		}
	}
}



/*
 * Procedimento que imprime de forma inteligente, quais índices da população
 *	estão preenchidos e quais estão vazios.
 */
void Imprime_Dados_Populacao (Solucao ** p) {
	int i = 0;
	
	printf("\n\n");
	
	for (i = 0; i < TAM_POP; i++) {
		printf("[%5d] ", i);
		printf("c%5.0lf / sum = a%12.6lf", p[i]->custo, p[i]->avaliacao);
		
		if (p[i]->custo > 5 * 209) {
			printf(" X\n");
		} else {
			printf("\n");
		}
	}
}



/*
 * Procedimento que realiza a leitura dos parâmetros de configuração do 
 *	algoritmo.
 */
void Le_Parametros(char * conf) {
	FILE * f = 0;

	f = fopen(conf, "r");

	if (f) {
		fscanf(f, "%d", &TAM_POP);
		fscanf(f, "%f", &TAX_CRUZAM);
		fscanf(f, "%f", &TAX_MUT);
		fscanf(f, "%d", &SECONDS);
		
		fclose(f);
		
	} else {
		printf("Erro ao ler Configuração!\n");
		exit(-1);
	}
}



/*
 * Procedimento que seleciona indivíduos não repetidos aleatoriamente da 
 *	população anterior adicionando-os na nova e eliminando os indivíduos 
 *	rejeitados.
 */
void Seleciona_Nova_Geracao(Solucao *** atual, Solucao *** proxima) {
	int i = 0, buffer[TAM_POP], indice = 0;
	Solucao ** ind_buffer = 0;
	
	// Inicializa o buffer de usados como 'nenhum item utilizado'
	for (i = 0; i < TAM_POP; i++)
		buffer[i] = 0;
	
	// Para cada vagas da próxima população ainda não preenchida
	for (i = (int) TAM_POP * TAX_CRUZAM + 1; i < TAM_POP; i++) {
		
		// Escolhe um indivíduos da antiga população que ainda não foram 
			// escolhidos (a fim de não gerar uma população com indivíduos 
			// idênticos.
		do {
			indice = random() % TAM_POP;
		} while (buffer[indice] == 1);
		
		// Define-o como utilizado.
		buffer[indice] = 1;
		// Referencia-o na nova população.
		(*proxima)[i] = (*atual)[indice];
	}
	
	// Assim, alguns indivíduos não serão referenciados e por isso devem ser
		// eliminados
	// Para cada item da população antiga
	for (i = 0; i < TAM_POP; i++) {
		// Verifica se o item não foi escolhido.
		if (buffer[i] == 0) {
			// Se sim, elimina-o
			free((*atual)[i]->tarefas);
			(*atual)[i]->tarefas = 0;
			(*atual)[i]->avaliacao = -1;
		}
		
		// Reseta a população antiga para se tornar um 'proxima população'
		(*atual)[i] = NULL;
	}
	
	// Comuta as populações.
	ind_buffer = (*atual);
	(*atual) = (*proxima);
	(*proxima) = ind_buffer;	
}



/*
 * Procedimento que copia os dados de um indivíduo para outro.
 */
void Copia_Melhor_Solucao(Solucao ** p, Solucao ** best){
	int i = 0;
	Solucao * best_pop_local = 0;
	
	// Inicia-se definido que o melhor é o primeiro indivíduo.
	best_pop_local = p[0];
	
	// Compara-se o primeiro com os outros de forma a escolher o melhor 
		// indivíduo de toda a população
	for (i = 1; i < TAM_POP; i++) {
		if (p[i]->avaliacao < best_pop_local->avaliacao)
			best_pop_local = p[i];
	}
	
	// Se o indivíduo best ainda não foi criado, cria-o
	if (*best == NULL) {
		// Cria indivíduo
		*best = calloc (1, sizeof(Solucao));
		(*best)->tarefas = calloc(QUANT_TAREFAS, sizeof(int));
		(*best)->excesso = calloc(QUANT_AGENTES, sizeof(int));
		
		// Copia do melhor encontrado
		(*best)->avaliacao = best_pop_local->avaliacao;
		(*best)->custo = best_pop_local->custo;
		
		// Copia os dados 'tarefa' e 'excesso'
		for (i = 0; i < QUANT_TAREFAS; i++) {
			(*best)->tarefas[i] = best_pop_local->tarefas[i];
			if (i < QUANT_AGENTES)
				(*best)->excesso[i] = best_pop_local->excesso[i];
		}
		
	} else {
		// verifica se o melhor encontrado é melhor que o indivíduo atual
		if ((*best)->avaliacao > 2 * 1000000 || 
				((best_pop_local->avaliacao < 2 * 1000000) && 
				(best_pop_local->custo < (*best)->custo))) {

			// Copia os dados 'tarefa' e 'excesso'// 
			for (i = 0; i < QUANT_TAREFAS; i++) {
				(*best)->tarefas[i] = best_pop_local->tarefas[i];
				if (i < QUANT_AGENTES)
					(*best)->excesso[i] = best_pop_local->excesso[i];
			}
			
			// Copia do melhor indivíduo
			(*best)->avaliacao = best_pop_local->avaliacao;
			(*best)->custo      = best_pop_local->custo;
		}
	}
}



/*
 * Procedimento que recombina dois indivíduos gerando um terceiro por meio de
 *	recombinação uniforme.
 */
int * Recombina(Solucao * i1, Solucao * i2) {
	int i = 0, * tarefas = 0;

	// Cria um novo vetor de tarefas
	tarefas = calloc(QUANT_TAREFAS, sizeof(int));
	
	// Recombina de forma uniforme
	for (i = 0; i < QUANT_TAREFAS; i++) {

		// Escolhe de forma uniforme sobre dois indivíduos
		if (random() % 2) 
			tarefas[i] = i1->tarefas[i];
		else
			tarefas[i] = i2->tarefas[i];
		
	}

	return tarefas;
}



/*
 * Procedimento que cria vários indivíduos aleatórios preenchendo a população.
 *	Além disso, é realizado a avaliação de cada um destes.
 */
void Cria_Nova_Populacao(Solucao *** P) {
	int i = 0, j = 0, k = 0, menor = 0;
	Solucao ** p_local = 0;

	p_local = *P;

	// Para cada item a ser criado
	for (i = 0; i < TAM_POP; i++) {
		
		// Aloca suas variáveis que armazenarão suas informações
		p_local[i]          = calloc(1, sizeof(Solucao));
		p_local[i]->excesso = calloc(QUANT_AGENTES, sizeof(int));
		p_local[i]->tarefas = calloc(QUANT_TAREFAS, sizeof(int));
		
		// Gera valores pra este indivíduo
		for (j = 0; j < QUANT_TAREFAS; j++) {
			// O primeiro indivíduo será gerado de forma gulosa e os outros
				// Serão uma mistura de Guloso com Aleatoriedade
			
			// Se não for o primeiro indivíduo, possui 66% de gerar valores
				// por meio de função randomica
			if (i > 0 && random() % 3 != 0) {
				p_local[i]->tarefas[j] = random() % QUANT_AGENTES;
				
			// Caso contrário, utiliza uma geração gulosa pra esta tarefa.
			} else {
				// O método guloso escolhe o recurso mais leve desta tarefa
				p_local[i]->tarefas[j] = 0;
				menor = 0;

				// Seleciona o agente que utiliza o menor recurso desta
					// tarefa
				for (k = 1; k < QUANT_AGENTES; k++) {
					if (RECURSOS_A_T[k][j] < RECURSOS_A_T[menor][j]) {
						menor = k;
						p_local[i]->tarefas[j] = k;
					}
				}
			}
		}

		// Avalia o novo indivíduo gerado
		Avalia_Solucao(p_local[i]);
	}
}



/*
 * Procedimento que realiza a seleção de dois indivíduos para a geração de um
 *	terceiro.
 */
void Gera_Filhos(Solucao ** selecao, Solucao *** filhos) {
	int i = 0, j = 0, numero_torneio = 0;
	Solucao * i1 = 0, * i2 = 0, * buffer = 0;
	
	Solucao ** filhos_local = * filhos;
	
	// O primeiro indivíduo gerado é o indivíduo com melhor avaliação de toda
		// população. Sendo assim, é realizado uma busca na população do 
		// melhor indivíduo e adiciona-o na próxima geração.
	j = 0;
	for (i = 1; i < TAM_POP; i++)
		if (selecao[i]->avaliacao < selecao[j]->avaliacao)
			j = i;
	
	// Aloca memória pra nova solução
	filhos_local[0]            = calloc(1, sizeof(Solucao));
	filhos_local[0]->excesso   = calloc(QUANT_AGENTES, sizeof(int));
	filhos_local[0]->tarefas   = calloc(QUANT_TAREFAS, sizeof(int));
	
	filhos_local[0]->avaliacao = selecao[j]->avaliacao;
	
	// Copia as tarefas do elemento escolhido
	for (i = 0; i < QUANT_TAREFAS; i++)
		filhos_local[0]->tarefas[i] = selecao[j]->tarefas[i];
	

	// Para os outros indivíduos que devem ser gerados, serão selecionados dois
		// pais por torneio e realizados a sua recombinação
	for (i = 1; i < (int) TAM_POP * TAX_CRUZAM + 1; i++) {
	
		// Quantidade de indivíduos que disputarão o torneio
		numero_torneio = 1 + random() % ((TAM_POP / 2) + 1);
	
		// Aloca o novo filho ainda vazio.
		filhos_local[i] = calloc(1, sizeof(Solucao));
		filhos_local[i]->excesso = calloc(QUANT_AGENTES, sizeof(int));
		
		// Realiza o torneio 1
		i1 = selecao[random() % TAM_POP];
		
		// Realiza torneio pra seleção do primeiro pai
		j = 0;
		while (j++ < numero_torneio) {
			buffer = selecao[random() % TAM_POP];
			
			if (buffer->avaliacao < i1->avaliacao)
				i1 = buffer;
		}
		
		// Realiza o torneio 2
		i2 = selecao[random() % TAM_POP];
		
		// Realiza torneio pra seleção do segundo pai
		j = 0;
		while (j++ < numero_torneio) {
			buffer = selecao[random() % TAM_POP];
			
			if (buffer->avaliacao < i2->avaliacao)
				i2 = buffer;
		}
		
		// Verifica se os dois pais são idênticos, caso sim, gera um outro
			// pai aleatório sem torneio e seleciona-o
		if (i1 == i2)
			i2 = selecao[random() % TAM_POP];
		
		// Recombina os dois indivíduos
		filhos_local[i]->tarefas = Recombina(i1, i2);
	}
}



/*
 * Procedimento que realiza a mutação dos filhos por meio do processo Creep
 *	Mutation.
 * 
 * Não são todos os filhos que são mutados. Eles são escolhidos aleatoriamente 
 *	e TAX_MUT representa a porcentagem de mutação que cada indivíduo receberá.
 * Ao final, este novo é avaliado.
 */
void Creep_Mutation(Solucao *** pop) {
	int i = 0, j = 0, indice_tarefa = 0, quant_filhos = 0, agente_atual = 0;

	// Quantidade de filhos gerados nesta próxima geração
	quant_filhos = (int) TAM_POP * TAX_CRUZAM + 1;

	// Para cada filho
	for (i = 0; i < quant_filhos; i++) {

		// Realiza mutação em uma porcentagem de do indivíduo
		for (j = 0; j < (int) QUANT_TAREFAS * TAX_MUT; j++) {

			// Muta a tarefa do indivíduo
			indice_tarefa = random() % QUANT_TAREFAS;

			agente_atual = (*pop)[i]->tarefas[indice_tarefa];

			do {
				(*pop)[i]->tarefas[indice_tarefa] = random() % QUANT_AGENTES;
			} while ((*pop)[i]->tarefas[indice_tarefa] == agente_atual);
		}	

		// Avalia o novo indivíduo gerado.
		Avalia_Solucao((*pop)[i]);
	}	
}



/*
 * Processo de geração de filhos e mutação.
 */
void Recombinacao(Solucao ** atual_p, Solucao *** proxima_p) {
	
	// Gera filhos da próxima geraçãp
	Gera_Filhos(atual_p, proxima_p);

	// Muta os filhos gerados
	Creep_Mutation(proxima_p);
}


/*
 * Algoritmo Genético segundo Lopes 2008.
 */
Solucao * Algoritmo_Genetico() {
	Solucao ** p_atual = 0, ** p_proxima = 0;
	Solucao * melhor_Solucao = 0;

	// Instancia as duas populações
	Instancia_Populacoes(&p_atual);
	Instancia_Populacoes(&p_proxima);
	
	// Gera a população inicial para execução
	Cria_Nova_Populacao(&p_atual);
	
	// Copia o melhor indivíduo gerado aleatoriamnte
	Copia_Melhor_Solucao(p_atual, &melhor_Solucao);
	
	// Inicia o contador de tempo
	start   = time(NULL);
    endwait = start + SECONDS;
	
	do {
		// Realiza a recombinação gerando novos filhos já mutados
		Recombinacao(p_atual, &p_proxima);
		
		// Completa a população com indivíduos da população anterior
		Seleciona_Nova_Geracao(&p_atual, &p_proxima);
		
		// Verifica a existência de um indivíduo melhor que o atual conhecido
		Copia_Melhor_Solucao(p_atual, &melhor_Solucao);
		
		// Recebe o tempo atual.
		start = time(NULL);
		
		// Verifica se o tempo excedeu o limite estabelecido por parâmentro.
	} while (start < endwait);

	// Desaloca populações após o fim do algoritmo
	Desaloca_Populacao(&p_atual);
	Desaloca_Populacao(&p_proxima);
	
	// Retorna o melhor indivíduo encontrado
	return melhor_Solucao;
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
 	printf("\n * Algoritmo: Genético.");
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
	
	out = fopen ("out_genetico.txt", "a");
	
	solve = Algoritmo_Genetico();
	
	Imprime_Solucao(solve);
	
	free(solve->tarefas);
	free(solve);
	
	fflush(out);
	fclose(out);
	
	fflush(stdout);

	return (EXIT_SUCCESS);
}