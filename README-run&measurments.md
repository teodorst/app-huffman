Note: Each script 'main command' was in a loop that was parsing all test files dimensions:

#!/bin/bash
for i in 10 20 30 40 50 100 200 300 400 500 1000 2000
do 
...
main command
...
done



SERIAL-COMPRESS scripts main command:

{ time ./huffman_codification_serial <INPUT-FILE> <OUTPUT-FILE> <CODIFICATION-FILE> ; } 2>> timeSerial-<NUME-COADA>.txt 

-4 scripts, one for each queue, each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -cwd ./<SCRIPT-NAME-QUEUE-NAME>


SERIAL-DECOMPRESS scripts main command:

{ time ./huffman_decodification_serial <OUTPUT-FILE> <DECODED-FILE> <CODIFICATION-FILE> ; } 2>> timeSerial-decoded-<NUME-COADA>.txt 

-4 scripts, one for each queue, each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -cwd ./<SCRIPT-NAME-QUEUE-NAME>



OPENMP-COMPRESS scripts main command:

{ time ./huffman_OpenMp-<NUMBER-OF_THREADS>   compress   <INPUT-FILE> <OUTPUT-FILE> <CODIFICATION-FILE> ; } 2>> timeOpenMp-<NUME-COADA>-<NUMBER-OF-THREADS>.txt

-12 scripts, 3 for each queue (2 threads, 4 threads, 8 threads), each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -cwd ./<SCRIPT-NAME-QUEUE-NAME-NUMBER-OF-THREADS>


OPENMP-DECOMPRESS scripts main command:

{ time ./huffman_OpenMp-<NUMBER-OF_THREADS>   decompress   <OUTPUT-FILE> <DECODED-FILE> <CODIFICATION-FILE> ; } 2>> timeOpenMp-decoded-<NUME-COADA>-<NUMBER-OF-THREADS>.txt

-12 scripts, 3 for each queue (2 threads, 4 threads, 8 threads), each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -cwd ./<SCRIPT-NAME-QUEUE-NAME-NUMBER-OF-THREADS>



PTHREADS-COMPRESS scripts main command:

{ time ./huffman_codification_pthreads-<NUMBER-OF_THREADS> <INPUT-FILE> <OUTPUT-FILE> <CODIFICATION-FILE> ; } 2>> timePthreads-<NUME-COADA>-<NUMBER-OF-THREADS>.txt

-12 scripts, 3 for each queue (2 threads, 4 threads, 8 threads), each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -cwd ./<SCRIPT-NAME-QUEUE-NAME-NUMBER-OF-THREADS>


PTHREADS-DECOMPRESS scripts main command:

{ time ./huffman_decodification_pthreads-<NUMBER-OF_THREADS> <OUTPUT-FILE> <DECODED-FILE> <CODIFICATION-FILE> ; } 2>> timePthreads-decoded-<NUME-COADA>-<NUMBER-OF-THREADS>.txt

-12 scripts, 3 for each queue (2 threads, 4 threads, 8 threads), each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -cwd ./<SCRIPT-NAME-QUEUE-NAME-NUMBER-OF-THREADS>



MPI-COMPRESS scripts main command:

{ time mpirun -n <NUMBER-OF-PROCESSES> mpi_codification <INPUT-FILE> <OUTPUT-FILE> <CODIFICATION-FILE> ; } 2>> timeMPI-<NUME-COADA>-<NUMBER-OF-PROCESSES>.txt

-12 scripts, 3 for each queue (2 procs, 4 procs, 8 procs), each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -pe openmpi <NUMBER-OF-PROCESSES> -cwd ./<SCRIPT-NAME-QUEUE-NAME-NUMBER-OF-PROCESSES>


MPI-DECOMPRESS scripts main command:

{ time mpirun -n <NUMBER-OF-PROCESSES> mpi_decodification <OUTPUT-FILE> <DECODED-FILE> <CODIFICATION-FILE> ; } 2>> timeMPI-decoded-<NUME-COADA>-<NUMBER-OF-PROCESSES>.txt

-12 scripts, 3 for each queue (2 procs, 4 procs, 8 procs), each submitted for 3 times with commands like:

qsub -q <QUEUE-NAME>.q -pe openmpi <NUMBER-OF-PROCESSES> -cwd ./<SCRIPT-NAME-QUEUE-NAME-NUMBER-OF-PROCESSES>


Final note: 
-best desription for the number of times resulted from these is ... "Ca s'avem parte de viata buna si numere mari / Am strans numere mari, mi'au placut, ..."