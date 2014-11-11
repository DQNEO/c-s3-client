all: s3_get

s3_get: s3_get.c
	gcc -I/usr/local/opt/apr/include/apr-1 -lcurl s3_get.c -o s3_get

