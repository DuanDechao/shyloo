VAPTH = %.h ../sllib:../sllib/net:../sllib/ipc

EXE=appsvr
MACRO = DEBUGALL
CFLAGS += -g -ldl
SOURCES = $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o,$(SOURCES))

CC = g++

$(EXE): $(OBJS)
	@echo "source files:" $(SOURCES)
	@echo "Object files:" $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(EXE)

sinclude $(SOURCES:.cpp=.d)
%d: %cpp
	echo "create depend"
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

clean:
	rm -rf $(OBJS)
	rm -f $(EXE)
