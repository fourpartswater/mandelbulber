/*
 * files.h
 *
 *  Created on: 2010-01-23
 *      Author: krzysztof marczak
 */

#ifndef FILES_H_
#define FILES_H_

extern "C"
{
#include <png.h>
#include <setjmp.h>
#include <jpeglib.h>
}

#include <string>
#include "cimage.hpp"

struct my_error_mgr
{
	struct jpeg_error_mgr pub; /* "public" fields */
	jmp_buf setjmp_buffer; /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

struct sImageRGBA16
{
	unsigned short R;
	unsigned short G;
	unsigned short B;
	unsigned short A;
};

extern std::string logfileName;

std::string IndexFilename(const char* filename, const char* extension, int number);
//METHODDEF(void) my_error_exit(j_common_ptr cinfo);
int LoadJPEG(const char *filename, JSAMPLE *image);
bool CheckJPEGsize(const char *filename, int *width, int *height);
void SaveJPEG(const char *filename, int quality, int width, int height, JSAMPLE *image);
void SavePNG(const char *filename, int quality, int width, int height, png_byte *image);
void SavePNG16(const char *filename, int quality, int width, int height, cImage *image);
void SavePNG16Alpha(const char *filename, int quality, int width, int height, cImage *image);
bool FileIfExists(const char *filename);
void WriteLog(const char *text);
void WriteLogDouble(const char *text, double value);
int fcopy(const char *source, const char *dest);
#endif /* FILES_H_ */
