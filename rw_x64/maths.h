#pragma once
#include "qangle.h"
#include "vector3.h"

void NormalizeAngles(QAngle& angle);

float GetFov(const QAngle& viewAngle, const QAngle& aimAngle);

float VectorDistance(Vector3 v1, Vector3 v2);

QAngle CalcAngle(const Vector3& src, const Vector3& dst);

float Round(float var);

float float_rand(float min, float max);