#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

// -------------- Structs ------------------------
typedef struct {
    double media_chegada;
    double media_servico;
    double tempo_simulacao;
} parametros;

typedef struct {
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

// -------------- Funcoes Auxiliares --------------
/*
    Inicia parametros para a lei de Little
    @param: struct de parametros para little
*/
void inicia_little(little * l){
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

/*
    Le parametros para as variaveis de embasamento da simulacao
    @param: struct de parametros
*/
void le_parametros(parametros * params){
    printf("Informe o tempo medio entre clientes (s): ");
    scanf("%lF", &params->media_chegada);
    params->media_chegada = 1.0/params->media_chegada;

    printf("Informe o tempo medio de servico (s): ");
    scanf("%lF", &params->media_servico);
    params->media_servico = 1.0/params->media_servico;

    printf("Informe o tempo a ser simulado (s): ");
    scanf("%lF", &params->tempo_simulacao);
}

/*
    Calcula o uniforme
*/
double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1);
	// Limitando entre (0,1]
	u = 1.0 - u;

	return (u);
}

/*
    Funcao auxiliar para achar o minimo
*/
double min(double d1, double d2){
    if(d1 < d2) return d1;
    return d2;
}

int main(){

    // Inicializa o gerador de numeros aleatorios com uma semente baseada no tempo
    int semente = time(NULL);
    srand(semente);

    // Le valores parametrizados
    parametros params;
    le_parametros(&params);

    // Variaveis de controle da simulacao
    double tempo_coleta = 10.0; // Define o primeiro ponto de coleta em 10 segundos
    double tempo_decorrido = 0.0;
    double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme());
    double tempo_saida = DBL_MAX;

    // Variaveis de controle da fila
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    // Variaveis de medidas de interesse
    double soma_ocupacao = 0.0;
    little e_n;
    little e_w_chegada;
    little e_w_saida;

    // Inicia littles
    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

    // Utilizado para arrumar o erro de comparacao entre double
    double epsilon = 0.000001;

    // // Auxilia no grafico
    // FILE *fp = fopen("resultados.txt", "w");
    // if (!fp) {
    //     printf("Erro ao abrir o arquivo para gravação!\n");
    //     return -1;
    // }

    while(tempo_decorrido < params.tempo_simulacao){
        tempo_decorrido = min(min(tempo_chegada, tempo_saida), tempo_coleta);
        // printf("%lF\n", tempo_decorrido);

        if(fabs(tempo_decorrido - tempo_chegada) < epsilon){
            // Chegada
            // A cabeca da fila eh quem esta em atendimento
            if(!fila){
                double tempo_servico = (-1.0/params.media_servico) * log(uniforme());

                tempo_saida = tempo_decorrido + tempo_servico;

                soma_ocupacao += tempo_servico;
            }
            fila++;
            max_fila = fila > max_fila? fila:max_fila;

            tempo_chegada = tempo_decorrido + (-1.0/params.media_chegada) * log(uniforme());

            // Calculo little -- E[n]
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos++;
            e_n.tempo_anterior = tempo_decorrido;

            // Calculo little -- E[W] - chegada
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.no_eventos++;
            e_w_chegada.tempo_anterior = tempo_decorrido;
        } else if(fabs(tempo_decorrido - tempo_saida) < epsilon){
            // Saida
            fila--; // Tira o cabeca da fila
            if(fila){
                double tempo_servico = (-1.0/params.media_servico) * log(uniforme());

                tempo_saida = tempo_decorrido + tempo_servico;

                soma_ocupacao += tempo_servico;
            } else if(fabs(tempo_decorrido - tempo_coleta) < epsilon){
                e_n.soma_areas += e_n.no_eventos * (tempo_decorrido - e_n.tempo_anterior);
                e_w_chegada.soma_areas += e_w_chegada.no_eventos * (tempo_decorrido - e_w_chegada.tempo_anterior);
                e_w_saida.soma_areas += e_w_saida.no_eventos * (tempo_decorrido - e_w_saida.tempo_anterior);

                e_n.tempo_anterior = tempo_decorrido;
                e_w_chegada.tempo_anterior = tempo_decorrido;
                e_w_saida.tempo_anterior = tempo_decorrido;

                double e_n_calculo_intervalo = e_n.soma_areas / tempo_decorrido;
                double e_w_calculo_intervalo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
                double lambda_intervalo = e_w_chegada.no_eventos / tempo_decorrido;

                // Auxilia no grafico e mostra o erro de Little
                // printf("Erro de Little: %.10lf\n",
                //         e_n_calculo_intervalo - lambda_intervalo * e_w_calculo_intervalo);
                // fprintf(fp, "Em tempo %lf:\nErro de Little: %.20lf\n\n",
                //         tempo_decorrido,
                //         e_n_calculo_intervalo - lambda_intervalo * e_w_calculo_intervalo);

                // Redefinir tempo_coleta para o proximo ponto de coleta
                tempo_coleta += 10.0;

            } else{
                tempo_saida = DBL_MAX;
            }

            // Calculo little - E[n]
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            // Calculo little -- E[W] - chegada
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;
        } else{
            printf("Evento invalido!\n");
            return(1);
        }
    }
    e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;

    e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

    // Variaveis declaradas basicamente para facilitar o printf
    double e_n_calculo = e_n.soma_areas / tempo_decorrido;
    double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    double lambda =  e_w_chegada.no_eventos / tempo_decorrido;

    printf("ocupacao: %lF\n", soma_ocupacao/tempo_decorrido);
    printf("tamanho da fila: %ld\n", max_fila);
    printf("E[N]: %lF\n", e_n_calculo);
    printf("E[W]: %lF\n", e_w_calculo);
    printf("Erro de Little: %lF\n", e_n_calculo - lambda * e_w_calculo);

    return 0;
}