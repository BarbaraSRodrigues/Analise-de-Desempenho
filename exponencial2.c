#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

// gcc exponencial.c -lm -o exponencial

double uniforme(){
    double u = rand() / ((double) RAND_MAX + 1);
    // limitando entre (0,1]
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

    // Na Exponencial, E[X] = 1/l
    // E[X] = 5
    // 1/l = 5
    // l = 1/5
    // l = 0.2
    double media_chegada;
    printf("Informe o tempo medio entre clientes (s): ");
    scanf("%lF", &media_chegada);
    media_chegada = 1.0/media_chegada; 

    double media_servico;
    printf("Informe o tempo medio de servico (s): ");
    scanf("%lF", &media_servico);
    media_servico = 1.0/media_servico;

    double tempo_simulacao;
    printf("Informe o tempo a ser simulado (s): ");
    scanf("%lF", &tempo_simulacao);

    double tempo_decorrido = 0.0;

    double tempo_chegada = (-1.0/media_chegada) * log(uniforme());
    double tempo_saida = DBL_MAX;

    unsigned long int fila = 0;

    while(tempo_decorrido <  tempo_simulacao){
        tempo_decorrido  = min(tempo_chegada, tempo_saida);

        if(tempo_decorrido == tempo_chegada){ // chegada
            if(fila){
                fila++;
            } else {
                tempo_saida = tempo_decorrido + (-1.0/media_servico) * log(uniforme());
            }

            tempo_chegada = tempo_decorrido + (-1.0/media_chegada) * log(uniforme());
            
        } else if(tempo_decorrido == tempo_saida) { // saida
            if(fila){
                fila--;
                tempo_saida = tempo_decorrido + (-1.0/media_servico) * log(uniforme());
            } else {
                tempo_saida = DBL_MAX;    
            }
        } else {
            printf("Evento invalido.\n");
            return 1;
        }
    }


    return 0;
}