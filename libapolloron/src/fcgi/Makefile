# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = ../../config
include $(CONFIG_FILE)

CXXFLAGS          += -fno-strict-aliasing -I. -I../../include
OBJS              = fcgi_stdio.o fcgiapp.o os_unix.o

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: $(OBJS)

fcgi_stdio.o: fcgi_stdio.cc fastcgi.h fcgi_stdio.h fcgiapp.h fcgimisc.h fcgios.h
fcgiapp.o: fcgiapp.cc fastcgi.h fcgi_stdio.h fcgiapp.h fcgimisc.h fcgios.h
os_unix.o: os_unix.cc fastcgi.h fcgi_stdio.h fcgiapp.h fcgimisc.h fcgios.h

clean:
	$(RM) $(OBJS)
