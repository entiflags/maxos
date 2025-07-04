DEFAULT_HOST!=../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)
 
CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include
 
CFLAGS:=$(CFLAGS) -D__is_kernel -Iinclude -fstack-protector -ffreestanding -Wall -Wextra -Wno-unused-function
CPPFLAGS:=$(CPPFLAGS) -fno-rtti -fno-exceptions
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lbank -lgcc
 
ARCHDIR=arch/$(HOSTARCH)
 
include $(ARCHDIR)/make.config
 
CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

BUILDDIR=$(abspath build)
 
KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
kernel/kernel.o \
kernel/Keyboard.o \
kernel/atexit.o \
kernel/kmalloc.o \
kernel/PIC.o \
kernel/PIT.o \
kernel/RTC.o \
kernel/Serial.o \
kernel/SSP.o \
 
OBJS=\
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \
 
LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \
 
.PHONY: all always clean install install-headers install-kernel
.SUFFIXES: .o .cpp .S
 
all: pineappleOs.kernel
 
pineappleOs.kernel: always $(OBJS) $(ARCHDIR)/linker.ld
	cd $(BUILDDIR) && $(CXX) -T ../$(ARCHDIR)/linker.ld -o pineappleOs.kernel $(CFLAGS) $(CPPFLAGS) $(LINK_LIST)
	cd $(BUILDDIR) && grub-file --is-x86-multiboot pineappleOs.kernel
 
$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $(BUILDDIR)/$@

.cpp.o:
	$(CXX) -MD -c $< -o $(BUILDDIR)/$@ $(CFLAGS) $(CPPFLAGS)
 
.S.o:
	$(CC) -MD -c $< -o $(BUILDDIR)/$@ $(CFLAGS)
 
always:
	mkdir -p $(BUILDDIR)/arch/i386
	mkdir -p $(BUILDDIR)/kernel

clean:
	rm -rf $(BUILDDIR)

install: install-headers install-kernel
 
install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.
 
install-kernel: pineappleOs.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp $(BUILDDIR)/pineappleOs.kernel $(DESTDIR)$(BOOTDIR)
 
-include $(OBJS:.o=.d)