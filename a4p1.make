a4p1Server: a4p1Server.c 
	g++ -pthread a4p1Server.c -o a4p1Server -lsqlite3
	./a4p1Server 129.110.92.15

a4p1Client1: a4p1Client1.c
	g++ a4p1Client1.c -o a4p1Client1 
	./a4p1Client1 129.110.92.15

a4p1Client2: a4p1Client2.c
	g++ a4p1Client2.c -o a4p1Client2 
	./a4p1Client2 129.110.92.15

a4p1Client3: a4p1Client3.c
	g++ a4p1Client3.c -o a4p1Client3 
	./a4p1Client3 129.110.92.15
