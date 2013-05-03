/*
 * shaders.h
 *
 *  Created on: 2010-04-24
 *      Author: Krzysztof Marczak
 */

#ifndef SHADERS_H_
#define SHADERS_H_

#include "Render3D.h"

struct sShaderOutput
{
	double R;
	double G;
	double B;
};

struct sLight
{
	CVector3 position;
	sRGB colour;
	double intensity;
	bool enabled;
};

struct sShaderInputData
{
	sParamRender *param;
	sFractal *calcParam;
	CVector3 point;
	CVector3 viewVector;
	CVector3 normal;
	CVector3 lightVect;
	double dist_thresh;
	double lastDist;
};

extern sLight *Lights;
extern int lightsPlaced;

sShaderOutput ObjectShader(sShaderInputData input, sShaderOutput *surfaceColour);

sShaderOutput MainShadow(sShaderInputData &input);
sShaderOutput AmbientOcclusion(sParamRender *param, sFractal *calcParam, CVector3 point, double wsp_persp, double dist_thresh, double last_distance, sVectorsAround *vectorsAround,
		int vectorsCount);
sShaderOutput FastAmbientOcclusion(sFractal *calcParam, CVector3 point);
sShaderOutput FastAmbientOcclusion2(sFractal *calcParam, CVector3 point, CVector3 normal, double dist_thresh, double tune, int quality);
CVector3 CalculateNormals(sShaderInputData input);
sShaderOutput MainShading(sShaderInputData &input);
sShaderOutput MainSpecular(sShaderInputData &input);
sShaderOutput EnvMapping(CVector3 normal, CVector3 viewVector, cTexture *texture);
sShaderOutput SurfaceColour(sShaderInputData &input);
sShaderOutput TexturedBackground(sParamRender *param, CVector3 viewVector);
sShaderOutput LightShading(sParamRender *fractParams, sFractal *calcParam, CVector3 point, CVector3 normal, CVector3 viewVector, sLight light, double wsp_persp, double dist_thresh,
		int number, sShaderOutput *outSpecular);
sShaderOutput AuxLightsShader(sParamRender *param, sFractal *calcParam, CVector3 point, CVector3 normal, CVector3 viewVector, double wsp_persp, double dist_thresh,
		sShaderOutput *specularAuxOut);
double AuxShadow(sParamRender *fractParams, sFractal *calcParam, double wsp_persp, double dist_thresh, double distance, CVector3 point, CVector3 lightVector, bool linear);
sShaderOutput VolumetricLight(sParamRender *param, sFractal *calcParam, CVector3 point, double yStart, double min_y, double last_distance, double zoom, CVector3 lightVector);
sShaderOutput VolumetricFog(sParamRender *param, int buffCount, double *distanceBuff, double *stepBuff, double *densityOut);
void PlaceRandomLights(sParamRender *fractParams, bool onlyPredefined);
void PostRenderingLights(cImage *image, sParamRender *fractParam);
void RenderBuddhabrot(sParamRender *fractParam);
double IterOpacity(double step, double iters, double maxN, double trim, double opacitySp);
sShaderOutput IterationFog(sParamRender *param, sFractal *calcParam, CVector3 point, double yStart, double min_y, double last_distance, double zoom,
		CVector3 lightVector, bool found, double *densityOut, sVectorsAround *vectorsAround, int vectorsCount, sRGB16 oldPixel);
sShaderOutput FakeLights(sParamRender *param, sFractal *calcParam, CVector3 point, CVector3 normal, CVector3 viewVector, double dist_thresh, sShaderOutput *fakeSpec);
sRGB IndexToColour(int index, sRGB *palette);
#endif /* SHADERS_H_ */
