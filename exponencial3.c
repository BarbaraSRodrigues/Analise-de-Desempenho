#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef struct {
    double media_chegada;
    double media_servico;
    double tempo_simulacao;
} parametros;

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

double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1);
	// Limitando entre (0,1]
	u = 1.0 - u;

	return (u);
}

double min(double d1, double d2){
    if(d1 < d2) return d1;
    return d2;
}

int main(){
    int semente = time(NULL);
    srand(semente);

    // Le valores parametrizados
    parametros params;
    le_parametros(&params);

    // Variaveis de controle da simulacao
    double tempo_decorrido = 0.0;
    double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme());
    double tempo_saida = DBL_MAX;
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    // Variaveis de medidas de interesse
    double soma_ocupacao = 0.0;

    while(tempo_decorrido < params.tempo_simulacao){
        tempo_decorrido = min(tempo_chegada, tempo_saida);
        // printf("%lF\n", tempo_decorrido);

        if(tempo_decorrido == tempo_chegada){
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
        } else if(tempo_decorrido == tempo_saida){
            // Saida
            fila--; // Tira o cabeca da fila
            if(fila){
                double tempo_servico = (-1.0/params.media_servico) * log(uniforme());
                
                tempo_saida = tempo_decorrido + tempo_servico;

                soma_ocupacao += tempo_servico;
            }else{
                tempo_saida = DBL_MAX;
            }
        } else{
            printf("Evento invalido!\n");
            return(1);
        }
    }
    printf("ocupacao: %lF\n", soma_ocupacao/tempo_decorrido);
    printf("tamanho da fila: %ld\n", max_fila);

    return 0;
}