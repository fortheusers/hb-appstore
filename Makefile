ifeq ($(strip $(LIBTRANSISTOR_HOME)),)
$(error "Please set LIBTRANSISTOR_HOME in your environment. export LIBTRANSISTOR_HOME=<path to libtransistor>")
endif

PROGRAM := appstore
OBJ := main.o Console.o Menu.o libs/get/src/Package.o libs/get/src/Utils.o Input.o libs/get/src/Repo.o libs/get/src/Get.o libs/get/src/Zip.o stub.o

include $(LIBTRANSISTOR_HOME)/libtransistor.mk

RAPIDJSON := -I ./libs/get/src/libs/rapidjson/include
MINIZIP_O :=  libs/get/src/libs/zlib/contrib/minizip/inflate.o \
			libs/get/src/libs/zlib/contrib/minizip/inftrees.o \
			libs/get/src/libs/zlib/contrib/minizip/inffast.o \
			libs/get/src/libs/zlib/contrib/minizip/crc32.o \
			libs/get/src/libs/zlib/contrib/minizip/deflate.o \
			libs/get/src/libs/zlib/contrib/minizip/trees.o \
			libs/get/src/libs/zlib/contrib/minizip/adler32.o \
			libs/get/src/libs/zlib/contrib/minizip/zutil.o \
			libs/get/src/libs/zlib/contrib/minizip/zip.o \
			libs/get/src/libs/zlib/contrib/minizip/ioapi.o \
			libs/get/src/libs/zlib/contrib/minizip/unzip.o
OBJ := $(OBJ) $(MINIZIP_O)

all: $(PROGRAM).nro

$(PROGRAM).nro.so: ${OBJ} $(LIBTRANSITOR_NRO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
	$(LD) $(LD_FLAGS) -o $@ ${OBJ} $(LIBTRANSISTOR_NRO_LDFLAGS)

clean:
	rm -rf *.o *.nso *.nro *.so
