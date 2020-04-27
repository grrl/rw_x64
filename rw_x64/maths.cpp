#include "maths.h"
#include "vector3.h"

void NormalizeAngles(QAngle& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

float Round(float var)
{
	// 37.66666 * 100 =3766.66 
	// 3766.66 + .5 =37.6716    for rounding off value 
	// then type cast to int so value is 3766 
	// then divided by 100 so the value converted into 37.66 
	return roundf(var * 100) / 100;
}

float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	NormalizeAngles(delta);

	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

float VectorDistance(Vector3 v1, Vector3 v2)
{
	return (float)sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2)) * 0.0254f;

}

float FastSQRT(float x)
{
	union { int i; float x; } u;
	u.x = x; u.i = (u.i >> 1) + 0x1FC00000;
	u.x = u.x + x / u.x; return .25f * u.x + x / u.x;
}

float Dist3D(const Vector3& Src, const Vector3& Dst)
{
	return FastSQRT(powf(Src.x - Dst.x, 2.f) + powf(Src.y - Dst.y, 2.f) + powf(Src.z - Dst.z, 2.f));
}

QAngle CalcAngle(const Vector3& src, const Vector3& dst)
{
	QAngle srca = { src.x, src.y, src.z };
	QAngle dsta = { dst.x, dst.y, dst.z };

	QAngle angle;
	QAngle delta = srca - dsta;

	double hyp = sqrtf((double)(delta.x * delta.x) + (delta.y * delta.y));
	angle.x = atan(delta.z / hyp) * 57.295779513082f;
	angle.y = atan(delta.y / delta.x) * 57.295779513082f;
	if (delta.x >= 0.0) angle.y += 180.0f;


	return angle;
}

float float_rand(float min, float max)
{
	float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
	return min + scale * (max - min);      /* [min, max] */
}