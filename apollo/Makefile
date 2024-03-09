CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCDIR = common/math
SRCS := $(wildcard $(SRCDIR)/*.cc)
OBJS = $(SRCS:.cc=.o)

all: $(OBJS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -I. -c -o $@ $<

clean:
	rm -f $(OBJS)
