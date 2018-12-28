#pragma once

#include <gl/GL.h>
#include <gl/GLU.h>
#include <freeglut.h>
#include "kinectwrapper.h"
#include "ConfigHeader.h"

class GlutWrapper
{
public:
	GlutWrapper(KinectWrapper *p_k) {
		if (DisplayDepthImage) {
			original_image = p_k->image_depth;
			image.width = original_image.width;
			image.height = original_image.height;
		}
		else {
			original_image = p_k->image_rgb;

			float truncate_scale = 0.5;
			image.width = original_image.width * truncate_scale;
			image.height = original_image.height * truncate_scale;
		}

		image.data = new unsigned char[image.width * image.height * 4];
	};

	static IMAGERGBA original_image, image;
	static GLuint textureId;
	static void draw();

	bool init(int argc, char* argv[], const char *window_title) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowSize(image.width, image.height);
		glutCreateWindow(window_title);
		glutDisplayFunc(draw);
		glutIdleFunc(draw);

		// Initialize textures
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, 
			         GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)image.data);
		glBindTexture(GL_TEXTURE_2D, 0);

		// OpenGL setup
		glClearColor(0, 0, 0, 0);
		glClearDepth(1.0f);
		glEnable(GL_TEXTURE_2D);

		// Camera setup
		glViewport(0, 0, image.width, image.height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, image.width, image.height, 0, 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		return true;
	}

	void execute() {
		glutMainLoop();
	}
};

IMAGERGBA GlutWrapper::original_image, GlutWrapper::image;
GLuint GlutWrapper::textureId; 

void GlutWrapper::draw() {
	// update the kinect
	KinectWrapper::update();

	// truncate or downsample the original image
	float sx = original_image.width / image.width;
	float sy = original_image.height / image.height;
	for (int y = 0; y < image.height; ++y) {
		for (int x = 0; x < image.width; ++x) {
			int idx = (y * image.width + x) * 4;
			int original_idx = (y*sy * original_image.width + x*sx) * 4;
			image.data[idx] = original_image.data[original_idx];
			image.data[idx+1] = original_image.data[original_idx+1];
			image.data[idx+2] = original_image.data[original_idx+2];
			image.data[idx+3] = original_image.data[original_idx+3];
		}
	}

    // display the image
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, 
		            GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)image.data);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(image.width, 0, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(image.width, image.height, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, image.height, 0.0f);
	glEnd();

    glutSwapBuffers();
}




