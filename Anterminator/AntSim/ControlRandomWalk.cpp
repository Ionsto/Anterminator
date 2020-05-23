#include "ControlRandomWalk.h"
#include "World.h"
#include "Entity.h"
void ControlRandomWalk::Update(World& world, Entity& entity,float MixSetting)
{
	if ((rand() % 10001) / 10000.0 < RandomValue)
	{
		glm::vec2 Heading(distr(generator),distr(generator));
		entity.Heading += (Heading) * SpeedSetting * MixSetting;
	}
}
