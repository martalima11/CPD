input = ex1.in
thread = 4

inputs = ex1.in ex2.in ex3.in ex4.in ex5.in
threads = 2 4 8

logdir = ./data/logs
logfile = time.log
samples = 3

all: serial openmp

benchmark: serial openmp
	@echo "Starting benchmark with $(samples) samples and saving on file $(logdir)/$(logfile)"

	@echo "*** SERIAL ***" > $(logdir)/$(logfile)
	@echo "*** SERIAL ***"

	@for I in $(inputs); do \
		echo -n "$$I "; \
		echo -n "$$I -- " >> $(logdir)/$(logfile); \
		./maxsat-serial ./data/$$I >> $(logdir)/$(logfile); \
		echo "DONE."; \
	done;

	@echo >> $(logdir)/$(logfile)
	@for T in $(threads); do \
		echo "*** OpenMP $$T Threads ***" >> $(logdir)/$(logfile); \
		echo "*** OpenMP $$T Threads ***"; \
		export OMP_NUM_THREADS=$$T; \
		for I in $(inputs); do \
			echo $$I; \
			echo $$I >> $(logdir)/$(logfile); \
			for X in $$(seq 1 $(samples)); do \
				echo -n "- $$X "; \
				echo -n "- $$X - " >> $(logdir)/$(logfile); \
				./maxsat-openmp ./data/$$I >> $(logdir)/$(logfile); \
				echo "DONE."; \
			done; \
		done; \
		echo >> $(logdir)/$(logfile); \
	done;

benchmark_serial: serial
	@echo "Starting benchmark with $(samples) samples and saving on file $(logdir)/$(logfile)"

	@echo "*** SERIAL ***" > $(logdir)/$(logfile)
	@echo "*** SERIAL ***"

	@for I in $(inputs); do \
		echo $$I; \
		echo -n "$$I -- " >> $(logdir)/$(logfile); \
		./maxsat-serial ./data/$$I >> $(logdir)/$(logfile); \
		echo >> $(logdir)/$(logfile); \
	done;

benchmark_openmp: openmp
	@rm -f $(logdir)/$(logfile)
	@echo "Starting benchmark with $(samples) samples and saving on file $(logdir)/$(logfile)"

	@for T in $(threads); do \
		echo "*** OpenMP $$T Threads ***" >> $(logdir)/$(logfile); \
		echo "*** OpenMP $$T Threads ***"; \
		export OMP_NUM_THREADS=$$T; \
		for I in $(inputs); do \
			echo $$I; \
			echo $$I >> $(logdir)/$(logfile); \
			for X in $$(seq 1 $(samples)); do \
				echo -n "- $$X - "; \
				echo -n "- $$X - " >> $(logdir)/$(logfile); \
				./maxsat-openmp ./data/$$I >> $(logdir)/$(logfile); \
				echo "DONE."; \
			done; \
		done; \
		echo >> $(logdir)/$(logfile); \
	done;

serial: maxsat-serial.c maxsat.o
	gcc -o maxsat-serial maxsat-serial.c maxsat.o -Wall -lm -fopenmp

serial_run: serial
	@./maxsat-serial ./data/$(input)

serial_log: serial
	@echo -n "$(input) -- " > $(logdir)/$(logfile)
	@./maxsat-serial ./data/$(input) >> $(logdir)/$(logfile)

serial_val: serial
	@echo -n "$(input) - " > $(logdir)/$(logfile)
	@valgrind --track-origins=yes --leak-check=full ./maxsat-serial ./data/$(input)

serial_all: serial
	@rm -f $(logdir)/$(logfile)
	@touch $(logdir)/$(logfile)

	@for I in $(inputs); do \
	echo $$I; \
	echo -n "$$I -- " >> $(logdir)/$(logfile); \
	./maxsat-serial ./data/$$I >> $(logdir)/$(logfile); \
	done;

	@echo "DONE thank you for your patience"

openmp: maxsat-openmp.c maxsat.o
	gcc -o maxsat-openmp maxsat-openmp.c maxsat.o -Wall -lm -fopenmp

openmp_run: openmp
	@./maxsat-openmp ./data/$(input)

openmp_run_thread: openmp
	@./maxsat-openmp ./data/$(input) $(thread)

openmp_log: openmp
	@echo -n "$(input) -- " > $(logdir)/$(logfile)
	@./maxsat-openmp ./data/$(input) >> $(logdir)/$(logfile)

openmp_val: openmp
	@valgrind -v --tool=helgrind ./maxsat-openmp ./data/$(input)

openmp_all: openmp
	@rm -f $(logdir)/$(logfile)
	@touch $(logdir)/$(logfile)

	@for I in $(inputs); do \
	echo $$I; \
	echo -n "$$I -- " >> $(logdir)/$(logfile); \
	./maxsat-openmp ./data/$$I >> $(logdir)/$(logfile); \
	done;

	@echo "DONE thank you for your patience"

mpi: maxsat-mpi.c maxsat.o task.o
	mpicc -o maxsat-mpi $^ -Wall -lm -g -fopenmp

mpi_run: mpi
	mpirun -np 4 maxsat-mpi ./data/$(input)

mpi_run_dist: mpi
	mpirun -mca plm_rsh_agent /usr/local/bin/rnlssh --host lab13p1,lab13p2,lab13p4,lab13p5 maxsat-mpi ./data/$(input)

maxsat.o: maxsat.c maxsat.h
	gcc -c -o maxsat.o maxsat.c -Wall -lm

task.o: task.c task.h
	gcc -c -o task.o task.c -Wall -lm

clean:
	@rm -f *.o
	@rm -f maxsat-serial maxsat-openmp maxsat-mpi
	@rm -f data/*.out
	@echo "Done!"
