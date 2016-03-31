input = ex1.in
thread = 4

inputs = ex1.in ex2.in ex3.in ex4.in ex5.in
threads = 2 4 8 16

logfile = time.log
samples = 10


benchmark: serial openmp
	@rm -f $(logfile)
	
	@echo "Starting benchmark with $(samples) samples and saving on file $(logfile)"
	
	@echo "*** SERIAL ***" > $(logfile)
	@echo "*** SERIAL ***"
	
	@for I in $(inputs); do \
		echo $$I; \
		for X in $$(seq 1 $(samples)); do \
			echo "    $$T"; \
			echo -n "$$I $$X -- " >> $(logfile); \
			./maxsat-serial ./data/$$I >> $(logfile); \
		done; \
		echo >> $(logfile); \
	done;
	
	@for T in $(threads); do \
		echo "*** OpenMP $$T Threads ***" >> $(logfile); \
		echo "*** OpenMP $$T Threads ***"; \
		export OMP_NUM_THREADS=$$T; \
		for I in $(inputs); do \
			echo $$I; \
			for X in $$(seq 1 $(samples)); do \
				echo -n "$$I $$X -- " >> $(logfile); \
				./maxsat-openmp ./data/$$I >> $(logfile); \
			done; \
			echo >> $(logfile); \
		done; \
	done; 

benchmark_serial: serial
	@rm -f $(logfile)
	
	@echo "Starting benchmark with $(samples) samples and saving on file $(logfile)"
	
	@echo "*** SERIAL ***" > $(logfile)
	@echo "*** SERIAL ***"
	
	@for I in $(inputs); do \
		echo $$I; \
		for X in $$(seq 1 $(samples)); do \
			echo -n "$$I $$X -- " >> $(logfile); \
			./maxsat-serial ./data/$$I >> $(logfile); \
		done; \
		echo >> $(logfile); \
	done;

benchmark_openmp: openmp
	@echo "Starting benchmark with $(samples) samples and saving on file $(logfile)"
	
	@for T in $(threads); do \
		echo "*** OpenMP $$T Threads ***" >> $(logfile); \
		echo "*** OpenMP $$T Threads ***"; \
		export OMP_NUM_THREADS=$$T; \
		for I in $(inputs); do \
			echo $$I; \
			for X in $$(seq 1 $(samples)); do \
				echo -n "$$I $$X -- " >> $(logfile); \
				./maxsat-openmp ./data/$$I >> $(logfile); \
			done; \
			echo >> $(logfile); \
		done; \
	done; 
	
serial: maxsat-serial.c
	gcc -o maxsat-serial maxsat-serial.c -Wall -lm -fopenmp 

serial_run: serial
	@./maxsat-serial ./data/$(input)

serial_log: serial
	@echo -n "$(input) -- " > $(logfile)
	@./maxsat-serial ./data/$(input) >> $(logfile)

serial_val: serial
	@echo -n "ex1.in - " > $(logfile)
	@valgrind --track-origins=yes --leak-check=full ./maxsat-serial ./data/$(input)

serial_all: serial
	@rm -f $(logfile)
	@touch $(logfile)
	
	@for I in $(inputs); do \
	echo $$I; \
	echo -n "$$I -- " >> $(logfile); \
	./maxsat-serial ./data/$$I >> $(logfile); \
	done;
	
	@echo "DONE thank you for your patience"

openmp: maxsat-openmp.c
	gcc -o maxsat-openmp maxsat-openmp.c -Wall -lm -fopenmp 

openmp_run: openmp
	@./maxsat-openmp ./data/$(input)

openmp_run_thread: openmp
	@./maxsat-openmp ./data/$(input) $(thread)

openmp_log: openmp
	@echo -n "$(input) -- " > $(logfile)
	@./maxsat-openmp ./data/$(input) >> $(logfile)

openmp_val: openmp
	@valgrind -v --tool=helgrind ./maxsat-openmp ./data/$(input)

openmp_all: openmp
	@rm -f $(logfile)
	@touch $(logfile)
	
	@for I in $(inputs); do \
	echo $$I; \
	echo -n "$$I -- " >> $(logfile); \
	./maxsat-openmp ./data/$(input) >> $(logfile) \
	done;
	
	@echo "DONE thank you for your patience"
