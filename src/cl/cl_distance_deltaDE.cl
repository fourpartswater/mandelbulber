formulaOut CalculateDistance(float4 point, sClFractal *fractal)
{
	float delta = 1e-6;
	float4 dr = 0.0;
	formulaOut out = Fractal(point, fractal);
	float r = out.distance;
	float r1 = Fractal(point + (float4){delta, 0.0, 0.0, 0.0}, fractal).distance;
	dr.x = fabs(r1 - r) / delta;
	float r2 = Fractal(point + (float4){0.0, delta, 0.0, 0.0}, fractal).distance;
	dr.y = fabs(r2 - r) / delta;
	float r3 = Fractal(point + (float4){0.0, 0.0, delta, 0.0}, fractal).distance;
	dr.z = fabs(r3 - r) / delta;
	float d = fast_length(dr);

	float distance = 0.5 * r * native_log(r) / d;

	if(distance<0.0f) distance = 0.0f;
	if(distance>10.0f) distance = 10.0f;
	out.distance = distance;
	return out;
}