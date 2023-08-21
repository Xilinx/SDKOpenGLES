export TOP_DIR =$(shell pwd)
export ARCH = arm64
export OUTPUT_DIR =$(TOP_DIR)/../bin
LD_FLAGS= -lm -lpthread -ldl -lX11 -lxcb -lXau -lXdmcp -ldrm -lXfixes -lXext -lXdamage -lexpat -lglapi -lX11-xcb -lxcb-glx -lxcb-dri2 -lEGL -lGLESv2
LIB=-L$(MALI_LIBS)
INC=-I$(MALI_INCLUDE)
PROJ_INCLUDES=-I../SDK

srcfiles := $(shell find ../SDK -maxdepth 1 -name "*.c")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: EGLRenderingSample $(srfiles)

EGLRenderingSample $(objects) : EGLImageSample.c $(srcfiles)
	$(ARM_CROSS_COMPILER_PATH_C) EGLImageSample.c $(CXXFLAGS) $(LD_FLAGS) -o EGLRenderingSample $(INC) $(PROJ_INCLUDES) $(LIB)
		cp -rf EGLRenderingSample bin

# The dependency file names.
DEPS := $(objects:.o=.d)


bin=bin_FBDEV

install: EGLRenderingSample
	mkdir -p $(TOP_DIR)/../bin/$(bin)/EGLRenderingSample
	cp -rf EGLRenderingSample  $(TOP_DIR)/../bin/$(bin)/EGLRenderingSample


depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)
	-rm -rf bin
	-rm -rf EGLRenderingSample
	

dist-clean: clean
	rm -f *~ .depend


include .depend
