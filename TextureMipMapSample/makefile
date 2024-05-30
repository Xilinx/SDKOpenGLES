export TOP_DIR =$(shell pwd)
export ARCH = arm64
export OUTPUT_DIR =$(TOP_DIR)/../bin
LD_FLAGS= -lm -lpthread -ldl -lX11 -lxcb -lXau -lXdmcp -ldrm -lXfixes -lXext -lXdamage -lexpat -lglapi -lX11-xcb -lxcb-glx -lxcb-dri2 -lEGL -lGLESv2
LIB=-L$(MALI_LIBS)
INC=-I$(MALI_INCLUDE)
PROJ_INCLUDES=-I../Coreframework

srcfiles := $(shell find ../Coreframework -maxdepth 1 -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: TextureMipMapSample $(srcfiles)

TextureMipMapSample $(objects) : MipMapSample.cpp $(srcfiles)
	$(ARM_CROSS_COMPILER_PATH) MipMapSample.cpp $(srcfiles) $(CXXFLAGS) $(LD_FLAGS) -o TextureMipMapSample $(INC) $(PROJ_INCLUDES) $(LIB)
		cp -rf Supportingfiles bin
		cp -rf TextureMipMapSample bin

# The dependency file names.
DEPS := $(objects:.o=.d)


bin=
bin_FBDEV=bin_FBDEV
bin_X11=bin_X11

ifeq ($(CXXFLAGS),-DENABLE_FBDEV)
        bin=$(bin_FBDEV)
else
        bin=$(bin_X11)
endif


install: TextureMipMapSample
	mkdir -p $(TOP_DIR)/../bin/$(bin)/TextureMipMapSample
	cp -rf TextureMipMapSample  $(TOP_DIR)/../bin/$(bin)/TextureMipMapSample
	cp -rf Supportingfiles  $(TOP_DIR)/../bin/$(bin)/TextureMipMapSample



depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)
	-rm -rf bin
	-rm -rf TextureMipMapSample
	

dist-clean: clean
	rm -f *~ .depend


include .depend

