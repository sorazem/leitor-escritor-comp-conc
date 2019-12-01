## leitor-escritor-comp-conc
Implementation of an algorithm for the **reader-writer problem** without starvation.

On this version, reader and writer threads have their own identifier number and share one variable of type integer. Readers must read the variable's value and write it on a file (exclusive for each reader thread). Writers must change the value of the variable to the number of their identifier.

## Motivation
This is a work for our concurrent programming class, given by our amazing teacher Silvana.

## How to use?
Open your terminal and compile the leitor_escritor.c file using the *lpthread* flag.
```
gcc -o <name_of_executable> leitor_escritor.c -lpthread
```

To run the program you need to give the following information:
```
./<name_of_executable> <number of readers> <number of writers> <number of readings per thread> <number of writings per thread> <name of the log file>
```
**Attention! The log file must have python extension '.py'**

To run the auxiliary program and see the correctness of execution, run:
```
python <name of the log file>
```
**Attention! The auxiliary program only will run if leitor_escritor was run before. The log file name must be the same as before too.**
