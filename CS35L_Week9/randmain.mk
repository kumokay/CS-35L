
randlibhw.so: randlibhw.c
	$(CC) randlibhw.c -shared -fPIC -o randlibhw.so

randlibsw.so: randlibsw.c
	$(CC) randlibsw.c -shared -fPIC -o randlibsw.so

randmain: randcpuid.o randmain.o
	$(CC) $(CLAGS) -ldl -Wl,-rpath=$(PWD) -o randmain randcpuid.o randmain.o
