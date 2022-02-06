CC = gcc
CFLAGS += -g -std=gnu11 -Wall -Werror 
PFLAGS += -fPIC -fno-stack-protector
LDFLAGS = -x --shared
GLIB = `pkg-config --libs glib-2.0 gio-2.0`
G_CFLAGS = `pkg-config --cflags glib-2.0 gio-2.0`
OS:=$(shell grep -E "^ID" /etc/os-release | cut -d = -f 2)
DEAUTH_PATH = /etc/proxy_auth

ifeq ($(OS), fedora)
	LINUX_PAM_PATH = /lib64/security
else
	LINUX_PAM_PATH = /lib/security
endif
#override with user's preferred installation path
ifneq ($(ALT_PAM_PATH), )
	LINUX_PAM_PATH = $(ALT_PAM_PATH)
endif
ifneq ($(ALT_DEAUTH_PATH),)
	DEAUTH_PATH = $(ALT_DEAUTH_PATH)
endif
#end

PAM_PATH = $(LINUX_PAM_PATH)/pam_proxy.so
BLUETOOTH_LIB = -lbluetooth
LDLIBS = -lpam -lc
SRC = src
LIB_OBJ = pam_misc.o pam_bt_misc.o pam_bt_pair.o pam_bt_trust.o pam_post_auth.o pam_sec.o
#gcc -print-file-name=libasan.so

all: pam_proxy.so pam_test deauth
pam_misc.o: $(SRC)/pam_misc.h
pam_bt_misc.o: $(SRC)/pam_bt_misc.h
pam_bt_pair.o: $(SRC)/pam_bt_pair.h
pam_sec.o: $(SRC)/pam_sec.h
pam_bt_trust.o: $(SRC)/pam_bt_trust.h $(SRC)/pam_misc.h $(SRC)/pam_bt_misc.h $(SRC)/pam_bt_pair.h
pam_post_auth.o: $(SRC)/pam_post_auth.h $(SRC)/pam_bt_misc.h
proxy_dbus.o: $(SRC)/proxy_dbus.h $(SRC)/pam_bt_trust.h $(SRC)/pam_misc.h $(SRC)/pam_bt_misc.h $(SRC)/pam_bt_pair.h


pam: pam_proxy.so

pam_proxy.so: pam_proxy.o $(LIB_OBJ)
	sudo ld $(LDFLAGS) -o $(LINUX_PAM_PATH)/$@ $^ $(BLUETOOTH_LIB) $(LDLIBS) $(GLIB)
	rm $<

pam_proxy.o: $(SRC)/pam_proxy.c
	$(CC) -c $< $(CFLAGS) $(PFLAGS) $(G_CFLAGS) -o $@

pam_test: pam_test.o $(LIB_OBJ)
	if [ -f $@ ];	then rm -f $@; fi
	$(CC) $^ -o $@ $(CFLAGS) $(PFLAGS) $(BLUETOOTH_LIB) -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer $(G_CFLAGS) $(GLIB)

deauth: deauth.o $(LIB_OBJ) proxy_dbus.o
	$(CC) $^ -o $(DEAUTH_PATH)/$@ $(CFLAGS) $(BLUETOOTH_LIB) -fsanitize=address $(G_CFLAGS) $(GLIB) 

proxy_dbus.o: $(SRC)/proxy_dbus.c
	$(CC) $(CFLAGS) -c $< -o $@ $(BLUETOOTH_LIB) $(G_CFLAGS) $(GLIB)

%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ $(G_CFLAGS) $(GLIB)

install:
	perl install.pl

doc:
	doxygen Doxyfile
	find ./src -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" > cscope.files
	cscope -b -q -k

clean:
	rm -f *.o $(PAM_PATH) pam_test $(DEAUTH_PATH)/$@
