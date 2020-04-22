#include <Windows.h>
#include <iostream>
#include "driver.h"
#include "offsets.h"
#include "vector3.h"

Driver* drv;


QWORD get_local() {
	return drv->RPM<QWORD>(drv->base + OFFSET_LOCAL_ENT);
}
void entity_loop() {

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

		Vector3 vec;

		vec.x = drv->RPM<float>(local + OFFSET_ORIGIN);
		vec.y = drv->RPM<float>(local + OFFSET_ORIGIN + 0x4);
		vec.z = drv->RPM<float>(local + OFFSET_ORIGIN + 0x8);

		std::cout << "x " << vec.x << " y " << vec.y << " " << vec.z << "\n";
	}

}
int main() {

	drv = new Driver;

	drv->hDriver = CreateFileA("\\\\.\\neet", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	std::cout << "hdriver " << drv->hDriver << "\n";
	// Get address of client.dll & pid of csgo from our driver
	drv->ProcessId = drv->GetTargetPid();
	drv->base = drv->GetClientModule();

	std::cout << "processid " << drv->ProcessId << "\n";
	std::cout << "ClientAddress " << drv->base << "\n";

	while (true)
	{
		entity_loop();

	}

	return 0;
}