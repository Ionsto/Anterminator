#pragma once
#include "Entity.h"
#include <glm/glm.hpp>
#include <random>
namespace BehaviorRandomWalk {
	std::default_random_engine generator(rand());
	std::uniform_real_distribution<float>  distr;
	void Update(Entity & entity,float RandomValue,float SpeedSetting) {
		if (distr(generator) < RandomValue)
		{
			glm::vec2 Heading(distr(generator), distr(generator));
			Heading -= 0.5f;
			entity.Heading += (Heading) * SpeedSetting;
		}
	}
}
