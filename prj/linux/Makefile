LIB_SDK_TARGET := libsmartconfig.a
SDK_TARGET := smart_config

###############################################################################
# Compiler

# CC := gcc
# AR := ar

# START
# inject compile tool-chain when cross complile sdk
# need chang system gcc link

#export HOST="armv7-hf"
#export ARCH="arm-linux-gnueabihf"
#export TARGET=${ARCH}

# 4.9.3
#export CROSS_ROOT=/home/vivek/toolchain/4.9.3#the root dir of cross compile tool chain
#export CROSS_COMPILE=${CROSS_ROOT}/bin/arm-cortexa9-linux-gnueabihf-

export CC=${CROSS_COMPILE}gcc
export AR=${CROSS_COMPILE}ar

# END

###############################################################################
# Path definitions
#编译过程目录
BLDDIR  := build
#编译源目录
SRCDIR  := 
#编译根目录
SRCPATH := ../..
#编译执行文件目录
EXPDIR  := bin
#编译库文件目录
LIBPDIR := lib

#编译排除目录
EXCDIR  := -path "./hal/esp" 
EXCDIR  += -o -path "./.git" 
EXCDIR  += -o -path "./prj"
EXCDIR  += -o -path "./si_prj"

#编译库提供的头文件
LIB_SDK_INCLUDE := $(SRCPATH)/utc_smart_config.h 
LIB_SDK_INCLUDE += $(SRCPATH)/hal/common/global.h
LIB_SDK_INCLUDE += $(SRCPATH)/sdk/utc_log.h 
LIB_SDK_INCLUDE += $(SRCPATH)/hal/linux/utc_hal_log.h
LIB_SDK_INCLUDE += $(SRCPATH)/sdk/utc_task_timer.h 
LIB_SDK_INCLUDE += $(SRCPATH)/hal/linux/utc_hal_timer.h

#边缘头文件
HDRDIR := $(shell cd $(SRCPATH)&& find $(SRCDIR) \( $(EXCDIR) \)  -prune -o -type d -print)
HDRDIR := $(subst ./, ,$(HDRDIR))

#编译源码目录
SOURCE_FILE_C := $(shell cd $(SRCPATH) &&  find $(SRCDIR) \( $(EXCDIR) \)  -prune -o -type f -name "*.c" -print)
SOURCE_FILE_C := $(subst ./, ,$(SOURCE_FILE_C))

LIB_OBJS := $(SOURCE_FILE_C:.c=.o)
LIB_OBJS := $(addprefix $(BLDDIR)/,$(LIB_OBJS))

HDRDIR := $(addprefix $(SRCPATH)/,$(HDRDIR))
CFLAGS := $(addprefix -I,$(HDRDIR))
LDFLAGS := -L$(BLDDIR) -lrt -lpthread

.PHONY: clean prepare cpinclude

all: prepare $(SDK_TARGET) $(LIB_SDK_TARGET)  cpinclude 

prepare:
#	@echo "-----HDRDIR: $(HDRDIR)"
#	@echo ""
#	@echo "-----SOURCE_FILE_C: $(SOURCE_FILE_C)"
#	@echo ""
#	@echo "-----CFLAGS: $(CFLAGS)"
#	@echo ""
#	@echo "-----LIBPDIR: $(LIBPDIR)"
#	@echo ""
#	@echo "-----LIB_SDK_INCLUDE: $(LIB_SDK_INCLUDE)"
#	@echo ""
	mkdir -p $(BLDDIR)

#编译执行文件
$(SDK_TARGET): $(LIB_OBJS)
	echo "o Linking $@ ..."
	@mkdir -p $(EXPDIR)
	$(CC) -o $(EXPDIR)/$@ $^ $(CFLAGS) $(LDFLAGS)

#编译库文件
$(LIB_SDK_TARGET): $(LIB_OBJS)
	@echo "o Archiving $@ ..."
	@mkdir -p $(LIBPDIR)
	@$(AR) -rcs $(LIBPDIR)/$@ $^

$(LIB_OBJS): $(BLDDIR)/%.o:$(SRCPATH)/%.c
	@echo "c Compiling $< ..."
	@mkdir -p $(shell dirname $@)
	@$(CC) -o $@ -c $< $(CFLAGS)

#拷贝库头文件
cpinclude: $(LIB_SDK_INCLUDE)
	@echo "cp Include $^ ..."
	@mkdir -p $(LIBPDIR)/include
	@cp -f $^ $(LIBPDIR)/include

clean:
	rm -rf $(EXPDIR)
	rm -rf $(BLDDIR)
	rm -rf $(LIBPDIR)
	