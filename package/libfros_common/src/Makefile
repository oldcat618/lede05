OBJS:=fros_util.o fros_uci.o fros_queue.o
LIB_NAME:=libfros_common.so
all: $(OBJS)
	$(CC) -o $(LIB_NAME) $(OBJS) $(LIBS) -shared
%.o:%.c 
	@$(CC) -c -o $@ $< $(CFLAGS) -fPIC
clean:
	rm $(LIB_NAME) *.o
