CC = gcc
CFLAGS += -std=gnu11 -Wall -Werror -DNDEBUG
PFLAGS += -fPIC -fno-stack-protector
LDFLAGS = -x --shared
GLIB = `pkg-config --libs glib-2.0 gio-2.0`
G_CFLAGS = `pkg-config --cflags glib-2.0 gio-2.0`
LINUX_PAM_PATH = /lib/security/
DEAUTH_PATH = /etc/proxy_auth/
PAM_PATH = $(LINUX_PAM_PATH)/pam_proxy.so
BLUETOOTH_LIB = -lbluetooth
LDLIBS = -lc
SRC = src
LIB_DEP = $(SRC)/pam_bt_misc.h $(SRC)/pam_bt_pair.h $(SRC)/pam_bt_misc.h $(SRC)/pam_bt_trust.h
# LD_PRELOAD=${LIB_ASAN_PATH} $(PAM_PATH)
#gcc -print-file-name=libasan.so

all: pam_proxy.so pam_test deauth

pam: pam_proxy.so

pam_proxy.so: pam_proxy.o $(LIB_DEP)
	sudo ld $(LDFLAGS) -o $(LINUX_PAM_PATH)$@ $< $(BLUETOOTH_LIB) $(LDLIBS) $(GLIB)
	rm $<

pam_proxy.o: $(SRC)/pam_proxy.c
	$(CC) -c $< $(CFLAGS) $(PFLAGS) $(G_CFLAGS) -o $@ 

pam_test: $(SRC)/pam_test.c $(LIB_DEP)
	echo $@
	if [ -f $@ ];	then rm $@; fi
	$(CC) -o $@ $< $(CFLAGS) $(PFLAGS) -fsanitize=address $(BLUETOOTH_LIB) $(G_CFLAGS) $(GLIB)

deauth: $(SRC)/deauth.c $(LIB_DEP)
	$(CC) -o $(DEAUTH_PATH)$@ $< $(CFLAGS) -fsanitize=address $(BLUETOOTH_LIB) $(G_CFLAGS) $(GLIB) 

install: 
	perl install.pl

clean:
	rm -f *.o $(PAM_PATH) pam_test
