.PHONY: clean

cpuhog: cpuhog.c
	gcc -Wall -Wextra -lpthread cpuhog.c -o cpuhog

clean:
	rm -f cpuhog
