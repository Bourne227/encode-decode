CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic 

default: all

all: encode decode

encode: code.h encode.c io.h io.c trie.h trie.c
	$(CC) $(CFLAGS) encode.c io.c trie.c -o encode -lm

decode: code.h decode.c io.h io.c word.h word.c
	$(CC) $(CFLAGS) decode.c io.c word.c -o decode -lm

infer:
	infer -- make

clean:
	-@rm encode 2>/dev/null || true
	-@rm decode 2>/dev/null || true
	-@rm -rf infer-out 2>/dev/null || true
