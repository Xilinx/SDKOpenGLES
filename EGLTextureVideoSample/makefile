export TOP_DIR =$(shell pwd)
export ARCH = arm64
export OUTPUT_DIR =$(TOP_DIR)/../bin
LD_FLAGS= -lm -lpthread -ldl -lX11 -lxcb -lXau -lXdmcp -ldrm -lXfixes -lXext -lXdamage -lexpat -lglapi -lX11-xcb -lxcb-glx -lxcb-dri2 -lEGL -lGLESv2
LIB=-L$(MALI_LIBS)
INC=-I$(MALI_INCLUDE)
PROJ_INCLUDES=-I../Coreframework

srcfiles := $(shell find ../Coreframework -maxdepth 1 -name "*.c")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: EGLImageVideoSample $(srfiles)

EGLImageVideoSample $(objects) : EGLImageVideoSample.c $(srcfiles)
	$(ARM_CROSS_COMPILER_PATH_C) EGLImageVideoSample.c $(CXXFLAGS) $(LD_FLAGS) -o EGLImageVideoSample $(INC) $(PROJ_INCLUDES) $(LIB)
		cp -rf EGLImageVideoSample bin

# The dependency file names.
DEPS := $(objects:.o=.d)


bin=bin_FBDEV

install: EGLImageVideoSample
	mkdir -p $(TOP_DIR)/../bin/$(bin)/EGLImageVideoSample
	cp -rf EGLImageVideoSample  $(TOP_DIR)/../bin/$(bin)/EGLImageVideoSample


depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)
	-rm -rf EGLImageVideoSample
	-rm -rf bin

dist-clean: clean
	rm -f *~ .depend


include .depend
