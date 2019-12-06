all:
	gcc -g -c -Wall principal.c
	gcc -g -c -Wall modulo.c
	gcc principal.o modulo.o -o filtro -lm
	./filtro audio_acdc.wav filtro_tel_256.txt 256
