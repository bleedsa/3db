# make object files from c files
GKO := $(patsubst 3rd/%.c,$(O)/%.o,$(wildcard 3rd/gk/*.c))
KCORE := $(patsubst 3rd/gk/k.core/%.c,$(O)/k.core/%.o,$(wildcard 3rd/gk/k.core/*.c))

$(KCORE): $(O)/k.core/%.o: 3rd/gk/k.core/%.c
	$(CC) -c $(CFLAGS) -o $@ $^

$(GKO): $(O)/gk/%.o: 3rd/gk/%.c
	$(CC) -c $(CFLAGS) -o $@ $^

o/gk/gk.com: $(GKO) $(KCORE)
	$(CC) $(CFLAGS) -o $@ $^

clean-k:
	rm -rf o/gk o/k.core
