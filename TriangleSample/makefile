export TOP_DIR =$(shell pwd)
export ARCH = arm64
export OUTPUT_DIR =$(TOP_DIR)/../bin
LD_FLAGS= -lm -lpthread -ldl -lX11 -lxcb -lXau -lXdmcp -ldrm -lXfixes -lXext -lXdamage -lexpat -lglapi -lX11-xcb -lxcb-glx -lxcb-dri2 -lEGL -lGLESv2
LIB=-L$(MALI_LIBS)
INC=-I$(MALI_INCLUDE)
PROJ_INCLUDES=-I../Coreframework

srcfiles := $(shell find ../Coreframework -maxdepth 1 -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: TriangleSample $(srcfiles)

TriangleSample $(objects) : TriangleSample.cpp $(srcfiles)
	$(ARM_CROSS_COMPILER_PATH) TriangleSample.cpp $(srcfiles) $(CXXFLAGS) $(LD_FLAGS) -o TriangleSample $(INC) $(PROJ_INCLUDES) $(LIB)
		cp -rf Supportingfiles bin
		cp -rf TriangleSample bin

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


install: TriangleSample
	mkdir -p $(TOP_DIR)/../bin/$(bin)/TriangleSample
	cp -rf TriangleSample  $(TOP_DIR)/../bin/$(bin)/TriangleSample
	cp -rf Supportingfiles  $(TOP_DIR)/../bin/$(bin)/TriangleSample


depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)
	-rm -rf bin
	-rm -rf TriangleSample
	

dist-clean: clean
	rm -f *~ .depend


include .depend
