#pragma once
#include "driver.h"
#include "offsets.h"
#include "vector3.h"


extern Driver* drv;

QWORD get_local();
void UpdateMatrix();
void entity_loop();
QWORD findClosestToCrosshair();
void __stdcall thread();