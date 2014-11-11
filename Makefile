all: s3_get

s3_get: s3_get.c s3_client.c http_client.c s3_client.h http_client.h
	gcc -I/usr/local/include/glib-2.0/glib -I/usr/local/include/glib-2.0  -I/usr/local/Cellar/glib/2.40.0_1/lib/glib-2.0/include  -lcurl -lcrypto -lglib-2.0 s3_get.c http_client.c s3_client.c -o s3_get

.PHONY: clean
clean:
	rm -f s3_get
