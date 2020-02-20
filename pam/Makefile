CC = gcc
CFLAGS += -std=gnu11 -Wall -Werror -DNDEBUG -fPIC -fno-stack-protector
LDFLAGS = -x --shared
LINUX_PAM_PATH = /lib/security/
PAM_PATH = $(LINUX_PAM_PATH)/pam_proxy.so
BLUETOOTH_LIB = -lbluetooth
LDLIBS = -lc

all: pam_proxy.so

pam_proxy.so: pam_proxy.o
	sudo ld $(LDFLAGS) -o $(LINUX_PAM_PATH)$@ $< $(BLUETOOTH_LIB) $(LDLIBS)
	rm $<

pam_proxy.o: src/pam_proxy.c
	$(CC) -c $< $(CFLAGS) -o $@


clean:
	rm -f *.o *.so
