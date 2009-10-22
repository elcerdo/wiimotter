CC = gcc

COMPOPT = -O3 -Wall

CFLAGS = $(COMPOPT)

LDFLAGS = -lm -lcwiid `sdl-config --libs`

OBJS = main.o

SOURCE = .

EXEC = swiile

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(COMPOPT) $(LDFLAGS) -o $(EXEC);

%.o:$(SOURCE)/%.c $(SOURCE)/%.h
	$(CC) $(CFLAGS) $< -c -o $@

%.o:$(SOURCE)/%.c
	$(CC) $(CFLAGS) $< -c -o $@

exec: $(EXEC)
	./$(EXEC)

clean:
	$(RM) $(OBJS) $(SOURCE)/*~ $(SOURCE)/\#*\# $(EXEC)

distclean:
	$(RM) $(OBJS) $(SOURCE)/*~ $(SOURCE)/\#*\#
