#pragma once

#include <Kinect.h>
#include <iostream>
#include <fstream>
#include <string>

typedef struct tagIMAGERGBA {
	int width;
	int height;
	unsigned char *data;
} IMAGERGBA;

class KinectWrapper
{
public:
	KinectWrapper() {
		image_rgb.width = 1920;
		image_rgb.height = 1080;
		image_rgb.data = new unsigned char[image_rgb.width * image_rgb.height * 4];

		image_depth.width = 512;
		image_depth.height = 424;
		image_depth.data = new unsigned char[image_depth.width * image_depth.height * 4];

		depthbuf = new unsigned short[image_depth.width * image_depth.height];

		init();
	};

	~KinectWrapper() {
		delete image_rgb.data;
		delete image_depth.data;
		delete depthbuf;
	}

	static IMAGERGBA image_rgb, image_depth;
	static bool update();
	static bool updateImageRGB();
	static bool updateImageDepth();
	static void getRGBBuffer(unsigned char *buf);
	static void getDepthBuffer(unsigned char *buf);

	bool init() {
		if (FAILED(GetDefaultKinectSensor(&sensor))) {
			return false;
		}
		if (sensor) {
			sensor->Open();

			IColorFrameSource* framesource_rgb = NULL;
			sensor->get_ColorFrameSource(&framesource_rgb);
			framesource_rgb->OpenReader(&reader_rgb);
			if (framesource_rgb) {
				framesource_rgb->Release();
				framesource_rgb = NULL;
			}

			IDepthFrameSource* framesource_depth = NULL;
			sensor->get_DepthFrameSource(&framesource_depth);
			framesource_depth->OpenReader(&reader_depth);
			if (framesource_depth) {
				framesource_depth->Release();
				framesource_depth = NULL;
			}

			std::cout << "Kinect is initialized.\n";
			return true;
		}
		else {
			return false;
		}
	}

private:
	// Kinect variables
	IKinectSensor* sensor;         // Kinect sensor
	static IColorFrameReader* reader_rgb;     // Kinect color data source
	static IDepthFrameReader* reader_depth;     // Kinect depth data source

	static unsigned short *depthbuf; //for the original Kinect depth data
};

IMAGERGBA KinectWrapper::image_rgb;
IMAGERGBA KinectWrapper::image_depth;
IColorFrameReader *KinectWrapper::reader_rgb = NULL;
IDepthFrameReader *KinectWrapper::reader_depth = NULL;
unsigned short *KinectWrapper::depthbuf = NULL;

bool KinectWrapper::update() {
	if (updateImageRGB() && updateImageDepth()) return true;
	else return false;
}

bool KinectWrapper::updateImageRGB() {
	IColorFrame* frame_rgb = NULL;
	if (SUCCEEDED(reader_rgb->AcquireLatestFrame(&frame_rgb))) {
		frame_rgb->CopyConvertedFrameDataToArray(image_rgb.width * image_rgb.height * 4,
			                                     (GLubyte*)image_rgb.data, ColorImageFormat_Bgra);
		frame_rgb->Release();
		return true;
	}
	else
		return false;
}

bool KinectWrapper::updateImageDepth() {
	IDepthFrame* frame_depth = NULL;
	if (SUCCEEDED(reader_depth->AcquireLatestFrame(&frame_depth))) {
		frame_depth->CopyFrameDataToArray(image_depth.width * image_depth.height, depthbuf);
		frame_depth->Release();

		// convert the depth buffer to a grayscale image
        // the kinect depth data is of a short integer in millimeters
		for (int y = 0; y < image_depth.height; ++y) {
			for (int x = 0; x < image_depth.width; ++x) {
				unsigned short *p = depthbuf + (y * image_depth.width + x);
				int idx = (y * image_depth.width + x) * 4;

				float v = (*p) * 255 / 3000; // truncate the furthest distance at 3m 
				if (v > 255) v = 255;
				image_depth.data[idx] = v;
				image_depth.data[idx + 1] = v;
				image_depth.data[idx + 2] = v;
				image_depth.data[idx + 3] = 255;  // the alpha value
			}
		}

		return true;
	}
	else
		return false;
}

void KinectWrapper::getRGBBuffer(unsigned char *buf) {
    for (int y = 0; y < image_rgb.height; ++y) {
	    for (int x = 0; x < image_rgb.width; ++x) {
		    unsigned char *p = image_rgb.data + (y * image_rgb.width + x) * 4;
		    int idx = (y * image_rgb.width + x) * 3;
		    buf[idx] = *p;
		    buf[idx + 1] = *(p + 1);
		    buf[idx + 2] = *(p + 2);
	    }
    }
}

void KinectWrapper::getDepthBuffer(unsigned char *buf) {
	for (int y = 0; y < image_depth.height; ++y) {
		for (int x = 0; x < image_depth.width; ++x) {
			unsigned char *p = image_depth.data + (y * image_depth.width + x) * 4;
			int idx = (y * image_depth.width + x) * 3;
			buf[idx] = *p;
			buf[idx + 1] = *(p + 1);
			buf[idx + 2] = *(p + 2);
		}
	}
}