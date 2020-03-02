CC = gcc
CFLAGS += -std=gnu11 -Wall -Werror -DNDEBUG -fPIC -fno-stack-protector
LDFLAGS = -x --shared
GLIB = `pkg-config --cflags glib-2.0 gio-2.0 --libs glib-2.0 gio-2.0`
LINUX_PAM_PATH = /lib/security
PAM_PATH = $(LINUX_PAM_PATH)/pam_proxy.so
BLUETOOTH_LIB = -lbluetooth
LDLIBS = -lc
# LD_PRELOAD=${LIB_ASAN_PATH} $(PAM_PATH)
#gcc -print-file-name=libasan.so

all: pam_proxy.so pam_test

pam_proxy.so: pam_proxy.o
	sudo ld $(LDFLAGS) -o $(LINUX_PAM_PATH)$@ $< $(BLUETOOTH_LIB) $(LDLIBS)
	rm $<

pam_proxy.o: src/pam_proxy.c
	$(CC) -c $< $(CFLAGS) -o $@ 

pam_test: src/pam_test.c
	$(CC) -o $@ $< $(CFLAGS) -fsanitize=address $(BLUETOOTH_LIB) $(GLIB)

clean:
	rm -f *.o *.so
