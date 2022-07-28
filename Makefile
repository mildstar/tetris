OBJS = tetris.o x11winlib_db.o blocks.o kbhit.o
LDLIBS = -lpthread -lX11
Target = tetris

all: $(Target)

clean:
	rm -f *.o
	rm -f $(Target)

$(Target): $(OBJS)
	gcc -o $@ $(OBJS) $(LDLIBS)