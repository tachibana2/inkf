%define	name		inkf
%define	ver		2.1.8
%define	rel		1
%define	prefix		/usr
%define mandir		%{prefix}/share/man
%define serial		2025053101


Summary: 	Yet Another Network Kanji Filter
Name: 		%{name}
Version: 	%{ver}
Release: 	%{rel}
Group: 		Applications/Text
License: 	LGPLv2
#Copyright: 	Masashi Astro Tachibana
Source0: 	http://sourceforge.jp/projects/inkf/downloads/xxxxx/%{name}-%{version}.tar.gz
URL:            http://inkf.sourceforge.jp/
#Serial: 	%{serial}
BuildRoot: 	/var/tmp/%{name}-%{version}-root
BuildRequires:	openssl openssl-devel
#PreReq:	openssl perl
#Provides: 	perl-INKF
#NoSource: 0


%description
inkf is a yet another kanji code converter among networks, hosts and
terminals.  It converts input encoding code to designated encoding code
such as ISO-2022-JP, SHIFT_JIS, EUC-JP, ISO-8859-*, BIG5, KOI8-[R,U],
CP1251, CP1252, CP1258, GB18030, EUC-KR, UTF-8, UTF-7, UTF-7-IMAP, UTF-16,
or UTF-32.


%changelog 
* Sat May 31 2025 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.8, library update, changed contact mail address

* Sun May 18 2025 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.7, fixed input-charset bugs of --html-to-plain option

* Sat May 3 2025 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.6, updated for Clang 11, fixed small bugs in strmidi

* Tue Jan 21 2025 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.5, changed output encoding of JIS, from JIS8 to JIS7

* Sun Oct 20 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.4, updated CP1252 (Latin1) auto detection

* Sat Oct 12 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.3, added BIG5 (CP950) auto detection support

* Sat Jun 15 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.2, added libinkf

* Sat Jun 1 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.1, added CP1258, ISO-8859-11(CP874) support

* Sat May 18 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.1.0, added EUCJP-WIN (CP51932) support

* Sat May 11 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.7, --html-to-plain and https handshake failure

* Fri Apr 12 2019 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.6, optimized covertion of japanese hiragana/katakana from/to UTF-8

* Sat Oct 20 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.5, added "ftp://" support

* Sat Sep 8 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.4, do not put extra return-code when encoding MIME if -N is not set

* Sat Aug 18 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.3, added -n, -M[bq], --re-match option, update options --sort-csv, --sort-csv-r

* Wed Aug 8 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.2, added --sort-csv, --sort-csv-r, --format-json, --minify-json, --md5 and --sha1 option

* Mon Aug 6 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.1, fixed auto detection of UTF-32 big endian

* Sat Aug 4 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 2.0.0, fixed many problems, UTF-16 and UTF-32 full support

* Sat Jul 28 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.2.4, added UTF-16, UTF-32 as convertable charset

* Sat Jul 7 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.2.3, fixed charset autoditection

* Mon Jul 2 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.2.2, fixed ISO-8859-* convertion

* Sat Jun 30 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.2.1, performance tuning

* Sat Jun 23 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.2.0, GB18030 optimisation, fixed GB18030 UCS2 range, and UTF-7

* Sat Jun 16 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.9, GB18030 optimisation, fixed around CP932

* Sun Jun 10 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.8, added 4-byte GB18030 to/from UTF-8 convertion support

* Sat May 26 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.7, fixed memory allocation problem of String

* Sat May 26 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.6, fixed compiling error on FreeBSD 11

* Sat May 19 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.5, fixed compiling error on Fedora 28

* Tue Apr 10 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.4, fixed convertion from ISO-8859 to UTF-8

* Sat Apr 7 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.3, internal library update

* Mon Jan 1 2018 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.2, internal library update, fixed compile warnings on CentOS7

* Sat Aug 15 2015 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.1, fixed quoted-printable decoding

* Sat Feb 11 2012 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.1.0, fixed MIME decoding

* Sun Jan 29 2012 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.9, fixed MIME decoding

* Fri May 20 2011 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.8, fixed convertion with "-Sj" option

* Wed Sep 23 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.7, fixed internal

* Wed Jul 29 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.6, fixed internal

* Tue Jul 21 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.5, fixed compile error

* Sat Jul 18 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.4, fixed timeout of Socket

* Fri Jul 17 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.3, added HTMLClient class

* Wed Jul 1 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.2, fixed a bug of converting charset from UTF-8 to another

* Mon Jun 29 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.1, fixed a bug of converting text from HTML to plain

* Fri May 8 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 1.0.0, fixed a bug of charset convertion to UTF-8

* Fri Apr 24 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.9, fixed a bug of connect method of Socket class

* Sat Mar 28 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.8, fixed for warnings of C++ compiler

* Wed Feb 25 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.7, fixed bugs of Socket

* Sat Feb 21 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.6, fixed http connection bug

* Fri Feb 13 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.5, added --html-to-plain option, hankaku/zenkaku options

* Fri Feb 6 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.4, added CP1252, -Z option, fixed charset ditection of http(s)

* Mon Feb 2 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.3, fixed bugs of reading charset options

* Sun Feb 1 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.2, fixed ISO-8859-*, added KOI8-R, KOI8-U, CP1251

* Wed Jan 28 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.1, fixed UTF-7 / UTF-7-IMAP

* Mon Jan 26 2009 Masashi Astro Tachibana <astro@apolloron.org>
- Updated to 0.9.0
#--------------------------------------------------------------------
%prep
rm -fr $RPM_BUILD_ROOT


%setup -n inkf-%{version}
#mkdir INKF
#sh %{SOURCE0}


%build
./configure --with-openssl
make
#make PERL=perl perl
#perl -p -i -e "s|^PREFIX *=.*\$|PREFIX=${RPM_BUILD_ROOT}%{prefix}|" INKF/Makefile


%install
rm -fr ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}%{prefix}/bin
install -s -m 755 inkf ${RPM_BUILD_ROOT}%{prefix}/bin
mkdir -p ${RPM_BUILD_ROOT}%{mandir}/man1
gzip -nf -c inkf.1 > ${RPM_BUILD_ROOT}%{mandir}/man1/inkf.1.gz
#mkdir -p ${RPM_BUILD_ROOT}%{mandir}/ja/man1
#./inkf -We inkf.1j | gzip -nf > ${RPM_BUILD_ROOT}%{mandir}/ja/man1/inkf.1.gz
#( cd INKF
#  make install
#)
mkdir -p ${RPM_BUILD_ROOT}%{_libdir}
install -m 644 libinkf.a ${RPM_BUILD_ROOT}%{_libdir}
mkdir -p ${RPM_BUILD_ROOT}%{_includedir}
install -m 644 inkf.h ${RPM_BUILD_ROOT}%{_includedir}


%clean
rm -rf ${RPM_BUILD_ROOT}


%files
%defattr(-,root,root)
%{prefix}/bin/inkf
%{_libdir}/*
%{_includedir}/*
#/usr/lib/perl5/man/man3/*
#/usr/lib/perl5/site_perl/*/*/INKF.pm
#/usr/lib/perl5/site_perl/*/*/auto/*
/usr/share/man/man1/*
#/usr/share/man/ja/man1/*
