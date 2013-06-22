OBJS = objs/GameApp.o objs/GameLauncher.o objs/SimpleMenu.o objs/SimpleMenuItem.o objs/TextScroller.o objs/utils.o objs/ZipSupport.o
DEPS = $(patsubst objs/%.o, deps/%.d, $(OBJS))

RESULT = $(shell psp-config --psp-prefix 2> Makefile.cache)
QMAKE = qmake
QMAKEFILE = wagic-qt.pro
ifeq ($(RESULT),)
DEFAULT_RULE = qt
TARGET_ARCHITECTURE = linux
TARGET = bin/wagic
else
DEFAULT_RULE = 3xx
TARGET_ARCHITECTURE = psp
PSPSDK = $(shell psp-config --pspsdk-path)
PSPDIR = $(shell psp-config --psp-prefix)
TARGET = wagic
endif
ifeq ($(MAKECMDGOALS),debug)
DEFAULT_RULE = debug
endif
ifeq ($(MAKECMDGOALS),qt)
DEFAULT_RULE = qt
TARGET_ARCHITECTURE = linux
endif
ifeq ($(MAKECMDGOALS),x11)
# TODO: take other X11 platforms into account. Or not.
DEFAULT_RULE = linux.x11
TARGET_ARCHITECTURE = linux
endif

ifeq ($(TARGET_ARCHITECTURE),psp)
DEFAULT_RULE = 3xx
TARGET_ARCHITECTURE = psp
PSP_FW_VERSION=371
BUILD_PRX = 1
#SIGN_PRX = 1
LIBS = -ljge300 -lhgetools -lfreetype -ljpeg -lgif -lpng -lz -lm -lmikmod -lpsppower -lpspmpeg -lpspaudiocodec -lpspaudiolib -lpspaudio -lpspmp3 -lpspgum -lpspgu -lpsprtc -lstdc++ -lpspfpu

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = wMenu
#PSP_EBOOT_ICON = icon.png
#PSP_EBOOT_ICON1 = icon1.pmf
#PSP_EBOOT_UNKPNG = pic0.png
#PSP_EBOOT_PIC1 = pic1.png
INCDIR = ../../JGE/include ../../JGE/src/zipFS ../../JGE/include/psp ../../JGE/include/psp/freetype2 ../../JGE/src ../../projects/wmenu/include ../../Boost
LIBDIR = ../../JGE/lib/psp
CFLAGS = -O2 -G0 -DPSPFW3XX -DDEVHOOK -DUSE_PRECOMPILED_HEADERS=1 -DPSP
else
OBJS += objs/TestSuiteAI.o
INCDIR = -I../../JGE/include -I../../JGE/src -I/usr/X11/include -I../../projects/wmenu/include -I../../Boost -I../../JGE/src/zipFS
LIBDIR = -L../../JGE/lib/linux -L../../JGE -L/usr/X11/lib -L../../Boost/lib
LIBS = -ljge -lfreetype -ljpeg -lgif -lpng -lz -lm -lstdc++ -lhgetools -lGL -lGLU -lX11 -lboost_thread $(FMOD)
CFLAGS = $(INCDIR) -DLINUX -DUSE_PRECOMPILED_HEADERS=1
ASFLAGS = $(CXXFLAGS)

all: $(DEFAULT_RULE)

endif

CFLAGS := -Wall -W -Werror -Wno-unused $(CFLAGS)
CXXFLAGS += $(CFLAGS)
# -fno-exceptions

LDFLAGS += $(LIBS)

debug: CXXFLAGS += -ggdb3 -D_DEBUG -DDEBUG -DDEBUG_CACHE -pg

log: CXXFLAGS += -DDOLOG


ifeq ($(TARGET_ARCHITECTURE),psp)
include $(PSPSDK)/lib/build.mak

EXTRA_CLEAN += PrecompiledHeader.h.gch ../../projects/wmenu/include/PrecompiledHeader.h.gch

log: all
debug: all

3xx:
	@echo Rule 3xx is deprecated. Did you want to use just "make" ?

else



$(TARGET): Makefile.$(TARGET_ARCHITECTURE) $(OBJS) ../../JGE/lib/linux/libjge.a
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(LIBDIR)

x11: $(TARGET)

qt: Makefile.qt
	$(MAKE) -f Makefile.qt

debug: linux

clean:
	$(RM) PrecompiledHeader.h.gch $(OBJS) Makefile.$(TARGET_ARCHITECTURE)

endif

define compile
$(CXX) -c $(CXXFLAGS) $< -o $@  
endef

PrecompiledHeader.h.gch: ../../projects/wmenu/include/PrecompiledHeader.h
	$(compile)

$(OBJS): objs/%.o: src/%.cpp PrecompiledHeader.h.gch
	$(compile)

$(DEPS): deps/%.d: src/%.cpp
	@$(CXX) -MM $(CXXFLAGS) -MQ $(patsubst deps/%.d, objs/%.o, $(@)) -MQ $@ $< > $@

.DEFAULT:
	@echo $@ has been deleted : updating deps.

Makefile.linux:
	$(CXX) -o /dev/null src/testfeatures.c $(LIBDIR) -lfmod-3.75 > /dev/null 2>&1 ; if [ "0" = "$$?" ]; then echo 'FMOD=-lfmod-3.75'; else echo 'FMOD='; fi > $@

Makefile.qt:
	$(QMAKE) -o $@ $(QMAKEFILE)

-include $(DEPS)
-include Makefile.$(TARGET_ARCHITECTURE)
