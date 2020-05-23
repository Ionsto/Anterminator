#pragma once
#include <random>
#include <time.h>
class World;
class Entity;
class ControlRandomWalk
{

	std::default_random_engine generator;
	std::uniform_real_distribution<float>  distr;
	float RandomValue = 0.1;
	float SpeedSetting = 0.1;
public:
	ControlRandomWalk() : distr(-1.0, 1.0) {
		generator.seed(rand()%123123);
	};
	void Update(World& world, Entity& entity,float Mix);
};

