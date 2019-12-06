#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "modulo.h"
#define TRUE 1
#define FALSE 0

struct wav_header header;

int main(int argc, char **argv) {

	unsigned int ordem = 0,num_amostras,tam_amostra,bytes_por_canal,nconv;
	FILE *ptr_in,*ptr_out,*ptr_ri;
	char *file_in,*file_out,*file_ri,audio_format_name[10] = "";
	double *ri,*ch1_read,*ch2_read,*ch1_conv,*ch2_conv;
	unsigned char buffer4[4],buffer2[2];
	short int *dados,*dados_out;
	//float duracao_segundos;
	long low = 0l,high = 0l;

	if (argc < 4 || argc > 4) {				// Checando a identação na linha de comando
		printf("Use: ./filtro <arquivo.wav> <arquivo_ri.wav> <ordem>\n");
		return -1;
	}

	file_in = (char*) aloca_char(1024);		// Alocando memória para receber o nome do arquivo
	file_out = (char*) aloca_char(1024);
	file_ri = (char*) aloca_char(1024);
	strcpy(file_in, argv[1]);				// Copiando o parâmetro da linha de comando
	strcpy(file_out, "audio.wav.out");
	strcpy(file_ri, argv[2]);
	ordem = atoi(argv[3]) + 1;
	ptr_in = fopen(file_in, "rb");
	ptr_out = fopen(file_out, "wb");
	ptr_ri = fopen(file_ri, "r");
	ri = (double*) aloca_double(ordem);

	if (ptr_in == NULL || ptr_out == NULL || ptr_ri == NULL){	// Testa a abertura dos arquivos
		printf("Falha na abertura do arquivo.\n");
		desaloca_char(file_in);
		desaloca_char(file_out);
		desaloca_char(file_ri);
		desaloca_double(ri);
		return -1;
	}

	// Lendo as partes do HEADER do arquivo.wav
 	fread(header.chunk_id, sizeof(header.chunk_id), 1, ptr_in);
	fwrite(header.chunk_id, sizeof(header.chunk_id), 1, ptr_out);
 	printf("\n(01-04): Chunk ID: %s \n", header.chunk_id); 

	fread(buffer4, sizeof(buffer4), 1, ptr_in);
	fwrite(buffer4, sizeof(buffer4), 1, ptr_out);
	//printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
	// Convertendo de little endian para inteiro big endian 4 byte
	header.chunk_size  = buffer4[0] | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
	printf("(05-08): Chunk Size: %u bytes, %u Kb \n", header.chunk_size, header.chunk_size/1024);

	fread(header.format, sizeof(header.format), 1, ptr_in);
	fwrite(header.format, sizeof(header.format), 1, ptr_out);
	printf("(09-12): Format: %s \n", header.format);

	fread(header.subchunk1_id, sizeof(header.subchunk1_id), 1, ptr_in);
	fwrite(header.subchunk1_id, sizeof(header.subchunk1_id), 1, ptr_out);
	printf("(13-16): Subchunk1 ID: %s \n", header.subchunk1_id);
	
	fread(buffer4, sizeof(buffer4), 1, ptr_in);
	fwrite(buffer4, sizeof(buffer4), 1, ptr_out);
	header.subchunk1_size = buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
	printf("(17-20): Subchunk1 Size: %u \n", header.subchunk1_size);

	fread(buffer2, sizeof(buffer2), 1, ptr_in);
	fwrite(buffer2, sizeof(buffer2), 1, ptr_out);
 	header.audio_format = buffer2[0] | (buffer2[1] << 8);

	if (header.audio_format == 1)
		strcpy(audio_format_name,"PCM");
	else if (header.audio_format == 6)
		strcpy(audio_format_name, "A-law");
	else if (header.audio_format == 7)
		strcpy(audio_format_name, "Mu-law");

	printf("(21-22): Audio Format: %u %s \n", header.audio_format, audio_format_name);

	fread(buffer2, sizeof(buffer2), 1, ptr_in);
	fwrite(buffer2, sizeof(buffer2), 1, ptr_out);
	header.channels = buffer2[0] | (buffer2[1] << 8);
	printf("(23-24): Channels: %u \n", header.channels);

	fread(buffer4, sizeof(buffer4), 1, ptr_in);
	fwrite(buffer4, sizeof(buffer4), 1, ptr_out);
	header.sample_rate = buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
	printf("(25-28): Sample rate: %u Hz \n", header.sample_rate);

	fread(buffer4, sizeof(buffer4), 1, ptr_in);
	fwrite(buffer4, sizeof(buffer4), 1, ptr_out);
	header.byte_rate  = buffer4[0] |	(buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
	printf("(29-32): Rate: %u bytes, %u Bits \n", header.byte_rate, header.byte_rate*8);

	fread(buffer2, sizeof(buffer2), 1, ptr_in);
	fwrite(buffer2, sizeof(buffer2), 1, ptr_out);
	header.block_align = buffer2[0] | (buffer2[1] << 8);
	printf("(33-34): Block Align: %u \n", header.block_align);

	fread(buffer2, sizeof(buffer2), 1, ptr_in);
	fwrite(buffer2, sizeof(buffer2), 1, ptr_out);
 	header.bits_per_sample = buffer2[0] | (buffer2[1] << 8);
	printf("(35-36): Bits per Sample: %u \n", header.bits_per_sample);

	fread(header.subchunk2_id, sizeof(header.subchunk2_id), 1, ptr_in);
	fwrite(header.subchunk2_id, sizeof(header.subchunk2_id), 1, ptr_out);
	printf("(37-40): Subchunk2 ID: %s \n", header.subchunk2_id);

	fread(buffer4, sizeof(buffer4), 1, ptr_in);
	fwrite(buffer4, sizeof(buffer4), 1, ptr_out);
	header.subchunk2_size = buffer4[0] |	(buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24 );
	printf("(41-44): Subchunk2 Size: %u \n", header.subchunk2_size);

	num_amostras = (8 * header.subchunk2_size) / (header.channels * header.bits_per_sample);
	printf("Número de Amostras: %d \n", num_amostras);//lu
	tam_amostra = (header.channels * header.bits_per_sample) / 8;
	printf("Tamanho de Cada Amostra: %d bytes \n", tam_amostra);
	bytes_por_canal = (tam_amostra/header.channels);			
	printf("Byte por Canal: %d bytes \n",bytes_por_canal);

	// Cálculo da duração do arquivo
	//duracao_segundos = (float) header.chunk_size / header.byte_rate;
	//printf("Duração Aproximada = %fs, %s \n", duracao_segundos, segundos_tempo(duracao_segundos));

	nconv = num_amostras + ordem - 1;
	ch1_read = (double*) aloca_double(num_amostras*2);
	ch2_read = (double*) aloca_double(num_amostras*2);
	ch1_conv = (double*) aloca_double(nconv*2);
	ch2_conv = (double*) aloca_double(nconv*2);
	dados = (short int*) aloca_short_int(num_amostras*2);
	dados_out = (short int*) aloca_short_int(nconv*2);

	switch (header.bits_per_sample) {
		case 8:
			low = -128;
			high = 127;
		break;
		case 16:
			low = -32768;
			high = 32767;
		break;
		case 32:
			low = -2147483648;
			high = 2147483647;
		break;
	}

	printf("Range: %ld à %ld\n\n",low,high);

	if (header.audio_format == 1) { // Verificando se o arquivo tem o formato PCM
		printf("\n\t(1) Lendo o arquivo de RI...\n");
		ler_txt(ordem,ptr_ri,ri);
		printf("\t(2) Lendo arquivo de áudio...\n");
		fread(dados, sizeof(short int), num_amostras*2, ptr_in);
		printf("\t\tConvertendo em double...\n");
		conv_int_d(dados,num_amostras,ch1_read,ch2_read);
		printf("\t(3) Fazendo convolução...\n");
		convolucao(ordem,num_amostras,ri,ch1_read,ch1_conv);
		convolucao(ordem,num_amostras,ri,ch2_read,ch2_conv);
		printf("\t\tConvertendo em short int...\n");
		conv_d_int(ch1_conv,ch2_conv,num_amostras,dados_out);
		printf("\t(4) Escrevendo...\n");
		fwrite(dados_out, sizeof(short int), num_amostras*2, ptr_out);
		printf("\t(5) Concluído!\n\n");
	}else{
		printf("O arquivo não tem o formato PCM. Seu fomato é: %s.\n",audio_format_name);
	}

	fclose(ptr_in);
	fclose(ptr_out);
	fclose(ptr_ri);
	desaloca_char(file_in);
	desaloca_char(file_out);
	desaloca_char(file_ri);
	desaloca_double(ri);
	desaloca_double(ch1_read);
	desaloca_double(ch2_read);
	desaloca_double(ch1_conv);
	desaloca_double(ch2_conv);
	desaloca_short_int(dados);
	desaloca_short_int(dados_out);
	return 0;

}
