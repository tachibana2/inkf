# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = ../config
include $(CONFIG_FILE)

CXXFLAGS          += -I. -I../include
LDFLAGS           +=
LIBS              = ../lib/libapolloron.a
TEST_OBJS         = test.o
TEST              = test

ifeq ($(OS),Windows_NT)
  CXXFLAGS        +=
  #LDFLAGS        += -mno-cygwin
  TEST            = test.exe
endif

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: $(TEST)

$(TEST): $(LIBS) $(TEST_OBJS)
	$(CXXLD) -o $@ $(TEST_OBJS) $(LIBS) $(LDFLAGS)
	$(STRIP) $@
	@./$@ || ($(RM) $@ test9.txt $(TEST_OBJS) $(LIBS); exit -1)

test.o: test.cc

clean:
	$(RM) $(TEST) $(TEST_OBJS) test9.txt core *.stackdump
