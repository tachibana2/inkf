# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = ../../config
include $(CONFIG_FILE)

CXXFLAGS            += -I.
OBJS              = table_iso8859_unicode.o table_europe_unicode.o \
                    table_sjis_unicode.o table_euckr_unicode.o \
                    table_gbk_unicode.o table_big5_unicode.o \
                    table_unicode_sjis.o table_unicode_euckr.o \
                    table_unicode_gbk.o table_unicode_big5.o \
                    table_gb18030_utf8.o table_utf8_gb18030.o \
                    table_charwidth.o

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: $(OBJS)

table_iso8859_unicode.o: table_iso8859_unicode.cc table_iso8859_unicode.h
table_europe_unicode.o: table_europe_unicode.cc table_europe_unicode.h
table_sjis_unicode.o: table_sjis_unicode.cc table_sjis_unicode.h
table_euckr_unicode.o: table_euckr_unicode.cc table_euckr_unicode.h
table_gbk_unicode.o: table_gbk_unicode.cc table_gbk_unicode.h
table_big5_unicode.o: table_big5_unicode.cc table_big5_unicode.h
table_unicode_sjis.o: table_unicode_sjis.cc table_unicode_sjis.h
table_unicode_euckr.o: table_unicode_euckr.cc table_unicode_euckr.h
table_unicode_gbk.o: table_unicode_gbk.cc table_unicode_gbk.h
table_unicode_big5.o: table_unicode_big5.cc table_unicode_big5.h
table_gb18030_utf8.o: table_gb18030_utf8.cc table_gb18030_utf8.h
table_utf8_gb18030.o: table_utf8_gb18030.cc table_utf8_gb18030.h
table_charwidth.o: table_charwidth.cc table_charwidth.h

clean:
	$(RM) $(OBJS)
