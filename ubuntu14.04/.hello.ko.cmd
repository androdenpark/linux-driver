cmd_/home/zyy/rasperberrypi/hello.ko := ld -r -m elf_i386 -T /usr/src/linux-headers-3.13.0-32-generic/scripts/module-common.lds --build-id  -o /home/zyy/rasperberrypi/hello.ko /home/zyy/rasperberrypi/hello.o /home/zyy/rasperberrypi/hello.mod.o