# Multi-purpose C++ class library
# libapolloron
# Masashi Astro Tachibana, Apolloron Project

CONFIG_FILE = config
MAKE=`grep "MAKE" $(CONFIG_FILE) | sed -e "s/MAKE.*=\(.*\)/\1/"`
MAKE2=`grep "MAKE" ../$(CONFIG_FILE) | sed -e "s/MAKE.*=\(.*\)/\1/"`
MAKE3=`grep "MAKE" ../../$(CONFIG_FILE) | sed -e "s/MAKE.*=\(.*\)/\1/"`
RM=`grep "RM" $(CONFIG_FILE) | sed -e "s/RM.*=\(.*\)/\1/"`

all:
	./configure
	cd src && $(MAKE2)
	cd src/jsoncpp && $(MAKE3) test
	cd test && $(MAKE2)

clean:
	./configure
	cd test && $(MAKE2) $@
	cd src && $(MAKE2) $@

distclean:
	./configure
	$(MAKE) clean
	$(RM) $(CONFIG_FILE)
