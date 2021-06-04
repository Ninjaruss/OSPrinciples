KERNELDIR=/lib/modules/`uname -r`/build


#Change the names here to your file name
MODULES = char_driver.ko 
obj-m += char_driver.o 

all:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean

install:	
	make -C $(KERNELDIR) M=$(PWD) modules_install

quickInstall:
	cp $(MODULES) /lib/modules/`uname -r`/extra