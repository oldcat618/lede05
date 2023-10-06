OBJS:=AT-Tool.o
EXEC:=ylx_AT-Tool

all: clean $(OBJS) 
	$(CC) -o $(EXEC) $(OBJS)  $(LIBS) -Wall 

%.o:%.c 
	@$(CC) -c -o $@ $< $(CFLAGS) -fPIC

clean:
	rm -f $(EXEC) *.o