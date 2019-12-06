#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "modulo.h"
#define FALSE 0
#define MAX_INT_D 3.2768E4
#define MAX_D_INT 32000

char * aloca_char(int t){
	char *p;
	p = (char*) malloc(sizeof(char) * t);
	if(p == NULL) return NULL;
	return p;
}

double * aloca_double(int t){
	double *p;
	p = (double*) malloc(sizeof(double) * t);
	if(p == NULL) return NULL;
	return p;
}

short int * aloca_short_int(int t){
	short int *p;
	p = (short int*) malloc(sizeof(short int) * t);
	if(p == NULL) return NULL;
	return p;
}

void ler_txt(unsigned int ordem, FILE* ptr, double* out){
	unsigned int i;
	for (i = 0; i < ordem; i++){
		fscanf(ptr, "%lf", &out[i]);
	}
}

void conv_int_d(short int* dados, unsigned int amostras, double* ch1, double* ch2){
	unsigned int i,x = 0,v = 0;
	double amostra_convertida;
	for (i = 0; i < amostras*2; i++) {
		amostra_convertida = ((double) dados[i])/(MAX_INT_D);
		if (((i + 2) % 2) == FALSE) {
			ch1[x] = amostra_convertida;
			x++;
		} else {
			ch2[v] = amostra_convertida;
			v++;
		}
	}
}

void convolucao(unsigned int ordem, unsigned int amostras, double* resposta, double* sinal, double* saida){
    int i, j, k;
    for(i = ordem - 1; i < amostras; i++){		// comeca convolução de saida[ordem - 1] até saida[amostras - 1]
        saida[i] = 0;
        for(j = i, k = 0; k < ordem; j--, k++)
            saida[i] = saida[i] + sinal[j] * resposta[k];
    }
    for(i = 0; i < ordem - 1; i++){				// convolução de saida[0] até saida[ordem - 2]
        saida[i] = 0;
        for(j = i, k = 0; j >= 0; j--, k++)
            saida[i] = saida[i] + sinal[j] * resposta[k];
    }
}

void conv_d_int(double* ch1, double* ch2, unsigned int amostras, short int* out){
	unsigned int i,x = 0,v = 0;
	for (i = 0; i < amostras*2; i++) {
		if (((i + 2) % 2) == FALSE) {
			out[i] = (short int)(ch1[x] * MAX_D_INT);
			x++;
		} else {
			out[i] = (short int)(ch2[v] * MAX_D_INT);
			v++;
		}
	}
}

int desaloca_char(char *vetor){
	free(vetor);
	return 0;
}

int desaloca_double(double *vetor){
	free(vetor);
	return 0;
}

int desaloca_short_int(short int *vetor){
	free(vetor);
	return 0;
}

// Conversão de segundos para tempo no formato h:m:s, retornado como string
char * segundos_tempo(float segundos) {

	int ipos,hora,resto_hora,minuto,segundo,milisegundo;
	char *hms,*pos,decimal[15];

	hms = (char*) malloc(100);
	sprintf(hms, "%f", segundos);
	hora = (int) segundos/3600;
	resto_hora = (int) segundos % 3600;
	minuto = resto_hora/60;
	segundo = resto_hora % 60;
	milisegundo = 0;

	// Transformando a parte decimal de segundo para milisegundo
	pos = strchr(hms, '.');
	ipos = (int) (pos - hms);
	memset(decimal, ' ', sizeof(decimal));
	strncpy(decimal, &hms[ipos+1], 3);
	milisegundo = atoi(decimal);
	sprintf(hms, "%dh:%dm:%ds%dms", hora, minuto, segundo, milisegundo);
	return hms;
}
