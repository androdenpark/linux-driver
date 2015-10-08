obj-m := hello.o
KERNELDIR := /lib/modules/3.13.0-32-generic/build
#KERNELDIR := /usr/src/linux-source-3.13.0/linux-source-3.13.0

PWD := $(shell pwd);

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm -rf *.0 *~ core.depend .*.cmd *.ko *.mod.c .tmp_versions $(TARGET)

