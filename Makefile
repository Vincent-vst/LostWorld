
output : 
	gcc server.c src/parser.c src/process.c -o server
	./server

clean : 
	rm -f server 


