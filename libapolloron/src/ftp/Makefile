# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = ../../config
include $(CONFIG_FILE)

CXXFLAGS          += -I. -I../../include
OBJS              = ftplib.o

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: $(OBJS)

ftplib.o: ftplib.cc ftplib.h

clean:
	$(RM) $(OBJS)
