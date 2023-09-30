OBJS:=ModemDetect.o main.o cJSON.o
EXEC:=YLX_CallManager

all: $(OBJS) 
	$(CC) -o $(EXEC) $(OBJS)  $(LIBS) -Wall 

%.o:%.c 
	@$(CC) -c -o $@ $< $(CFLAGS) -fPIC

clean:
	rm -f $(EXEC) *.o