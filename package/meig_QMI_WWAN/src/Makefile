obj-m += qmi_wwan_m.o

PWD := $(shell pwd)
OUTPUTDIR=/lib/modules/`uname -r`/kernel/drivers/net/usb/

ifeq ($(ARCH),)
ARCH := $(shell uname -m)
endif
ifeq ($(CROSS_COMPILE),)
CROSS_COMPILE :=
endif
ifeq ($(KDIR),)
KDIR := /lib/modules/$(shell uname -r)/build
ifeq ($(ARCH),i686)
ifeq ($(wildcard $KDIR/arch/$ARCH),)
ARCH=i386
endif
endif
endif

default:
	$(MAKE) ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} -C $(KDIR) M=$(PWD) modules

install: default
	cp $(PWD)/qmi_wwan_m.ko /lib/modules/$(shell uname -r)/kernel/drivers/net/usb/
	depmod

clean:
	rm -rf *~ .tmp_versions modules.order Module.symvers
	find . -type f -name "*~" -o -name "*.o" -o -name "*.ko" -o -name "*.cmd" -o -name "*.mod.c" |  xargs rm -rf
