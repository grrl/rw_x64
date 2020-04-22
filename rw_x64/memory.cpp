#include <iostream>
#include "memory.h"
#include "w2s.h"
#include "draw.h"
QWORD get_local() {
	return drv->RPM<QWORD>(drv->base + OFFSET_LOCAL_ENT);
}

void UpdateMatrix() {
	QWORD viewrender = drv->RPM<QWORD>(drv->base + OFFSET_RENDER);
	QWORD matrixpointer2 = drv->RPM<QWORD>(viewrender + OFFSET_MATRIX);
	for (int v = 0; v < 16; v++)
		m_fMatrix[v] = drv->RPM<float>(matrixpointer2 + (v * 0x4));
}

void entity_loop() {

	UpdateMatrix();

	//std::cout << "matrix " << m_fMatrix[0] << "\n";

	for (size_t i = 0; i < 150; i++)
	{
		uint64_t entitylist = drv->base + OFFSET_ENTITYLIST;
		std::cout << "entitylist " << entitylist << "\n";
		//if (!entitylist)
		//	continue;
		//std::cout << "entitylist " << entitylist << "\n";
		uint64_t entity = drv->RPM<uint64_t>(entitylist + ((uint64_t)i << 5));
		//std::cout << "entity " << entity << "\n";

		uint64_t local = get_local();
		//std::cout << "local " << local << "\n";
		Vector3 vec = drv->RPM<Vector3>(local + OFFSET_ORIGIN);
		std::cout << "local is " << vec.x << " " << vec.y << " " << vec.z << "\n";

		if (entity == local)
			continue;

		if (!entity || entity == 0)  // "player.."
			continue;
		/*
		QWORD name = drv->RPM<QWORD>(entity + OFFSET_NAME);
		std::cout << "name " << name << "\n";
		if (name != 125780153691248)  // "player.."
		{
			continue;
		}
		*/
		Vector3 vect, entity_transformed;
		std::cout << "entity " << entity << "\n";
		Vector3 entity_pos = drv->RPM<Vector3>(entity + OFFSET_ORIGIN);

		if (&entity_pos == nullptr)
			continue;

		std::cout << "entitypos " << entity_pos.x << " " << entity_pos.y << " " << entity_pos.z << "\n";
		//Vector3 entity_transformed;
		//vec = drv->RPM<Vector3>(local + OFFSET_ORIGIN);
		//std::cout << "vec is " << vec.x << " " << vec.y << " " << vec.z << "\n";
		if (!WorldToScreen(entity_pos, entity_transformed))
			continue;

		DrawShadowString((char*)"enemy", entity_transformed.x, entity_transformed.y, 255, 0, 255, dx_FontCalibri);
	}
}