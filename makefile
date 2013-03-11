
# Nexus examples

#ifdef COMSPEC
# Any DOS environment
#NEXUS_TOP := $(shell cd .. && cd)
#else
#NEXUS_TOP := $(shell cd ../../; pwd)
#endif

APPLIB_TOP = $(NEXUS_TOP)/../BSEAV/lib

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
Q_ ?= @
else
# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
endif

PATH_TOMMATH = $(APPLIB_TOP)/libtommath
#PATH_ZLIB = $(APPLIB_TOP)/zlib/zlib-1.2.3
PATH_ZLIB = $(APPLIB_TOP)/zlib
PATH_FFMPEG = $(APPLIB_TOP)/libffmpeg

THEAPPS = libhmplayer.so 

ifeq ($(B_REFSW_OS),vxworks)
# VxWorks needs a wrapper function to call main.
VXOBJS = vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, $(THEAPPS))
else
ifeq ($(B_REFSW_OS),linuxkernel)
APPSUFFIX = .ko
APPS = $(addsuffix ${APPSUFFIX}, $(THEAPPS))
else
APPS = $(THEAPPS)
endif
endif


.PHONY: api clean
OBJDIR=./obj_so

all: $(APPS)

$(OBJDIR)/exists:
	mkdir -p $(OBJDIR)
	@touch $@

ifneq ($(NEXUS_PREBUILT_BINARY),y)
$(APPS): api
endif

ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -L${NEXUS_BIN_DIR}/lib -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm 
endif

# For VxWorks builds, link in the nexus library.
ifeq ($(B_REFSW_OS),vxworks)
LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR}
endif

# The following script assumes the gcc version number is the last field on the first line
GCC_VERSION ?= $(shell $(CC) --version |  $(AWK) 'NR == 1 { print $$NF }')

#hh add 
LDFLAGS += -L$(PATH_ZLIB) -lz 
LDFLAGS += -L$(PATH_TOMMATH)/lib -ltommath
LDFLAGS += -L./lib -L/root/syslibs/libnexus -lcimov -shared -fPIC


# This builds the Nexus api
#api:
#	$(MAKE) -C $(NEXUS_TOP)/build

# This cleans nexus and local apps
#clean: clean_apps
#	$(MAKE) -C $(NEXUS_TOP)/build clean

#clean_apps:
clean:
	-$(RM) -r $(APPS) $(OBJDIR)


# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# Always build with debug
CFLAGS += -g -O0 -std=gnu99 -shared -fPIC
 
# media probe (only needed for playback and record)
BSEAV = $(NEXUS_TOP)/../BSEAV
include $(BSEAV)/lib/bfile/bfile.inc
ifeq ($(MEDIA_AVI_SUPPORT),y)
include $(BSEAV)/lib/avi/bavi.inc
endif
ifeq ($(MEDIA_ASF_SUPPORT),y)
include $(BSEAV)/lib/asf/basf.inc
endif
ifeq ($(MEDIA_RMFF_SUPPORT),y)
include $(BSEAV)/lib/media/rmff/brmff.inc
endif
include $(BSEAV)/lib/utils/batom.inc
include $(BSEAV)/lib/media/bmedia.inc
CFLAGS += $(addprefix -I,$(BMEDIA_INCLUDES) $(BFILE_MEDIA_INCLUDES)) -I$(BSEAV)/lib/mpeg2_ts_parse -I$(BSEAV)/lib/tspsi
CFLAGS += $(addprefix -D,$(BMEDIA_PROBE_DEFINES))

CFLAGS += -I./io/ -I$(PATH_TOMMATH)/include -I$(PATH_ZLIB) -I$(PATH_FFMPEG)/include 

DECODE_SRCS = io/nexusio_cryp_ec.c io/nexusio_mftp_cs.c io/nexusio_mftp_wd.c io/nexusio_song_de.c io/nexusio_file_io.c io/nexusio_mftp_io.c nexus_bmz.c nexus_xpiz.c nexus_link.c nexus_osd.c nexus_ctrl.c nexus_transport.c nexus_mw.c nexus_pizplay.c nexus_ctrl_sec.c nexus_audioplayer.c cmdline_args.c

DECODE_OBJS = $(foreach file, $(DECODE_SRCS), $(OBJDIR)/$(notdir $(patsubst %.c, %.o, $(file))))

vpath %.c $(foreach file, $(DECODE_SRCS), $(dir $(patsubst %.c, %.o, $(file))))

test:
	@echo $(EXTRA_OBJS)
	@echo $(foreach file, $(EXTRA_SRCS), $(dir $(patsubst %.c, %.o, $(file))))

$(OBJDIR)/%.o: %.c ${OBJDIR}/exists
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)
#	$(CC) -c -o $@ $< $(CFLAGS)

libhmplayer.so : $(OBJDIR)/nexus_hmplayerlib.o $(DECODE_OBJS)
ifeq ($(OS),linuxkernel)
	${Q_}$(LD) ${LDFLAGS} --strip-debug -r $(filter %.o, $^) ${NEXUS_BIN_DIR}/libnexus.a $(FREETYPE)/lib/libfreetype.a$@
else
	$(Q_)$(CC) -o $@ $(filter %.o, $^) $(LDFLAGS)
endif
#	mipsel-linux-strip $@
	cp $@ ${NEXUS_BIN_DIR}

HMPLAYER_SDK :=  $(shell cd ./; pwd)/libhmplayer

install:
	cp *.h $(HMPLAYER_SDK)/include/
	cp io/*.h  $(HMPLAYER_SDK)/include/
	cp libhmplayer.so $(HMPLAYER_SDK)/lib/
