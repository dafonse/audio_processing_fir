struct wav_header {
	unsigned char chunk_id[4];
	unsigned int chunk_size;
	unsigned char format[4];
	unsigned char subchunk1_id[4];
	unsigned int subchunk1_size;
	unsigned int audio_format;	// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	unsigned int channels;
	unsigned int sample_rate;
	unsigned int byte_rate;
	unsigned int block_align;
	unsigned int bits_per_sample;
	unsigned char subchunk2_id[4];
	unsigned int subchunk2_size;
};

char * aloca_char(int t);
double * aloca_double(int t);
short int * aloca_short_int(int t);
void ler_txt(unsigned int ordem, FILE * ptr, double* out);
void conv_int_d(short int* dados, unsigned int amostras, double* ch1, double* ch2);
void convolucao(unsigned int ordem, unsigned int amostras, double* resposta, double* sinal, double* saida);
void conv_d_int(double* ch1, double* ch2, unsigned int amostras, short int* out);
int desaloca_char(char *vetor);
int desaloca_double(double *vetor);
int desaloca_short_int(short int *vetor);
char * segundos_tempo(float segundos);
