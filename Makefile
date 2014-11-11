all: s3_get

s3_get: s3_get.c
	gcc -lcurl s3_get.c -o s3_get

.PHONY: clean
clean:
	rm s3_get
