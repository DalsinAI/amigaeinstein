# Host tests: make -C tests test
test:
	sh test-part.sh
	sh test-patches.sh
	$(CC) -Wall -o test-wcstod test-wcstod.c -lm && ./test-wcstod && rm -f test-wcstod
	$(CC) -Wall -o test-latin1 test-latin1.c && ./test-latin1 && rm -f test-latin1
	$(CC) -Wall -o test-wmem test-wmem.c && ./test-wmem && rm -f test-wmem
	$(CC) -Wall -o test-wide test-wide.c && ./test-wide && rm -f test-wide
.PHONY: test
