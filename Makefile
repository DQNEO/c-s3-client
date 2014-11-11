all: s3_get

s3_get: s3_get.c http_client.c
	gcc -lcurl s3_get.c http_client.c -o s3_get

.PHONY: clean
clean:
	rm s3_get
