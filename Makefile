CC      := g++ -c -g -m32
LL      := g++ -g -m32
LNKFLAG := -L"./libs"
INCFLAG := -I"./include"
LDFLAGS  := -mwindows -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 
OBJFILE = GameOfLife.o  
GameOfLife:	$(OBJFILE)
	$(LL) -o GameOfLife $(OBJFILE) $(LNKFLAG) $(LDFLAGS)-lm
GameOfLife.o:	src/GameOfLife.cpp
	$(CC) $(INCFLAG) $< 
.PHONY:	clean
clean:
	-rm $(OBJFILE) GameOfLife.exe
