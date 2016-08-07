
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

#include <time.h>

/*
 * Estrutura de dados para armazenamento das informações de conjunto de 
 *	soluções.
 * 
 * excesso - valor com a capacidade atual de cada agente desta solução
 * tarefas - Vetor com tamanho Tarefas onde cada endereço é indicado o agente
 *	responsável por tal
 * avaliação - Valor fitness da solução (sum_excesso + custo) * penalidade
 * custo - custo total desta solução
 */
typedef struct Struct_Solucao {
	int * excesso;
	int * tarefas;
	double avaliacao;
	double custo;
} Solucao;

int QUANT_TAREFAS   ; // Quantidade de tarefas
int QUANT_AGENTES   ; // Quantidade de agentes
int * CAPAC_AGENTES ; // Vetor com capacidade máxima de cada gente.
int ** RECURSOS_A_T ; // Matriz de recursos[a,t]
int ** CUSTO_A_T    ; // Matriz de custo [a,t]


int    TAM_POP    ; // Tamanho da população do algoritmo
int    ITERACOES  ; // Quantidade de iterações
float  TAX_CRUZAM ; // Porcentagem de cruzamentos a serem realizados
float  TAX_MUT    ; // Porcentagem dos dados do filho que serão mutados

char IMPRIMIR ; // Variável que permite impressão na tela.
int SECONDS   ; // Tempo lido pelo arquivo de configuração
int MAXIteracoes ; // Quantidade de iterações daquela temperatura
FILE * out    ; // Arquivo para gravação de dados permanente
time_t endwait, start; // Variáveis de tempo para cálculo do intervalo de 
	//tempo de execução

double Avalia_Solucao(Solucao * sol) ;
void Gera_Vizinho(Solucao * atual, Solucao ** proxima) ;
Solucao * Instancia_Solucao() ;
void Gera_Solucao_Aleatoria(Solucao ** s) ;
char Teste_Aceita_Nova_Solucao(double temp, Solucao ** atual, Solucao * proxima) ;
void Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) ;
void Le_Instancia(char * path) ;
void Imprime_Solucao(Solucao * ind) ;
void Imprime_Status(double t, Solucao * m) ;
void Desaloca_Solucao(Solucao ** p) ;
Solucao * Instancia_Solucao_Aleatoria() ;
void Desaloca_Populacao(Solucao *** p) ;
void Imprime_Instancia() ;