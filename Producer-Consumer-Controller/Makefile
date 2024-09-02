compile:
	gcc -lpthread -lrt Reset.c -o reset
	gcc -lpthread -lrt Producer.c -o producer
	gcc -lpthread -lrt Consumer.c -o consumer
	gcc -lpthread -lrt Controller.c -o controller

run: run_reset run_producer run_controller

run_reset:
	./reset

run_producer:
	./producer

run_consumer1:
	sleep 5
	./consumer

run_consumer2:
	sleep 10
	./consumer

run_controller:
	sleep 5
	./controller