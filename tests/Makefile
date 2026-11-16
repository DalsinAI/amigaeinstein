# Host tests: make -C tests test
test:
	sh test-part.sh
	sh test-patches.sh
	$(CC) -Wall -o test-wcstod test-wcstod.c -lm && ./test-wcstod && rm -f test-wcstod
.PHONY: test
