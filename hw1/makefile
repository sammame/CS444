hw1: hw1.o latex.o mt19937ar.o
	gcc hw1.o mt19937ar.o -std=c99 -Wall -pthread -o hw1
hw1.o: hw1.c
	gcc -c hw1.c -std=c99 -Wall -pthread
mt19937ar.o: mt19937ar.c
	gcc -c mt19937ar.c -std=c99
latex.o: hw1.tex
	pdflatex hw1.tex
clean:
	rm -f *.o *.log *.aux *.pdf hw1
