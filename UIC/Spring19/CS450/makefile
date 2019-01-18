src = $(wildcard *.c)
obj = $(src:.c=.o)

HW1: $(obj)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(obj) HW1
