#pragma once
#include "vector3.h"
#include "d3d9.h"
float m_fMatrix[16];
bool WorldToScreen(Vector3 &vOrigin, Vector3 &vScreen)
{
	float w = 0.0f;

	//std::cout << "is " << m_fMatrix[0] << std::endl;
	vScreen.x = m_fMatrix[0] * vOrigin.x + m_fMatrix[1] * vOrigin.y + m_fMatrix[2] * vOrigin.z + m_fMatrix[3];
	vScreen.y = m_fMatrix[4] * vOrigin.x + m_fMatrix[5] * vOrigin.y + m_fMatrix[6] * vOrigin.z + m_fMatrix[7];
	w = m_fMatrix[12] * vOrigin.x + m_fMatrix[13] * vOrigin.y + m_fMatrix[14] * vOrigin.z + m_fMatrix[15];

	if (w < 0.01f)
		return false;

	float invw = 1.0f / w;
	vScreen.x *= invw;
	vScreen.y *= invw;


	float width = (float)clientWidth; //was clientwidth doesn't work with windowed
	float height = (float)clientHeight; //was clientheight doesn't work with windowed

	float x = width / 2;
	float y = height / 2;

	x += 0.5f * vScreen.x * width + 0.5f;
	y -= 0.5f * vScreen.y * height + 0.5f;

	vScreen.x = x;
	vScreen.y = y;
	return true;
}