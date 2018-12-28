#pragma once

// This is the configuration for the Visual Studio project.

// 1. Make a directory ""..\\..\\_Build" for the intermediate files.
// 2. Make the following directories for the output image files
#define DirRGB "..\\..\\_Output\\RGB"
#define DirDepth "..\\..\\_Output\\Depth"

// 3. This is the time interval for capturing Kinect images in milliseconds.
#define CaptureIntervalTime 2000

// 4. Optionally let the preview window display Kinect depth image (true) or RBG image (false)
#define DisplayDepthImage true