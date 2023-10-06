OBJS:=ModemDetect.o ModemDial.o main.o #fros_uci.o fros_queue.o cJSON.o
EXEC:=ylx_CallManager

all: clean $(OBJS) 
	$(CC) -o $(EXEC) $(OBJS)  $(LIBS) -Wall 

%.o:%.c 
	@$(CC) -c -o $@ $< $(CFLAGS) -fPIC

clean:
	rm -f $(EXEC) *.o