# inkf - yet another nkf with international language support
# Masashi Astro Tachibana

CONFIG_FILE = config
MAKE=`grep "MAKE" $(CONFIG_FILE) | sed -e "s/MAKE.*=\(.*\)/\1/"`

all:
	@./configure
	@$(MAKE) -f Makefile.all

install:
	@./configure
	@$(MAKE) -f Makefile.all $@

uninstall:
	@./configure
	@$(MAKE) -f Makefile.all $@

clean:
	@./configure
	@$(MAKE) -f Makefile.all $@

distclean:
	@./configure
	@$(MAKE) -f Makefile.all $@
