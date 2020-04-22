#include <iostream>
#include "memory.h"
#include "w2s.h"

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

	//UpdateMatrix();

	for (size_t i = 0; i < 150; i++)
	{
		QWORD entitylist = drv->RPM<QWORD>(drv->base + OFFSET_ENTITYLIST);
		//std::cout << "entitylist " << entitylist << "\n";
		QWORD entity = drv->RPM<QWORD>(entitylist + ((QWORD)i << 5));
		//std::cout << "entity " << entity << "\n";

		QWORD local = get_local();
		//std::cout << "local " << local << "\n";

		if (entity == local)
			continue;

		//QWORD name = read<QWORD>(entity + OFFSET_NAME);
		//std::cout << "name " << name << "\n";
		if (!entity)  // "player.."
			continue;

		//QWORD name = drv->RPM<QWORD>(entity + OFFSET_NAME);
		//if (name != 125780153691248)  // "player.."
		//{
		//	continue;
		//}

		Vector3 vec;
		//Vector3 entity_pos = drv->RPM<Vector3>(entity + OFFSET_ORIGIN);
		//Vector3 entity_transformed;
		vec = drv->RPM<Vector3>(local + OFFSET_ORIGIN);
		std::cout << "vec is " << vec.x << " " << vec.y << " " << vec.z << "\n";
	}
}