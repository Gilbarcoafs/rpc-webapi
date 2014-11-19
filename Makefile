NAME = rpc+webapi

SRCS = $(wildcard *.h) $(wildcard *.c)

all: rpc+webapi

rpc+webapi: $(SRCS)
	$(CC) $^ -lpthread -o $@

clean:
	rm -f rpc+webapi
