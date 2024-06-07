TARGET = out

IDIR = .
SDIR = .
ODIR = .

SRC = $(wildcard $(SDIR)/*.cpp)
OBJ = $(SRC:$(SDIR)/%.c=$(ODIR)/%.o)

CXX = g++
CXXFLAGS = -I$(IDIR)
CXXFLAGS += -g -O0
CXXFLAGS += -Iinclude
CFLAGS += -Wall
SCPATH = $(SYSTEMC_HOME)
LIBS = -lm
DEBUG = -g

$(TARGET): clean $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -g -I$(SCPATH)/include -L. -L$(SCPATH)/lib-linux64 -Wl,-rpath $(SCPATH)/lib-linux64 $(SRC) $(LIBS) -o $@ -lsystemc

$(ODIR)/%.o: $(SDIR)/%.c
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@
	
run: clean $(TARGET)
	./out

debug:
	gdb out

clean:
	$(RM) $(TARGET)
	$(RM) *.vcd