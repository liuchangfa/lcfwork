ifneq ($(KERNELRELEASE),)
obj-m:=hello.o
else 
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	rm -f *.o *.mod.c
distclean:
	rm -f *.ko *.o *.mod.c *.order *.symvers 
endif
