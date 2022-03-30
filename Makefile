
output : 
	gcc main.c
	./a.out &
	PID=$!
	sleep 2
	kill $PID


clean : 
	rm -f a.out


