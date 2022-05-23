main:
	gcc -c -o main.o assignment01_Royster_Michael_main.c -lrt
	gcc -c -o readInput.o assignment01_Royster_Michael_readInput.c -lrt
	gcc -c -o writeReceipt.o assignment01_Royster_Michael_writeReceipt.c -lrt
	gcc -o main main.o readInput.o writeReceipt.o -lrt
clean:
	rm main
	rm *.o
	rm receipt*.txt