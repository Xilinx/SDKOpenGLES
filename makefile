all:
	(cd YUV2RGBSample; make all)
	(cd WaveformSample; make all)
	(cd FBOSample; make all)
	(cd TriangleSample; make all)
	(cd DepthSample; make all)
	(cd MipMapSample; make all)
	(cd MoleculeSystemSample; make all)
	(cd MultiCubeSample; make all)
	(cd EGLImageSample; make all)
	(cd EGLImageVideoSample; make all)

install:
	(cd YUV2RGBSample; make install)
	(cd WaveformSample; make install)
	(cd FBOSample; make install)
	(cd TriangleSample; make install)
	(cd DepthSample; make install)
	(cd MipMapSample; make install)
	(cd MoleculeSystemSample; make install)
	(cd MultiCubeSample; make install)
	(cd EGLImageSample; make install)
	(cd EGLImageVideoSample; make install)

clean:
	(cd YUV2RGBSample; make clean)
	(cd FBOSample; make clean)
	(cd TriangleSample; make clean)
	(cd DepthSample; make clean)
	(cd MipMapSample; make clean)
	(cd MoleculeSystemSample; make clean)
	(cd MultiCubeSample; make clean)
	(cd EGLImageSample; make clean)
	(cd EGLImageVideoSample; make clean)
	(cd WaveformSample; make clean)
