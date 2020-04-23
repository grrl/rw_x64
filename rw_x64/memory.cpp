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

int GetEntityHealth(QWORD Entity)
{
	return drv->RPM<int>(Entity + OFFSET_HEALTH);
}

int GetEntityArmor(QWORD Entity) {
	return drv->RPM<int>(Entity + OFFSET_SHIELD);
}

float VectorDistance(Vector3 v1, Vector3 v2)
{
	return (float)sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2)) * 0.0254f;

}

void entity_loop() {

	UpdateMatrix();

	//std::cout << "matrix " << m_fMatrix[0] << "\n";

	for (size_t i = 0; i < 150; i++)
	{
		QWORD entitylist = drv->base + OFFSET_ENTITYLIST;
		std::cout << "entitylist " << entitylist << "\n";
		//if (!entitylist)
		//	continue;
		//std::cout << "entitylist " << entitylist << "\n";
		QWORD entity = drv->RPM<QWORD>(entitylist + ((QWORD)i << 5));
		//std::cout << "entity " << entity << "\n";

		QWORD local = get_local();

		int enemyteam = drv->RPM<int>(entity + OFFSET_TEAM);
		int localTeam = drv->RPM<int>(local + OFFSET_TEAM);

		if (enemyteam == localTeam)
			continue;

		//std::cout << "local " << local << "\n";
		Vector3 vec = drv->RPM<Vector3>(local + OFFSET_ORIGIN);

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
		Vector3 vect, entity_transformed, entityhead_transformed;
		Vector3 entity_pos = drv->RPM<Vector3>(entity + OFFSET_ORIGIN);

		if (&entity_pos == nullptr)
			continue;

		int dist = VectorDistance(vec, entity_pos);

		if (dist >= 400)
			continue;

		Vector3 entityhead = entity_pos;
		entityhead.z += 64.0626 + 9;
		std::cout << "entitypos " << entity_pos.x << " " << entity_pos.y << " " << entity_pos.z << "\n";
		//Vector3 entity_transformed;
		//vec = drv->RPM<Vector3>(local + OFFSET_ORIGIN);
		//std::cout << "vec is " << vec.x << " " << vec.y << " " << vec.z << "\n";
		if (!WorldToScreen(entity_pos, entity_transformed))
			continue;

		if (!WorldToScreen(entityhead, entityhead_transformed))
			continue;

		float Height = entityhead_transformed.y - entity_transformed.y;
		float health = GetEntityHealth(entity);
		float delta = (Height / 100.0f) * health;

		if (health > 75)
			DrawHealthBar((entity_transformed.x - Height / 4) + 3, entity_transformed.y, 3, delta, 57, 255, 20, 255);
		else if (health > 50)
			DrawHealthBar((entity_transformed.x - Height / 4) + 3, entity_transformed.y, 3, delta, 255, 255, 0, 255);
		else if (health > 25)
			DrawHealthBar((entity_transformed.x - Height / 4) + 3, entity_transformed.y, 3, delta, 255, 102, 0, 255);
		else
			DrawHealthBar((entity_transformed.x - Height / 4) + 3, entity_transformed.y, 3, delta, 255, 0, 0, 255);

		float armor = GetEntityArmor(entity);
		float armordelta = (Height / 100.0f) * armor;
		if (armor > 0)
			DrawHealthBar((entity_transformed.x - Height / 4) + 6, entity_transformed.y, 3, armordelta, 77, 149, 255, 255/*102, 255, 255*/);

		float rEn = 1;
		float gEn = 0;
		float bEn = 0;
		float aEn = 1;
		DrawLine(entity_transformed.x - Height / 4, entityhead_transformed.y, entity_transformed.x - Height / 4, entityhead_transformed.y - Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x - Height / 4, entity_transformed.y, entity_transformed.x - Height / 4, entity_transformed.y + Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawLine(entity_transformed.x + Height / 4, entityhead_transformed.y, entity_transformed.x + Height / 4, entityhead_transformed.y - Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x + Height / 4, entity_transformed.y, entity_transformed.x + Height / 4, entity_transformed.y + Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawLine(entity_transformed.x - Height / 4, entityhead_transformed.y, entity_transformed.x - Height / 16, entityhead_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x + Height / 4, entityhead_transformed.y, entity_transformed.x + Height / 16, entityhead_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawLine(entity_transformed.x - Height / 4, entity_transformed.y, entity_transformed.x - Height / 16, entity_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x + Height / 4, entity_transformed.y, entity_transformed.x + Height / 16, entity_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawShadowString((char*)"enemy", entity_transformed.x, entity_transformed.y, 255, 255, 255, dx_FontCalibri);
	}
}

void __stdcall thread() {

	while (true) {

	}
}