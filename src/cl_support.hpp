/*
 * cl_support.hpp
 *
 *  Created on: 18-09-2011
 *      Author: krzysztof
 */

#ifndef CL_SUPPORT_HPP_
#define CL_SUPPORT_HPP_

//#define __NO_STD_VECTOR // Use cl::vector instead of STL version

//#define CLSUPPORT

#ifdef CLSUPPORT

#ifdef WIN32
#include "clew.h"
#include "clew-cl.hpp"
#else
#include <CL/cl.hpp>
#endif
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

//#include "fractal.h"
#include "cimage.hpp"


#include "mandelbulber_cl_data.h"

class CCustomFormulas
{
public:
	CCustomFormulas(std::string dataDir);
	~CCustomFormulas();
	void SetActual(int index) {actualIndex = index;}
	void GetActual(std::string *name, std::string *formulaFile, std::string *iniFile);
	void NewFormula(std::string newName);
	void DeleteFormula(void);
private:
	void RefreshList(void);
	std::vector<std::string> listOfFiles;
	std::vector<std::string> listOfNames;
	int count;
	int actualIndex;
	std::string customFormulasPath;
};

class CclSupport
{
public:
	CclSupport();
	bool checkErr(cl_int err, const char * name);
	void Init(void);
	void SetParams(sClInBuff *inBuff, sClInConstants *inConstants, enumFractalFormula formula);
	void Render(cImage *image, GtkWidget *outputDarea);
	sClPixel * GetRgbBuff() {return rgbbuff;}
	bool IsReady(void) {return ready;}
	bool IsEnabled(void) {return enabled;}
	void Enable(void);
	void Disable(void);
	int GetWidth() {return width;}
	int GetHeight() {return height;}
	void SetSize(int w, int h);
	void RecopileRequest(void);
	void Recompile(void);
	sClInBuff* GetInBuffer1(void) {return inBuffer1;}
	sClInConstants* GetInConstantBuffer1(void) {return constantsBuffer1;}
	CCustomFormulas *customFormulas;

private:
	bool enabled;
	bool ready;
	bool recompileRequest;
	sClPixel *rgbbuff;
	sClInBuff *inBuffer1;
	sClReflect *reflectBuffer;
	sClInConstants *constantsBuffer1;
	std::vector<cl::Platform> platformList;
	std::string platformVendor;
	cl::Context *context;
	cl::Buffer *outCL;
	cl::Buffer *inCLBuffer1;
	cl::Buffer *inCLConstBuffer1;
	cl::Buffer *auxReflectBuffer;
	std::vector<cl::Device> devices;
	cl::Program::Sources *source;
	cl::Program::Sources *source2;
	cl::Program *program;
	cl::Kernel *kernel;
	cl::CommandQueue *queue;

	int width;
	int height;
	unsigned int deviceIndex;
	unsigned int platformIndex;
	bool useCPU;
	size_t buffSize;
	unsigned int stepSize;
	unsigned int steps;
	size_t reflectBufferSize;
	cl_int numberOfComputeUnits;
	cl_int maxWorkItemDimmensions;
	cl_int maxMaxWorkGroupSize[3];
	cl_int maxClockFrequency;
	size_t memorySize;
	size_t maxAllocMemSize;
	size_t memoryLimitByUser;
	size_t workGroupSize;
	size_t maxConstantBufferSize;

	bool isNVIDIA;

	sClParams lastParams;
	sClFractal lastFractal;
	enumFractalFormula lastFormula;
	int lastStepSize;
	int lastEngineNumber;


};

#else
//dummy class CclSupport
class CclSupport
{
public:
	bool IsEnabled(void) {return false;}
	bool IsReady(void) {return false;}
	void Enable(void) {return;}
	void Disable(void) {return;}
	void Init(void) {return;}
	void SetSize(int w, int h) {return;};
};
#endif

extern CclSupport *clSupport;



#endif /* CL_SUPPORT_HPP_ */
