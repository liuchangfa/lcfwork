ifneq ($(KERNELRELEASE),)
obj-m:=asyncmonitor.o
else 
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.o *.mod.c
distclean:
	rm -f *.ko *.o *.mod.c *.order *.symvers 
endif
