/*
 * cl_support.cpp
 *
 *  Created on: 18-09-2011
 *      Author: krzysztof marczak
 *      some small parts of code taken from: http://www.codeproject.com/KB/GPU-Programming/IntroToOpenCL.aspx
 */

#include <string>
#include "interface.h"
#include "settings.h"
#include "cl_support.hpp"
#include <sstream>

CclSupport *clSupport;

#ifdef CLSUPPORT



CclSupport::CclSupport(void)
{
	enabled = false;
	ready = false;
	width = 800;
	height = 600;
	steps = 10;
	recompileRequest = true;
	lastFormula = trig_optim;
	lastEngineNumber = 0;
	lastStepSize = 0;
	inBuffer1 = new sClInBuff;
	constantsBuffer1 = new sClInConstants;
	auxReflectBuffer = NULL;
	buffSize = 0;
	context = NULL;
	kernel = NULL;
	outCL = NULL;
	rgbbuff = NULL;
	inCLBuffer1 = NULL;
	reflectBuffer = NULL;
	inCLConstBuffer1 = NULL;
	program = NULL;
	source = NULL;
	source2 = NULL;
	queue = NULL;
	memset(&lastParams, sizeof(lastParams), 0);
	memset(&lastFractal, sizeof(lastFractal), 0);
	isNVIDIA = false;
}

bool CclSupport::checkErr(cl_int err, const char * name)
{
	if (err != CL_SUCCESS)
	{
		std::stringstream errorMessageStream;
		errorMessageStream << "ERROR: " << name << " (" << err << ")";
		std::string errorMessage;
		errorMessage = errorMessageStream.str();
		std::cerr << errorMessage << std::endl;
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window_interface), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, errorMessage.c_str());
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		ready = false;
		enabled = false;
		recompileRequest = true;

#ifdef WIN32
		SetCurrentDirectory(data_directory);
#else
		chdir(data_directory);
#endif
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Interface.checkOpenClEnable), false);
		return false;
	}
	else
		return true;
}

void CclSupport::Init(void)
{
	cl_int err;

	ready = false;

	char progressText[1000];
	sprintf(progressText, "OpenCL - initialization");
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(Interface.progressBar), progressText);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(Interface.progressBar), 0.0);
	while (gtk_events_pending())
		gtk_main_iteration();

#ifdef WIN32
	const std::wstring opencldll( L"OpenCL.dll" );
	err = clewInit(opencldll.c_str());
	std::cout << clewErrorString(err) << std::endl;
#endif

	cl::Platform::get(&platformList);
	if(!checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get")) return;
	std::cout << "OpenCL Platform number is: " << platformList.size() << std::endl;

	platformList[0].getInfo((cl_platform_info) CL_PLATFORM_VENDOR, &platformVendor);
	std::cout << "OpenCL Platform is by: " << platformVendor << "\n";
	cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties) (platformList[0])(), 0 };
	if(platformVendor.find("NVIDIA") != std::string::npos)
	{
		isNVIDIA = true;
		printf("nVidia OpenCL library is detected\n");
	}
	if(platformVendor.find("Advanced") != std::string::npos)
	{
		isNVIDIA = false;
		printf("AMD OpenCL library is detected\n");
	}

	context = new cl::Context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
	if(!checkErr(err, "Context::Context()")) return;
	printf("OpenCL contexts created\n");

	devices = context->getInfo<CL_CONTEXT_DEVICES>();
	if(!checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0")) return;

	devices[0].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &numberOfComputeUnits);;
	printf("OpenCL Number of compute units: %d\n", numberOfComputeUnits);

	devices[0].getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &maxWorkItemDimmensions);
	printf("OpenCL Max work item dimmensions: %d\n", maxWorkItemDimmensions);

	devices[0].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxMaxWorkGroupSize);
	printf("OpenCL Max work group size: %d\n", maxMaxWorkGroupSize[0]);

	devices[0].getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &maxClockFrequency);
	printf("OpenCL Max clock frequency  %d MHz\n", maxClockFrequency);

	devices[0].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &memorySize);
	printf("OpenCL Memory size  %ld MB\n", memorySize/1024/1024);

	devices[0].getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &maxAllocMemSize);
	printf("OpenCL Max size of memory object allocation %ld MB\n", maxAllocMemSize/1024/1024);

	devices[0].getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &maxConstantBufferSize);
	printf("OpenCL Max constant buffer size  %ld kB\n", maxConstantBufferSize/1024);

	printf("OpenCL Constant buffer used  %ld kB\n", sizeof(sClInConstants)/1024);

	char text[1000];
	sprintf(text,"OpenCL platform by: %s", platformVendor.c_str());
	gtk_label_set_text(GTK_LABEL(Interface.label_OpenClPlatformBy), text);
	sprintf(text,"GPU frequency: %d MHz", maxClockFrequency);
	gtk_label_set_text(GTK_LABEL(Interface.label_OpenClMaxClock), text);
	sprintf(text,"GPU memory: %ld MB", memorySize/1024/1024);
	gtk_label_set_text(GTK_LABEL(Interface.label_OpenClMemorySize), text);
	sprintf(text,"Number of computing units: %d", numberOfComputeUnits);
	gtk_label_set_text(GTK_LABEL(Interface.label_OpenClComputingUnits), text);

	std::string clDir = std::string(sharedDir) + "cl/";

	std::string strFormula = "mandelbulb";

	if(lastFormula == trig_optim) strFormula = "mandelbulb";
	if(lastFormula == trig_DE) strFormula = "mandelbulb2";
	if(lastFormula == tglad)
	{
		if(lastFractal.mandelbox.rotEnabled)
			strFormula = "mandelbox_full";
		else
			strFormula = "mandelbox";
	}
	if(lastFormula == menger_sponge) strFormula = "mengersponge";
	if(lastFormula == hypercomplex) strFormula = "hypercomplex";
	if(lastFormula == quaternion) strFormula = "quaternion";
	if(lastFormula == kaleidoscopic) strFormula = "kaleidoscopic";
	if(lastFormula == xenodreambuie) strFormula = "xenodreambuie";

	std::string strFileEngine = clDir;
	int engineNumber = gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboOpenCLEngine));
	if		 (engineNumber == 0) 	strFileEngine += "cl_engine_fast.cl";
	else if(engineNumber == 1)	strFileEngine += "cl_engine.cl";
	else if(engineNumber == 2)	strFileEngine += "cl_engine_full.cl";

	std::ifstream fileEngine(strFileEngine.c_str());
	if(!checkErr(fileEngine.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileEngine).c_str())) return;

	std::string strFileDistance = clDir;
	if(lastFormula == xenodreambuie || lastFormula == hypercomplex)
		strFileDistance += "cl_distance_deltaDE.cl";
	else
		strFileDistance += "cl_distance.cl";
	std::ifstream fileDistance(strFileDistance.c_str());
	if(!checkErr(fileDistance.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileDistance).c_str())) return;

	std::string strFileFormulaBegin;
	if(lastFractal.juliaMode) strFileFormulaBegin  = clDir + "cl_formulaBegin" + "Julia" + ".cl";
	else  strFileFormulaBegin = clDir + "cl_formulaBegin" + ".cl";
	std::ifstream fileFormulaBegin(strFileFormulaBegin.c_str());
	if(!checkErr(fileFormulaBegin.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileFormulaBegin).c_str())) return;

	std::string strFileFormulaInit = clDir + "cl_" + strFormula + "Init.cl";
	std::ifstream fileFormulaInit(strFileFormulaInit.c_str());
	if(!checkErr(fileFormulaInit.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileFormulaInit).c_str())) return;

	std::string strFileFormulaFor = clDir + "cl_formulaFor.cl";
	std::ifstream fileFormulaFor(strFileFormulaFor.c_str());
	if(!checkErr(fileFormulaFor.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileFormulaFor).c_str())) return;

	std::string strFileFormula = clDir + "cl_" + strFormula + ".cl";
	std::ifstream fileFormula(strFileFormula.c_str());
	if(!checkErr(fileFormula.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileFormula).c_str())) return;

	std::string strFileFormulaEnd = clDir + "cl_formulaEnd.cl";
	std::ifstream fileFormulaEnd(strFileFormulaEnd.c_str());
	if(!checkErr(fileFormulaEnd.is_open() ? CL_SUCCESS : -1, ("Can't open file:" + strFileFormulaEnd).c_str())) return;

	std::string progEngine(std::istreambuf_iterator<char>(fileEngine), (std::istreambuf_iterator<char>()));
	std::string progDistance(std::istreambuf_iterator<char>(fileDistance), (std::istreambuf_iterator<char>()));
	std::string progFormulaBegin(std::istreambuf_iterator<char>(fileFormulaBegin), (std::istreambuf_iterator<char>()));
	std::string progFormulaInit(std::istreambuf_iterator<char>(fileFormulaInit), (std::istreambuf_iterator<char>()));
	std::string progFormulaFor(std::istreambuf_iterator<char>(fileFormulaFor), (std::istreambuf_iterator<char>()));
	std::string progFormula(std::istreambuf_iterator<char>(fileFormula), (std::istreambuf_iterator<char>()));
	std::string progFormulaEnd(std::istreambuf_iterator<char>(fileFormulaEnd), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources;
	sources.push_back(std::make_pair(progEngine.c_str(), progEngine.length()));
	sources.push_back(std::make_pair(progDistance.c_str(), progDistance.length()));
	sources.push_back(std::make_pair(progFormulaBegin.c_str(), progFormulaBegin.length()));
	sources.push_back(std::make_pair(progFormulaInit.c_str(), progFormulaInit.length()));
	sources.push_back(std::make_pair(progFormulaFor.c_str(), progFormulaFor.length()));
	sources.push_back(std::make_pair(progFormula.c_str(), progFormula.length()));
	sources.push_back(std::make_pair(progFormulaEnd.c_str(), progFormulaEnd.length()+1));
	printf("OpenCL Number of loaded sources %ld\n", sources.size());

	program = new cl::Program(*context, sources, &err);
	if(!checkErr(err, "Program()")) return;
	//program->getInfo(CL_PROGRAM_SOURCE, )
	//std::cout << "Program source:\t" << program->getInfo<CL_PROGRAM_SOURCE>() << std::endl;

#ifdef WIN32
	SetCurrentDirectory(clDir.c_str());
#else
	chdir(clDir.c_str());
#endif

	std::string buildParams;
	buildParams = "-w ";
#ifdef WIN32
	if(!isNVIDIA)
	{
		buildParams += "-I\"" + std::string(sharedDir) + "cl\" ";
	}
#else
	buildParams += "-I\"" + std::string(sharedDir) + "cl\" ";
#endif
	if(lastParams.DOFEnabled) buildParams += "-D_DOFEnabled ";
	if(lastParams.slowAmbientOcclusionEnabled) buildParams += "-D_SlowAOEnabled ";
	if(lastParams.fakeLightsEnabled) buildParams += "-D_orbitTrapsEnabled ";
	if(lastParams.auxLightNumber > 0) buildParams += "-D_AuxLightsEnabled ";
	printf("OpenCL build params:%s\n", buildParams.c_str());
	err = program->build(devices, buildParams.c_str());

	std::stringstream errorMessageStream;
	errorMessageStream << "OpenCL Build log:\t" << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
	std::string buildLogText;
	buildLogText = errorMessageStream.str();
	std::cout << buildLogText;

	if(!checkErr(err, "Program::build()"))
	{
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window_interface), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Program build log:");
		GtkWidget *messageArea = gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(dialog));

		GtkTextBuffer *textBuffer = gtk_text_buffer_new(NULL);
		gtk_text_buffer_set_text(textBuffer, buildLogText.c_str(), buildLogText.length());

		GtkWidget *textView = gtk_text_view_new_with_buffer(textBuffer);
		gtk_box_pack_start(GTK_BOX(messageArea), textView, false, false, 1);
		gtk_widget_show(textView);

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return;
	}

	printf("OpenCL program built done\n");

#ifdef WIN32
	SetCurrentDirectory(data_directory);
#else
  chdir(data_directory);
#endif

	kernel = new cl::Kernel(*program, "fractal3D", &err);
	if(!checkErr(err, "Kernel::Kernel()")) return;
	printf("OpenCL kernel opened\n");

	kernel->getWorkGroupInfo(devices[0], CL_KERNEL_WORK_GROUP_SIZE, &workGroupSize);
	printf("OpenCL workgroup size: %ld\n", workGroupSize);

	int pixelsPerJob =  workGroupSize * numberOfComputeUnits;
	steps = height * width / pixelsPerJob + 1;
	stepSize = (width * height / steps / workGroupSize + 1) * workGroupSize;
	printf("OpenCL Job size: %d\n", stepSize);
	buffSize = stepSize * sizeof(sClPixel);
	rgbbuff = new sClPixel[buffSize];

	reflectBufferSize = sizeof(sClReflect) * 10 * stepSize;
	reflectBuffer = new sClReflect[reflectBufferSize];
	auxReflectBuffer = new cl::Buffer(*context, CL_MEM_READ_WRITE, reflectBufferSize, NULL, &err);
	if(!checkErr(err, "Buffer::Buffer(*context, CL_MEM_READ_WRITE, reflectBufferSize, NULL, &err)")) return;

	inCLConstBuffer1 = new cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(sClInConstants), constantsBuffer1, &err);
	if(!checkErr(err, "Buffer::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(sClInConstants), constantsBuffer1, &err)")) return;

	inCLBuffer1 = new cl::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(sClInBuff), inBuffer1, &err);
	if(!checkErr(err, "Buffer::Buffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(sClInBuff), inBuffer1, &err)")) return;

	outCL = new cl::Buffer(*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize,rgbbuff,&err);
	if(!checkErr(err, "*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err")) return;

	printf("OpenCL buffers created\n");

	queue = new cl::CommandQueue(*context, devices[0], 0, &err);
	if(!checkErr(err, "CommandQueue::CommandQueue()"))return;
	printf("OpenCL command queue prepared\n");

	sprintf(progressText, "OpenCL - ready");
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(Interface.progressBar), progressText);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(Interface.progressBar), 1.0);

	sprintf(text,"Max workgroup size: %d", maxMaxWorkGroupSize[0]);
	gtk_label_set_text(GTK_LABEL(Interface.label_OpenClMaxWorkgroup), text);
	sprintf(text,"Actual workgroup size: %ld", workGroupSize);
	gtk_label_set_text(GTK_LABEL(Interface.label_OpenClWorkgroupSize), text);

	ready = true;
}

void CclSupport::SetParams(sClInBuff *inBuff, sClInConstants *inConstants, enumFractalFormula formula)
{
	if(inConstants->fractal.juliaMode != lastFractal.juliaMode) recompileRequest = true;
	if(formula != lastFormula) recompileRequest = true;
	if(inConstants->params.DOFEnabled != lastParams.DOFEnabled) recompileRequest = true;
	if(inConstants->params.slowAmbientOcclusionEnabled != lastParams.slowAmbientOcclusionEnabled) recompileRequest = true;
	if(inConstants->params.auxLightNumber != lastParams.auxLightNumber) recompileRequest = true;
	if(inConstants->fractal.mandelbox.rotEnabled != lastFractal.mandelbox.rotEnabled) recompileRequest = true;
	if(inConstants->params.fakeLightsEnabled != lastParams.fakeLightsEnabled) recompileRequest = true;

	int engineNumber = gtk_combo_box_get_active(GTK_COMBO_BOX(Interface.comboOpenCLEngine));
	if(engineNumber != lastEngineNumber) recompileRequest = true;
	lastEngineNumber = engineNumber;

	lastParams = inConstants->params;
	lastFractal = inConstants->fractal;
	lastFormula = formula;
}

void CclSupport::Render(cImage *image, GtkWidget *outputDarea)
{
	cl_int err;

	if(recompileRequest)
	{
		Disable();
		Enable();
		Init();
		recompileRequest = false;
	}

	stepSize = workGroupSize;
	int workGroupSizeMultiplier = 1;

	double startTime = real_clock();
	double lastTime = startTime;
	double lastTimeProcessing = startTime;
	double lastProcessingTime = 1.0;

	int nDof = 1;
	if(lastParams.DOFEnabled) nDof = 256;


	srand((unsigned int) ((double) clock() * 1000.0 / CLOCKS_PER_SEC));

	for(int dofLoop = 1; dofLoop <= nDof; dofLoop++)
	{
		for (int pixelIndex = 0; pixelIndex < width * height; pixelIndex += stepSize)
		{

			delete outCL;
			delete[] rgbbuff;
			delete auxReflectBuffer;
			delete[] reflectBuffer;

			double processingCycleTime = atof(gtk_entry_get_text(GTK_ENTRY(Interface.edit_OpenCLProcessingCycleTime)));
			if (processingCycleTime < 0.02) processingCycleTime = 0.02;

			workGroupSizeMultiplier *= processingCycleTime / lastProcessingTime;

			size_t sizeOfPixel = sizeof(sClPixel) + sizeof(sClReflect) * 10; //10 because max nuber of reflections is 10
			size_t jobSizeLimit;
			if (maxAllocMemSize > 0)
			{
				jobSizeLimit = maxAllocMemSize / sizeOfPixel * 0.75; //10 because max nuber of reflections is 10
			}
			else
			{
				jobSizeLimit = 65536;
			}
			//printf("job size limit: %ld\n", jobSizeLimit);
			int pixelsLeft = width * height - pixelIndex;
			int maxWorkGroupSizeMultiplier = pixelsLeft / workGroupSize;
			if (workGroupSizeMultiplier > maxWorkGroupSizeMultiplier) workGroupSizeMultiplier = maxWorkGroupSizeMultiplier;
			if (workGroupSizeMultiplier *  workGroupSize > jobSizeLimit) workGroupSizeMultiplier = jobSizeLimit / workGroupSize;
			if (workGroupSizeMultiplier < numberOfComputeUnits) workGroupSizeMultiplier = numberOfComputeUnits;

			constantsBuffer1->params.randomSeed = rand();

			stepSize = workGroupSize * workGroupSizeMultiplier;
			//printf("OpenCL Job size: %d\n", stepSize);
			buffSize = stepSize * sizeof(sClPixel);
			rgbbuff = new sClPixel[buffSize];
			outCL = new cl::Buffer(*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err);

			char errorText[1000];
			sprintf(errorText, "Buffer::Buffer(*context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, buffSize, rgbbuff, &err), buffSize = %ld", buffSize);
			if(!checkErr(err, errorText)) return;

			reflectBufferSize = sizeof(sClReflect) * 10 * stepSize;
			//printf("reflectBuffer size = %d\n", reflectBufferSize);
			reflectBuffer = new sClReflect[10 * stepSize];
			auxReflectBuffer = new cl::Buffer(*context, CL_MEM_READ_WRITE, reflectBufferSize, NULL, &err);
			sprintf(errorText, "Buffer::Buffer(*context, CL_MEM_READ_WRITE, reflectBufferSize, NULL, &err), reflectBufferSize = %ld", reflectBufferSize);
			if(!checkErr(err, errorText)) return;

			size_t usedGPUdMem = sizeOfPixel * stepSize;

			err = kernel->setArg(0, *outCL);
			err = kernel->setArg(1, *inCLBuffer1);
			err = kernel->setArg(2, *inCLConstBuffer1);
			err = kernel->setArg(3, *auxReflectBuffer);
			err = kernel->setArg(4, pixelIndex);

			//printf("size of inputs: %ld\n", sizeof(lastParams) + sizeof(lastFractal));

			err = queue->enqueueWriteBuffer(*inCLBuffer1, CL_TRUE, 0, sizeof(sClInBuff), inBuffer1);
			sprintf(errorText, "ComamndQueue::enqueueWriteBuffer(inCLBuffer1), used GPU mem = %ld", usedGPUdMem);
			if(!checkErr(err, errorText)) return;

			err = queue->finish();
			if(!checkErr(err, "ComamndQueue::finish() - CLBuffer")) return;

			err = queue->enqueueWriteBuffer(*inCLConstBuffer1, CL_TRUE, 0, sizeof(sClInConstants), constantsBuffer1);
			if(!checkErr(err, "ComamndQueue::enqueueWriteBuffer(inCLConstBuffer1)")) return;
			err = queue->finish();
			if(!checkErr(err, "ComamndQueue::finish() - ConstBuffer")) return;

			err = queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(stepSize), cl::NDRange(workGroupSize));
			sprintf(errorText, "ComamndQueue::nqueueNDRangeKernel(jobSize), jobSize = %d", stepSize);
			if(!checkErr(err, errorText)) return;

			err = queue->finish();
			if(!checkErr(err, "ComamndQueue::finish() - Kernel")) return;

			err = queue->enqueueReadBuffer(*outCL, CL_TRUE, 0, buffSize, rgbbuff);
			if(!checkErr(err, "ComamndQueue::enqueueReadBuffer()")) return;
			err = queue->finish();
			if(!checkErr(err, "ComamndQueue::finish() - ReadBuffer")) return;

			unsigned int offset = pixelIndex;

			for (unsigned int i = 0; i < stepSize; i++)
			{
				unsigned int a = offset + i;
				sRGBfloat pixel = { rgbbuff[i].R, rgbbuff[i].G, rgbbuff[i].B };
				int x = a % width;
				int y = a / width;

				if(lastParams.DOFEnabled)
				{
					sRGBfloat oldPixel = image->GetPixelImage(x,y);
					sRGBfloat newPixel;
					newPixel.R = oldPixel.R * (1.0 - 1.0/dofLoop) + pixel.R * (1.0/dofLoop);
					newPixel.G = oldPixel.G * (1.0 - 1.0/dofLoop) + pixel.G * (1.0/dofLoop);
					newPixel.B = oldPixel.B * (1.0 - 1.0/dofLoop) + pixel.B * (1.0/dofLoop);
					image->PutPixelImage(x, y, newPixel);
					image->PutPixelZBuffer(x, y, rgbbuff[i].zBuffer);
				}
				else
				{
					image->PutPixelImage(x, y, pixel);
					image->PutPixelZBuffer(x, y, rgbbuff[i].zBuffer);
				}
			}

			char progressText[1000];
			double percent;
			if(lastParams.DOFEnabled)
			{
				percent = (double) (dofLoop - 1.0) / nDof + (double) (pixelIndex + stepSize) / (width * height) / nDof;
			}
			else
			{
				percent = (double) (pixelIndex + stepSize) / (width * height);
			}
			if (percent > 1.0) percent = 1.0;
			double time = real_clock() - startTime;
			double time_to_go = (1.0 - percent) * time / percent;
			int togo_time_s = (int) time_to_go % 60;
			int togo_time_min = (int) (time_to_go / 60) % 60;
			int togo_time_h = time_to_go / 3600;
			int time_s = (int) time % 60;
			int time_min = (int) (time / 60) % 60;
			int time_h = time / 3600;
			sprintf(progressText, "OpenCL - rendering. Done %.1f%%, to go %dh%dm%ds, elapsed %dh%dm%ds, used GPU mem %ld MB", percent * 100.0, togo_time_h, togo_time_min, togo_time_s, time_h,
					time_min, time_s, usedGPUdMem/1024/1024);
			gtk_progress_bar_set_text(GTK_PROGRESS_BAR(Interface.progressBar), progressText);
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(Interface.progressBar), percent);

			lastProcessingTime = time - lastTimeProcessing;
			lastTimeProcessing = time;

			if (real_clock() - lastTime > 30.0)
			{
				if (image->IsPreview())
				{
					image->CompileImage();
					image->ConvertTo8bit();
					image->UpdatePreview();
					image->RedrawInWidget(outputDarea);
					while (gtk_events_pending())
						gtk_main_iteration();
				}
				lastTime = real_clock();
			}

			if (programClosed) break;

			while (gtk_events_pending())
				gtk_main_iteration();
		}
		if (programClosed) break;
	}

	//gdk_draw_rgb_image(outputDarea->window, renderWindow.drawingArea->style->fg_gc[GTK_STATE_NORMAL], 0, 0, clSupport->GetWidth(), clSupport->GetHeight(), GDK_RGB_DITHER_NONE,
	//		clSupport->GetRgbBuff(), clSupport->GetWidth() * 3);
}

void CclSupport::Enable(void)
{
	if(ready)
	{
		enabled = true;
	}
}

void CclSupport::Disable(void)
{
	if(context) delete context;
	context = NULL;
	if(rgbbuff) delete[] rgbbuff;
	rgbbuff = NULL;
	if(outCL) delete outCL;
	outCL = NULL;
	if(program) delete program;
	program = NULL;
	if(kernel) delete kernel;
	kernel = NULL;
	if(queue)	delete queue;
	queue = NULL;
	if (inCLBuffer1)delete inCLBuffer1;
	inCLBuffer1 = NULL;
	if(inCLConstBuffer1) delete inCLConstBuffer1;
	inCLConstBuffer1 = NULL;
	if(reflectBuffer) delete[] reflectBuffer;
	reflectBuffer = NULL;
	enabled = false;
	ready = false;
}

void CclSupport::SetSize(int w, int h)
{
	//if(width != w || height != h) recompileRequest = true;
	width = w;
	height = h;
}

#endif
