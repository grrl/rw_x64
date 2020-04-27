#include <iostream>
#include "memory.h"
#include "w2s.h"
#include "draw.h"
#include "maths.h"

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

QWORD GetEntityBoneArray(QWORD ent)
{
	return drv->RPM<QWORD>(ent + OFFSET_BONES);
}

Vector3 GetEntityBonePosition(QWORD ent, DWORD BoneId, Vector3 BasePosition)
{
	QWORD pBoneArray = GetEntityBoneArray(ent);

	Vector3 EntityHead;

	EntityHead.x = drv->RPM<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + BasePosition.x;
	EntityHead.y = drv->RPM<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + BasePosition.y;
	EntityHead.z = drv->RPM<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + BasePosition.z;

	return EntityHead;
}

Vector3 GetViewAngles(QWORD ent)
{
	return drv->RPM<Vector3>(ent + OFFSET_VIEWANGLES);
}

QAngle GetQangles(QWORD ent)
{
	return drv->RPM<QAngle>(ent + OFFSET_VIEWANGLES);
}

void SetViewAngles(QWORD ent, Vector3 angles)
{

	drv->WPM<float>(ent + OFFSET_VIEWANGLES, angles.x);
	drv->WPM<float>(ent + OFFSET_VIEWANGLES + 0x4, angles.y);
	drv->WPM<float>(ent + OFFSET_VIEWANGLES + 0x8, angles.z);
}

Vector3 GetCamPos(uintptr_t ent)
{
	return drv->RPM<Vector3>(ent + OFFSET_CAMERAPOS);
}

Vector3 GetEntityBasePosition(uintptr_t ent)
{
	return drv->RPM<Vector3>(ent + OFFSET_ORIGIN);
}
float smooth = 0.f;

void AimAtPos(float x, float y)
{
	//By fredaikis
	float ScreenCenterX = (clientWidth / 2);
	float ScreenCenterY = (clientHeight / 2);
	float TargetX = 0;
	float TargetY = 0;
	float AimSpeed = 1.0f;
	smooth = float_rand(0.f, 1.f);
	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}
	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}
	if (!smooth)
	{
		mouse_event(0x0001, (TargetX), (TargetY), NULL, NULL);
		return;
	}
	TargetX /= 10;
	TargetY /= 10;
	if (abs(TargetX) < 1)
	{
		if (TargetX > 0)
		{
			TargetX = 1;
		}
		if (TargetX < 0)
		{
			TargetX = -1;
		}
	}
	if (abs(TargetY) < 1)
	{
		if (TargetY > 0)
		{
			TargetY = 1;
		}
		if (TargetY < 0)
		{
			TargetY = -1;
		}
	}
	mouse_event(0x0001, TargetX, TargetY, NULL, NULL);
}

QWORD active_weapon(QWORD local)
{
	QWORD ActWeaponId = drv->RPM<QWORD>(local + OFFSET_WEAPON) & 0xFFFF;
	if (ActWeaponId)
	{
		QWORD entitylist = drv->base + OFFSET_ENTITYLIST;
		QWORD pweapon = drv->RPM<QWORD>(drv->base + entitylist + (ActWeaponId << 5));
		return pweapon;
	}
	return 0;
}

float bullet_speed(QWORD weapon)
{
	return drv->RPM<float>(weapon + OFFSET_BULLETSPEED);
}

float bullet_gravity(QWORD weapon)
{
	return drv->RPM<float>(weapon + OFFSET_BULLETGRAVITY);
}

Vector3 Velocity(QWORD Entity)
{
	return drv->RPM<Vector3>(Entity + OFFSET_ABSVELOCITY);
}

QWORD closest_entity() {

	Vector3 vec = { 0, 0, 0 };
	float max = 999.0f;
	QWORD aimentity = 0;
	for (int i = 0; i <= 150; i++) // TODO: Check if 1 is local
	{
		QWORD entitylist = drv->base + OFFSET_ENTITYLIST;

		QWORD entity = drv->RPM<QWORD>(entitylist + ((QWORD)i << 5));

		if (!entity)
			continue;
	
		int health = drv->RPM<int>(entity + OFFSET_HEALTH);

		if (health <= 0)
			continue;
		
		QWORD localent = get_local();

		if (localent == entity)
			continue;

		Vector3 LocalCamera = GetCamPos(localent);
		QAngle ViewAngles = GetQangles(localent);
		Vector3 FeetPosition = GetEntityBasePosition(entity);
		Vector3 HeadPosition = GetEntityBonePosition(entity, 8, FeetPosition);
		QAngle angle = CalcAngle(LocalCamera, HeadPosition);

		float fov = GetFov(ViewAngles, angle);

		if (fov < max)
		{
			max = fov;
			aimentity = entity;
		}
	}
	return aimentity;
}

QWORD findClosestToCrosshair() {


	float Finish;
	QWORD ClosestEntity = 0;
	Vector3 Calc = { 0 ,0 ,0 };
	float Closest = FLT_MAX;
	for (int i = 0; i < 150; i++)
	{
		QWORD entitylist = drv->base + OFFSET_ENTITYLIST;
		//std::cout << "entitylist " << entitylist << "\n";
		//if (!entitylist)
		//	continue;
		//std::cout << "entitylist " << entitylist << "\n";
		QWORD entity = drv->RPM<QWORD>(entitylist + ((QWORD)i << 5));
		//std::cout << "entity " << entity << "\n";

		if (!entity)
			continue;
		QWORD local = get_local();

		if (local == entity)
			continue;

		int enemyteam = drv->RPM<int>(entity + OFFSET_TEAM);
		int localTeam = drv->RPM<int>(local + OFFSET_TEAM);

		if (enemyteam == localTeam)
			continue;

		int health = drv->RPM<int>(entity + OFFSET_HEALTH);

		if (health > 0) {

			Vector3 FeetPosition = GetEntityBasePosition(entity);
			Vector3 HeadPosition = GetEntityBonePosition(entity, 8, FeetPosition);
			Vector3 head_transformed;

			if (!WorldToScreen(HeadPosition, head_transformed))
				continue;
			Vector3 center_screen;
			center_screen.x = clientWidth / 2;
			center_screen.y = clientHeight / 2;

			float x = head_transformed.x - clientWidth / 2;
			float y = head_transformed.y - clientHeight / 2;
			float distanceFromCrosshair = (float)sqrt((x * x) + (y * y));

			Finish = distanceFromCrosshair;

			if (Finish < Closest)
			{
				Closest = Finish;
				ClosestEntity = entity;
			}
		}

	}
	//std::cout << "closest from crosshair is " << Closest << "\n";
	return ClosestEntity;

}

void MouseEventAimbot(QWORD Entity) {

	//int EntNum = FindClosestEnemy();

	//DWORD Entity = GetEntity(EntNum);

	//8 head
	//7 neck
	//5 mid
	QWORD local = get_local();
	Vector3 FeetPosition = GetEntityBasePosition(Entity);
	Vector3 HeadPosition;
	int randomNumber = (rand() % 2) + 1;

	switch (randomNumber) {
	case 1:
		HeadPosition = GetEntityBonePosition(Entity, 8, FeetPosition);
		break;
	case 2:
		HeadPosition = GetEntityBonePosition(Entity, 7, FeetPosition);
		break;
	default:
		HeadPosition = GetEntityBonePosition(Entity, 8, FeetPosition);
	}

	HeadPosition.x += float_rand(-0.025, 0.025);
	HeadPosition.y += float_rand(-0.025, 0.025);
	HeadPosition.z += float_rand(-0.025, 0.025);


	QWORD active = active_weapon(local);
	bool velocity = true;
	bool gravity = true;

	if (active)
	{
		float BulletSpeed = bullet_speed(active);
		float BulletGrav = bullet_gravity(active);

		if (BulletSpeed > 1.f)
		{
			Vector3 localfeet = GetEntityBasePosition(local);
			Vector3 muzzle = GetEntityBonePosition(local, 8, FeetPosition);

			if (gravity)
			{
				float VerticalTime = Dist3D(HeadPosition, muzzle) / BulletSpeed;
				HeadPosition.z += (750.f * BulletGrav * 0.5f) * (VerticalTime * VerticalTime);
				std::cout << "muzzle " << muzzle.x << " " << muzzle.y << " " << muzzle.z << " vertical time " << VerticalTime << " bullet speed " << BulletSpeed << " bullet grav " << BulletGrav << "\n";
			}
			if (velocity)
			{
				float HorizontalTime = Dist3D(HeadPosition, muzzle) / BulletSpeed;
				HeadPosition.x += (Velocity(Entity).x * HorizontalTime);
				HeadPosition.y += (Velocity(Entity).y * HorizontalTime);
			}

		}
	}

	Vector3 head_transformed;
	if (!WorldToScreen(HeadPosition, head_transformed))
		return;

	Vector3 center_screen;
	center_screen.x = clientWidth / 2;
	center_screen.y = clientHeight / 2;
	Vector3 Aimpos;
	Aimpos.x = head_transformed.x;
	Aimpos.y = head_transformed.y;

	float radiusx = (fov) * (center_screen.x / 100.0f);
	float radiusy = (fov) * (center_screen.y / 100.0f);

	if (Aimpos.x >= center_screen.x - radiusx && Aimpos.x <= center_screen.x + radiusx && Aimpos.y >= center_screen.y - radiusy && Aimpos.y <= center_screen.y + radiusy) {
		//std::cout << "aiming" << "\n";
		if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
			AimAtPos(Aimpos.x, Aimpos.y);

	}

}

void entity_loop() {

	QWORD closest = findClosestToCrosshair();
	for (size_t i = 0; i < 150; i++)
	{
		QWORD entitylist = drv->base + OFFSET_ENTITYLIST;
		//std::cout << "entitylist " << entitylist << "\n";
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

		float health = GetEntityHealth(entity);

		if (health <= 0)
			continue;

		Vector3 vect, entity_transformed, entityhead_transformed;
		Vector3 entity_pos = drv->RPM<Vector3>(entity + OFFSET_ORIGIN);

		if (&entity_pos == nullptr)
			continue;

		float dist = VectorDistance(vec, entity_pos);

		if (dist >= 400.f)
			continue;

		Vector3 entityhead = entity_pos;
		entityhead.z += 64.0626 + 9;
		//std::cout << "dist " << dist << "\n";
		//std::cout << "entitypos " << entity_pos.x << " " << entity_pos.y << " " << entity_pos.z << "\n";
		//Vector3 entity_transformed;
		//vec = drv->RPM<Vector3>(local + OFFSET_ORIGIN);
		//std::cout << "vec is " << vec.x << " " << vec.y << " " << vec.z << "\n";
		if (!WorldToScreen(entity_pos, entity_transformed))
			continue;

		if (!WorldToScreen(entityhead, entityhead_transformed))
			continue;

		if (closest == entity) {
			//std::cout << "same " << closest << "\n";
			MouseEventAimbot(closest);
		}

		float Height = entityhead_transformed.y - entity_transformed.y;
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

		if (dist <= 200.f)
			DrawLine(clientWidth / 2, clientHeight / 2, entity_transformed.x, entityhead_transformed.y, 57, 255, 20, 255);

		DrawLine(entity_transformed.x - Height / 4, entityhead_transformed.y, entity_transformed.x - Height / 4, entityhead_transformed.y - Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x - Height / 4, entity_transformed.y, entity_transformed.x - Height / 4, entity_transformed.y + Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawLine(entity_transformed.x + Height / 4, entityhead_transformed.y, entity_transformed.x + Height / 4, entityhead_transformed.y - Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x + Height / 4, entity_transformed.y, entity_transformed.x + Height / 4, entity_transformed.y + Height / 5, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawLine(entity_transformed.x - Height / 4, entityhead_transformed.y, entity_transformed.x - Height / 16, entityhead_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x + Height / 4, entityhead_transformed.y, entity_transformed.x + Height / 16, entityhead_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		DrawLine(entity_transformed.x - Height / 4, entity_transformed.y, entity_transformed.x - Height / 16, entity_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);
		DrawLine(entity_transformed.x + Height / 4, entity_transformed.y, entity_transformed.x + Height / 16, entity_transformed.y, rEn * 255, gEn * 255, bEn * 255, aEn * 255);

		char buffer[5];
		char buffer2[4];
		//double dist2 = dist >= 0. ? floor(dist*100.) / 100. : ceil(dist*100.) / 100.;
		int ret = snprintf(buffer, sizeof buffer, "%f", Round(dist));
		int ret2 = snprintf(buffer2, sizeof buffer2, "%f", health);
		//printf("%d\n", dist);
		char printChar1[2] = "[";
		char printChar2[4] = "m] ";
		char result[15];   // array to hold the result.

		strcpy(result, printChar1); // copy string one into the result.
		strcat(result, buffer); // append string two to the result.
		strcat(result, printChar2); // append string two to the result.
		strcat(result, buffer2); // append string two to the result.
		DrawShadowString(result, entity_transformed.x, entity_transformed.y, 255, 255, 255, dx_FontCalibri);
	}
}


void __stdcall thread() {

	while (true) {

		/*
		QWORD localent = get_local();

		if (!localent)
			continue;

		QWORD aimentity = closest_entity();

		if (!aimentity)
			continue;
		Vector3 LocalCamera = GetCamPos(localent);
		Vector3 ViewAngles = GetViewAngles(localent);
		Vector3 FeetPosition = GetEntityBasePosition(aimentity);
		Vector3 HeadPosition = GetEntityBonePosition(aimentity, 8, FeetPosition);
		QAngle angle = CalcAngle(LocalCamera, HeadPosition);
		//Math::ClampAngles(angle);
		Vector3 CalculatedAngles = *(Vector3*)(&(angle));

		Vector3 Delta = (CalculatedAngles - ViewAngles); // / SMOOTH;

		//if (s_Smooth != 0)
		//{
		//	Delta = (CalculatedAngles - ViewAngles) / s_Smooth;
		//}

		Vector3 puredelta = CalculatedAngles - ViewAngles;
		Vector3 SmoothedAngles = ViewAngles + Delta;

		//if (s_Recoil)
		//{
		//	Vector RecoilVec = driver::read<Vector>(g_Sock, g_PID, localent + OFFSET_AIMPUNCH);
		//	if (RecoilVec.x != 0 || RecoilVec.y != 0)
		//	{
		//		SmoothedAngles -= RecoilVec;
		//	}
		//}

		if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
			SetViewAngles(localent, SmoothedAngles);
		*/

		QWORD closest = findClosestToCrosshair();

		if (closest && closest != 0)
			MouseEventAimbot(closest);
	}
}