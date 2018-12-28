#include <Windows.h>
#include <Ole2.h>

#include "glutwrapper.h" 
#include "kinectwrapper.h"
#include "filewrapper.h"

int main(int argc, char* argv[]) {
	KinectWrapper *p_kinect = new KinectWrapper();
	GlutWrapper *p_glut = new GlutWrapper(p_kinect);
	p_glut->init(argc, argv, "Kinect Image RGB");

 	FileWrapper *p_file = new FileWrapper(p_kinect);

    // Main loop
    p_glut->execute();
    return 0;
}
