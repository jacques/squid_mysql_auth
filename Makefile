# Squid MySQL Authentication
#
# (C) 2004-2006 Jacques Marneweck <jacques@php.net>
#
# $Id$

build:
	gcc -O3 -o mysql_auth squid_mysql_auth.c -I/usr/local/include/mysql -L/usr/local/lib/mysql -lmysqlclient

install:
	install -S -o root -g squid -m 0550 mysql_auth /usr/local/libexec/squid/

clean:
	-rm -f *.core mysql_auth
