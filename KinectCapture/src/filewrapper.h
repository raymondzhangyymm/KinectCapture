#pragma once

#include <Windows.h>
#include "kinectwrapper.h"
#include "ppm2bmp.h"
#include "ConfigHeader.h"

using namespace std;

class FileWrapper
{
public:
	FileWrapper(KinectWrapper *p_k) {
		// Allocate memory for local RGB-only image
		image_rgb.width = p_k->image_rgb.width;
		image_rgb.height = p_k->image_rgb.height;
		image_rgb.data = new unsigned char[image_rgb.width * image_rgb.height * 3];

		image_depth.width = p_k->image_depth.width;
		image_depth.height = p_k->image_depth.height;
		image_depth.data = new unsigned char[image_depth.width * image_depth.height * 3];

		// Delete the files in the working directory
		cleanupDir(DirRGB);
		cleanupDir(DirDepth);

		// Setup a timer to call the callback function periodly
		UINT TimerId = SetTimer(NULL, 0, CaptureIntervalTime, &TimerProc); 
	};

	~FileWrapper() {
		delete image_rgb.data;
		delete image_depth.data;
	}

private:
	static IMAGERGBA image_rgb, image_depth;
	static VOID CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
	//static void saveImageFilePPM(char *filename);
	static void saveImageFileBMP(char *filename);

	// Clean up the directory
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;
	LPSTR DirSpec;
	size_t length_of_arg;

	void cleanupDir(char *dd)
	{
		// Get the target directory
		//char currentdir[512], targetdir[512];
		//GetCurrentDirectory(512, currentdir);
		//
		//snprintf(targetdir, 512, "%s\\%s", currentdir, dd);
		//cout << "Clean up : " << targetdir << endl;

		// Find the first file in the directory.
		char ss[512];
		snprintf(ss, 512, "%s\\*", dd);
		hFind = FindFirstFile(ss, &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			printf("Invalid file handle. Error is %u.\n", GetLastError());
		}
		else
		{
			//printf("First file name is %s.\n", FindFileData.cFileName);

			// List all the other files in the directory.
			int count = 0;
			while (FindNextFile(hFind, &FindFileData) != 0)
			{
				if (FindFileData.cFileName[0] != '.') {
					snprintf(ss, 512, "%s\\%s", dd, FindFileData.cFileName);
					//printf("Delete %s\n", FindFileData.cFileName);
					DeleteFile(ss);
					count++;
				}
			}
			printf("Delete %d files.\n", count);

			dwError = GetLastError();
			FindClose(hFind);
			if (dwError != ERROR_NO_MORE_FILES)
			{
				printf("FindNextFile error. Error is %u.\n", dwError);
			}
		}
	}
};

IMAGERGBA FileWrapper::image_rgb, FileWrapper::image_depth;

VOID CALLBACK FileWrapper::TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime) {
	char ss[512];
	snprintf(ss, 512, "%d", dwTime/1000);
	saveImageFileBMP(ss);

	cout << "CALLBACK " << dwTime << ", " << ss << "\n";
	cout.flush();
}

//void FileWrapper::saveImageFilePPM(char *filename) {
//	int bufsize = width * height * 3;
//    unsigned char *buf = (unsigned char *)malloc(bufsize);
//	KinectWrapper::updateImageRGB();
//	KinectWrapper::getRGBBuffer(buf);
//
//	char outname[512];
//	snprintf(outname, 512, "%s\\%s.ppm", DirRGB, filename);
//	ofstream outfile(outname, ios::out | ios::binary);
//	outfile << "P6\n" << width << " " << height << "\n255\n";
//	outfile.write((char *)buf, sizeof(unsigned char) * bufsize);
//	outfile.close();
//
//	delete buf;
//}

void FileWrapper::saveImageFileBMP(char *filename) {
	char outname[512];
	KinectWrapper::updateImageRGB();
	KinectWrapper::getRGBBuffer(image_rgb.data);
	snprintf(outname, 512, "%s\\%s.bmp", DirRGB, filename);
	ppm2bmp(image_rgb.width, image_rgb.height, (BYTE*)image_rgb.data, outname);

	KinectWrapper::updateImageDepth();
	KinectWrapper::getDepthBuffer(image_depth.data);
	snprintf(outname, 512, "%s\\%s.bmp", DirDepth, filename);
	ppm2bmp(image_depth.width, image_depth.height, (BYTE*)image_depth.data, outname);
}