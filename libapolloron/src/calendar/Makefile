# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = ../../config
include $(CONFIG_FILE)

CXXFLAGS          += -I.
OBJS              = msg_en.o msg_ja.o msg_ko.o msg_zh_cn.o \
                    msg_de.o msg_es.o msg_fr.o

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: $(OBJS)

msg_en.o: msg_en.cc msg.h
msg_ja.o: msg_ja.cc msg.h
msg_ko.o: msg_ko.cc msg.h
msg_zh_cn.o: msg_zh_cn.cc msg.h
msg_de.o: msg_de.cc msg.h
msg_es.o: msg_es.cc msg.h
msg_fr.o: msg_fr.cc msg.h

clean:
	$(RM) $(OBJS)
