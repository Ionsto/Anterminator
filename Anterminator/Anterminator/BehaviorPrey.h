#pragma once
#include "Entity.h"
namespace BehaviorPrey {
	std::default_random_engine generator(rand());
	std::uniform_real_distribution<float>  distr;
	void Update(Entity& e)
	{
		if (e.Alive) 
		{
			BehaviorRandomWalk::Update(e, 0.01, 10);
		}
	}
};
