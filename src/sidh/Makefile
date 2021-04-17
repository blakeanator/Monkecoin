####  Makefile for compilation on Unix-like operative systems  ####

OPT=-O3     # Optimization option by default

CC=gcc

ifeq "$(CC)" "gcc"
    COMPILER=gcc
else ifeq "$(CC)" "clang"
    COMPILER=clang
endif

ARCHITECTURE=_AMD64_
USE_OPT_LEVEL=_FAST_
ifeq "$(ARCH)" "x64"
    ARCHITECTURE=_AMD64_
    USE_OPT_LEVEL=_FAST_
else ifeq "$(ARCH)" "x86"
    ARCHITECTURE=_X86_
    USE_OPT_LEVEL=_GENERIC_
else ifeq "$(ARCH)" "s390x"
    ARCHITECTURE=_S390X_
    USE_OPT_LEVEL=_GENERIC_
else ifeq "$(ARCH)" "ARM"
    ARCHITECTURE=_ARM_
    USE_OPT_LEVEL=_GENERIC_
    ARM_TARGET=YES
else ifeq "$(ARCH)" "ARM64"
    ARCHITECTURE=_ARM64_
    USE_OPT_LEVEL=_FAST_
    ARM_TARGET=YES
endif

ifeq "$(OPT_LEVEL)" "GENERIC"
    USE_OPT_LEVEL=_GENERIC_
endif

ifeq "$(ARM_TARGET)" "YES"
    ARM_SETTING=-lrt
endif

ifeq "$(ARCHITECTURE)" "_AMD64_"
    ifeq "$(USE_OPT_LEVEL)" "_FAST_"
        MULX=-D _MULX_
        ifeq "$(USE_MULX)" "FALSE"
            MULX=
        else
            ADX=-D _ADX_
            ifeq "$(USE_ADX)" "FALSE"
                ADX=
            endif
        endif
    endif
endif

AR=ar rcs
RANLIB=ranlib

ADDITIONAL_SETTINGS=-march=native
ifeq "$(CC)" "clang"
ifeq "$(ARM_TARGET)" "YES"
    ADDITIONAL_SETTINGS=
endif
endif
ifeq "$(ARCHITECTURE)" "_S390X_"
	ADDITIONAL_SETTINGS=-march=z10
endif

CFLAGS=$(OPT) -std=gnu11 $(ADDITIONAL_SETTINGS) -D $(ARCHITECTURE) -D __NIX__ -D $(USE_OPT_LEVEL) $(MULX) $(ADX)

ifeq "$(USE_OPT_LEVEL)" "_GENERIC_"
    EXTRA_OBJECTS_434=src/P434/P434_fp_generic.o
    EXTRA_OBJECTS_503=src/P503/P503_fp_generic.o
    EXTRA_OBJECTS_610=src/P610/P610_fp_generic.o
    EXTRA_OBJECTS_751=src/P751/P751_fp_generic.o
else ifeq "$(USE_OPT_LEVEL)" "_FAST_"
ifeq "$(ARCHITECTURE)" "_AMD64_"
    EXTRA_OBJECTS_434=src/P434/P434_fp_x64.o src/P434/P434_fp_x64_asm.o
    EXTRA_OBJECTS_503=src/P503/P503_fp_x64.o src/P503/P503_fp_x64_asm.o
    EXTRA_OBJECTS_610=src/P610/P610_fp_x64.o src/P610/P610_fp_x64_asm.o
    EXTRA_OBJECTS_751=src/P751/P751_fp_x64.o src/P751/P751_fp_x64_asm.o
else ifeq "$(ARCHITECTURE)" "_ARM64_"
    EXTRA_OBJECTS_434=src/P434/P434_fp_arm64.o src/P434/P434_fp_arm64_asm.o
    EXTRA_OBJECTS_503=src/P503/P503_fp_arm64.o src/P503/P503_fp_arm64_asm.o
    EXTRA_OBJECTS_610=src/P610/P610_fp_arm64.o src/P610/P610_fp_arm64_asm.o
    EXTRA_OBJECTS_751=src/P751/P751_fp_arm64.o src/P751/P751_fp_arm64_asm.o
endif
endif

RANDOM_OBJECT=src/random/random.o
SHA3_OBJECT=src/sha3/fips202.o

OBJECTS_434=src/P434/P434.o $(EXTRA_OBJECTS_434) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_503=src/P503/P503.o $(EXTRA_OBJECTS_503) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_610=src/P610/P610.o $(EXTRA_OBJECTS_610) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_751=src/P751/P751.o $(EXTRA_OBJECTS_751) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_434_COMP=src/P434/P434_compressed.o $(EXTRA_OBJECTS_434) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_503_COMP=src/P503/P503_compressed.o $(EXTRA_OBJECTS_503) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_610_COMP=src/P610/P610_compressed.o $(EXTRA_OBJECTS_610) $(RANDOM_OBJECT) $(SHA3_OBJECT)
OBJECTS_751_COMP=src/P751/P751_compressed.o $(EXTRA_OBJECTS_751) $(RANDOM_OBJECT) $(SHA3_OBJECT)

ALL_OBJECTS=src/P434/P434.o $(EXTRA_OBJECTS_434) \
src/P503/P503.o $(EXTRA_OBJECTS_503) \
src/P610/P610.o $(EXTRA_OBJECTS_610) \
src/P751/P751.o $(EXTRA_OBJECTS_751) \
src/P434/P434_compressed.o $(EXTRA_OBJECTS_434) \
src/P503/P503_compressed.o $(EXTRA_OBJECTS_503) \
src/P610/P610_compressed.o $(EXTRA_OBJECTS_610) \
src/P751/P751_compressed.o $(EXTRA_OBJECTS_751) \
$(RANDOM_OBJECT) $(SHA3_OBJECT)

LIB=.libs/libsidh.a

all: $(LIB)

src/P434/%.o: src/P434/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

src/P503/%.o: src/P503/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

src/P610/%.o: src/P610/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

src/P751/%.o: src/P751/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

ifeq "$(USE_OPT_LEVEL)" "_GENERIC_"		
    src/P434/P434_fp_generic.o: src/P434/generic/fp_generic.c
	    $(CC) -c $(CFLAGS) src/P434/generic/fp_generic.c -o src/P434/P434_fp_generic.o

    src/P503/P503_fp_generic.o: src/P503/generic/fp_generic.c
	    $(CC) -c $(CFLAGS) src/P503/generic/fp_generic.c -o src/P503/P503_fp_generic.o

    src/P610/P610_fp_generic.o: src/P610/generic/fp_generic.c
	    $(CC) -c $(CFLAGS) src/P610/generic/fp_generic.c -o src/P610/P610_fp_generic.o

    src/P751/P751_fp_generic.o: src/P751/generic/fp_generic.c
	    $(CC) -c $(CFLAGS) src/P751/generic/fp_generic.c -o src/P751/P751_fp_generic.o
else ifeq "$(USE_OPT_LEVEL)" "_FAST_"
ifeq "$(ARCHITECTURE)" "_AMD64_"		
    src/P434/P434_fp_x64.o: src/P434/AMD64/fp_x64.c
	    $(CC) -c $(CFLAGS) src/P434/AMD64/fp_x64.c -o src/P434/P434_fp_x64.o

    src/P434/P434_fp_x64_asm.o: src/P434/AMD64/fp_x64_asm.S
	    $(CC) -c $(CFLAGS) src/P434/AMD64/fp_x64_asm.S -o src/P434/P434_fp_x64_asm.o

    src/P503/P503_fp_x64.o: src/P503/AMD64/fp_x64.c
	    $(CC) -c $(CFLAGS) src/P503/AMD64/fp_x64.c -o src/P503/P503_fp_x64.o

    src/P503/P503_fp_x64_asm.o: src/P503/AMD64/fp_x64_asm.S
	    $(CC) -c $(CFLAGS) src/P503/AMD64/fp_x64_asm.S -o src/P503/P503_fp_x64_asm.o

    src/P610/P610_fp_x64.o: src/P610/AMD64/fp_x64.c
	    $(CC) -c $(CFLAGS) src/P610/AMD64/fp_x64.c -o src/P610/P610_fp_x64.o

    src/P610/P610_fp_x64_asm.o: src/P610/AMD64/fp_x64_asm.S
	    $(CC) -c $(CFLAGS) src/P610/AMD64/fp_x64_asm.S -o src/P610/P610_fp_x64_asm.o

    src/P751/P751_fp_x64.o: src/P751/AMD64/fp_x64.c
	    $(CC) -c $(CFLAGS) src/P751/AMD64/fp_x64.c -o src/P751/P751_fp_x64.o

    src/P751/P751_fp_x64_asm.o: src/P751/AMD64/fp_x64_asm.S
	    $(CC) -c $(CFLAGS) src/P751/AMD64/fp_x64_asm.S -o src/P751/P751_fp_x64_asm.o
else ifeq "$(ARCHITECTURE)" "_ARM64_"	
    src/P434/P434_fp_arm64.o: src/P434/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P434/ARM64/fp_arm64.c -o src/P434/P434_fp_arm64.o

    src/P434/P434_fp_arm64_asm.o: src/P434/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P434/ARM64/fp_arm64_asm.S -o src/P434/P434_fp_arm64_asm.o

    src/P503/P503_fp_arm64.o: src/P503/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P503/ARM64/fp_arm64.c -o src/P503/P503_fp_arm64.o

    src/P503/P503_fp_arm64_asm.o: src/P503/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P503/ARM64/fp_arm64_asm.S -o src/P503/P503_fp_arm64_asm.o

    src/P610/P610_fp_arm64.o: src/P610/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P610/ARM64/fp_arm64.c -o src/P610/P610_fp_arm64.o

    src/P610/P610_fp_arm64_asm.o: src/P610/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P610/ARM64/fp_arm64_asm.S -o src/P610/P610_fp_arm64_asm.o

    src/P751/P751_fp_arm64.o: src/P751/ARM64/fp_arm64.c
	    $(CC) -c $(CFLAGS) src/P751/ARM64/fp_arm64.c -o src/P751/P751_fp_arm64.o

    src/P751/P751_fp_arm64_asm.o: src/P751/ARM64/fp_arm64_asm.S
	    $(CC) -c $(CFLAGS) src/P751/ARM64/fp_arm64_asm.S -o src/P751/P751_fp_arm64_asm.o
endif
endif

$(RANDOM_OBJECT): src/random/random.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) src/random/random.c -o $(RANDOM_OBJECT)

$(SHA3_OBJECT): src/sha3/fips202.c
	$(CC) -c $(CFLAGS) src/sha3/fips202.c -o $(SHA3_OBJECT)
	
#$(LIB): $(ALL_OBJECTS)
#	rm -rf .libs
#	mkdir .libs
#	$(AR) $@ $^
#	$(RANLIB) $@
	
$(LIB): $(OBJECTS_751_COMP)
	rm -rf .libs
	mkdir .libs
	$(AR) $@ $^
	$(RANLIB) $@
	
# AES
AES_OBJS=objs/aes.o objs/aes_c.o

objs/%.o: tests/aes/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf *.req src/P434/*.o src/P503/*.o src/P610/*.o src/P751/*.o .libs* src/random/*.o src/sha3/*.o

