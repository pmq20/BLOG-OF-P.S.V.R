objects = regenerate.o handler.o

all:ofpsvr
ofpsvr:main.c $(objects)
	export LD_RUN_PATH=/usr/local/lib;cc main.c $(objects) -o $@ -lmysqlclient -lcrypto -lmicrohttpd -lpthread -O3
$(objects): %.o: %.c
	cc $< -o $@ -c -lmysqlclient -lcrypto -lmicrohttpd -lpthread -O3
install:ofpsvr
	cp ./ofpsvr /usr/local/bin
clean:
	rm *.o ./ofpsvr
