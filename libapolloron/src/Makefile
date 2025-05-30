# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = ../config
include $(CONFIG_FILE)

INCLUDES +=  -I. -I../include -I./fcgi

ifeq ($(ENABLE_REGEX),1)
  REGEX_SRC       = regcomp.cc regexec.cc reggnu.cc
  REGEX_HEAD      = regex.h regint.h
  REGEX_OBJ       = regexec.o reggnu.o
else
  REGEX_SRC       =
  REGEX_HEAD      =
  REGEX_OBJ       =
endif

ifeq ($(ENABLE_MD5),1)
  MD5_SRC         = md5.cc
  MD5_HEAD        = md5.h
  MD5_OBJ         = md5.o
else
  MD5_CC          =
  MD5_H           =
  MD5_O           =
endif

ifeq ($(ENABLE_SHA1),1)
  SHA1_SRC         = sha1.cc
  SHA1_HEAD        = sha1.h
  SHA1_OBJ         = sha1.o
else
  SHA1_CC          =
  SHA1_H           =
  SHA1_O           =
endif

FCGI_SRC          = fcgi/fcgi_stdio.cc fcgi/fcgiapp.cc fcgi/os_unix.cc
FCGI_HEAD         = fcgi/fastcgi.h fcgi/fcgi_stdio.h fcgi/fcgiapp.h \
                    fcgi/fcgimisc.h fcgi/fcgios.h
FCGI_OBJ          = fcgi/fcgi_stdio.o fcgi/fcgiapp.o fcgi/os_unix.o

JSONCPP_SRC       = jsoncpp/json_reader.cc jsoncpp/json_value.cc \
                    jsoncpp/json_writer.cc
JSONCPP_OBJ       = jsoncpp/json_reader.o jsoncpp/json_value.o \
                    jsoncpp/json_writer.o

FTP_SRC           = ftp/ftplib.cc
FTP_HEAD          = ftp/ftplib.h
FTP_OBJ           = ftp/ftplib.o

LIBAPOLLORON_SRC  = systeminfo.cc \
                    String.cc Keys.cc List.cc Sheet.cc DateTime.cc MIMEHeader.cc \
                    Socket.cc WebSocket.cc CGI.cc FCGI.cc \
                    HTTPClient.cc FTPStream.cc POP3Stream.cc \
                    IMAPStream.cc IMAPMailBoxList.cc IMAPSearch.cc IMAPMail.cc \
                    SMTPStream.cc \
                    utils.cc ansi.cc charset.cc strmidi.cc \
                    $(REGEX_SRC) $(MD5_SRC) $(SHA1_SRC)
CHARSET_HEAD      = charset/table_iso8859_unicode.h charset/table_europe_unicode.h \
                    charset/table_sjis_unicode.h charset/table_euckr_unicode.h \
                    charset/table_gbk_unicode.h charset/table_big5_unicode.h \
                    charset/table_unicode_sjis.h charset/table_unicode_euckr.h \
                    charset/table_unicode_gbk.h charset/table_unicode_big5.h \
                    charset/table_gb18030_utf8.h charset/table_utf8_gb18030.h \
                    charset/table_charwidth.h
CALENDAR_MSG_HEAD = calendar/msg.h
LIBAPOLLORON_HEAD = ../include/apolloron.h charset.h $(CHARSET_HEAD) \
                    strmidi.h $(REGEX_HEAD) $(MD5_HEAD) $(SHA1_HEAD)
CHARSET_OBJ       = charset/table_iso8859_unicode.o charset/table_europe_unicode.o \
                    charset/table_sjis_unicode.o charset/table_euckr_unicode.o \
                    charset/table_gbk_unicode.o charset/table_big5_unicode.o \
                    charset/table_unicode_sjis.o charset/table_unicode_euckr.o \
                    charset/table_unicode_gbk.o charset/table_unicode_big5.o \
                    charset/table_gb18030_utf8.o charset/table_utf8_gb18030.o \
                    charset/table_charwidth.o
CALENDAR_MSG_OBJ  = calendar/msg_en.o calendar/msg_ja.o \
                    calendar/msg_ko.o calendar/msg_zh_cn.o \
                    calendar/msg_de.o calendar/msg_es.o calendar/msg_fr.o
LIBAPOLLORON_OBJ  = systeminfo.o \
                    String.o Keys.o List.o Sheet.o DateTime.o MIMEHeader.o \
                    Socket.o WebSocket.o CGI.o FCGI.o \
                    HTTPClient.o FTPStream.o ftp/ftplib.o POP3Stream.o \
                    IMAPStream.o IMAPMailBoxList.o IMAPSearch.o IMAPMail.o \
                    SMTPStream.o \
                    utils.o ansi.o charset.o $(CHARSET_OBJ) \
                    $(CALENDAR_MSG_OBJ) strmidi.o \
                    $(REGEX_OBJ) $(MD5_OBJ) $(SHA1_OBJ) $(FCGI_OBJ) $(JSONCPP_OBJ)
LIBAPOLLORON_LIB  = ../lib/libapolloron.a

.cc.o:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $<

all: $(LIBAPOLLORON_LIB) $(LIBAPOLLORON_OBJ)

$(LIBAPOLLORON_LIB): $(LIBAPOLLORON_OBJ) $(LIBAPOLLORON_HEAD)
	@touch $@ && $(RM) $@
	$(AR) rv $@ $(LIBAPOLLORON_OBJ)
	$(RANLIB) $@

systeminfo.o: systeminfo.cc systeminfo.h
systeminfo.h: systeminfo.sh
	./systeminfo.sh "$(CXX)"
String.o:     String.cc     $(LIBAPOLLORON_HEAD)
Keys.o:       Keys.cc       $(LIBAPOLLORON_HEAD)
List.o:       List.cc       $(LIBAPOLLORON_HEAD)
Sheet.o:      Sheet.cc      $(LIBAPOLLORON_HEAD)
DateTime.o:   DateTime.cc   $(LIBAPOLLORON_HEAD)
MIMEHeader.o: MIMEHeader.cc $(LIBAPOLLORON_HEAD)
Socket.o:     Socket.cc     $(LIBAPOLLORON_HEAD)
WebSocket.o:  WebSocket.cc  $(LIBAPOLLORON_HEAD)
CGI.o:        CGI.cc        $(LIBAPOLLORON_HEAD)
FCGI.o:       FCGI.cc       $(LIBAPOLLORON_HEAD)
HTTPClient.o: HTTPClient.cc $(LIBAPOLLORON_HEAD)
FTPStream.o:  FTPStream.cc  $(LIBAPOLLORON_HEAD) $(FTP_OBJ)
POP3Stream.o: POP3Stream.cc $(LIBAPOLLORON_HEAD)
IMAPStream.o: IMAPStream.cc $(LIBAPOLLORON_HEAD)
IMAPMailBoxList.o: IMAPMailBoxList.cc $(LIBAPOLLORON_HEAD)
IMAPSearch.o: IMAPSearch.cc $(LIBAPOLLORON_HEAD)
IMAPMail.o:   IMAPMail.cc   $(LIBAPOLLORON_HEAD)
SMTPStream.o: SMTPStream.cc $(LIBAPOLLORON_HEAD)
utils.o:      utils.cc      $(LIBAPOLLORON_HEAD)
ansi.o:       ansi.cc       $(LIBAPOLLORON_HEAD)
charset.o:    charset.cc    $(LIBAPOLLORON_HEAD)
	$(CXX) -c $(INCLUDES) $(CXXFLAGS) -D__ICONV_CONST=1 $< || $(CXX) -c $(INCLUDES) $(CXXFLAGS) -D__ICONV_CONST=0 $<
$(CHARSET_OBJ): $(CHARSET_HEAD)
	cd charset && $(MAKE)
$(CALENDAR_MSG_OBJ): $(CALENDAR_MSG_HEAD)
	cd calendar && $(MAKE)
strmidi.o:    strmidi.cc    $(LIBAPOLLORON_HEAD)
regexec.o:    regexec.cc    $(REGEX_HEAD)
reggnu.o:     reggnu.cc     $(REGEX_HEAD) regcomp.cc
md5.o:        md5.cc        $(MD5_HEAD)
sha1.o:       sha1.cc       $(SHA1_HEAD)
$(FCGI_OBJ): $(FCGI_SRC) $(FCGI_HEAD)
	cd fcgi && $(MAKE)
$(JSONCPP_OBJ): $(JSONCPP_SRC)
	cd jsoncpp && $(MAKE)
$(FTP_OBJ): $(FTP_SRC) $(FTP_HEAD)
	cd ftp && $(MAKE)

clean:
	cd fcgi && $(MAKE) clean
	cd jsoncpp && $(MAKE) clean
	cd ftp && $(MAKE) clean
	cd charset && $(MAKE) clean
	cd calendar && $(MAKE) clean
	$(RM) *.o systeminfo.h $(LIBAPOLLORON_LIB) .DS_Store ._*
