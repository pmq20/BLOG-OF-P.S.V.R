objects = regenerate.o handler.o

all:ofpsvr
ofpsvr:main.c $(objects)
	cc main.c $(objects) -o $@ -L/usr/lib64/mysql -lmysqlclient -lcrypto -lmicrohttpd -lpthread -O3
$(objects): %.o: %.c
	cc $< -o $@ -c -L/usr/lib64/mysql -lmysqlclient -lcrypto -lmicrohttpd -lpthread -O3
install:ofpsvr
	cp ./ofpsvr /usr/local/bin
clean:
	rm *.o ./ofpsvr
