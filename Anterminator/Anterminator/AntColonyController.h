#pragma once
#include <glm/glm.hpp>
#include <random>
#include <ctime>
#include "PheremoneGrid.h"
#include <vector>
class World;
class Entity;
class AntColonyController
{
public:
	int AntCount = 0;
	int ColonyCount = 0;
	bool Alive = true;
	float AntSpawnRate = 1;
	float ColonySpawnRate = 0.00001;
	float ColonySpawnRateError = 2;
	float MinColonyEnergy = 1000;
	float ColonyStartEnergy = 10'000;
	PheremoneGrid PheremoneHome;
	PheremoneGrid PheremoneFood;
	PheremoneGrid PheremoneDensity;
	PheremoneGrid PheremoneKilled;
	PheremoneGrid PheremoneAttack;
	std::default_random_engine generator;
	std::uniform_real_distribution<float>  distr;
	int Affiliation = 0;
	float WalkHomeTime = 10;
	float WalkHomeRandom = 0.1;
	float WalkHomeRandomSpeed = 10;
	float WalkHomePHFollowHome = 1;
	float WalkDiscoverRandom = 0.1;
	float WalkDiscoverRandomSpeed = 12;
	float WalkDiscoverPHLayHome = 1;
	float WalkDiscoverPHFollowFood = 1;
	float WalkDiscoverPHFollowHome = -0.1;
	float WalkFoodPHLayFood = 2.5;
	float WalkFoodRandom = 0.1;
	float WalkFoodRandomSpeed = 1000;
	float WalkFoodPHFollowHome = 1;
	float FindKilled = 10;
	float FindDead = -1;
	float WarnKilled = 20;
	float WarnDead = 1;
	float DensityLay = 0.1;
	float DensityAvoid = -1;
	float AttackDensityMult = 0;
	float AttackRepropogateMult = 0;
	float AttackFollow = 0;
	float FollowFood = 10;
	glm::vec3 Colour = glm::vec3(0,0,0);

	AntColonyController();
	void Randomise(float random);
	Entity&& CreateAnt(World& w);
	Entity&& CreateColony(World& w);
	bool AddAnt(World & w, Entity&& e);
	bool AddColony(World& w,glm::vec2 pos);
	bool AddColonyQueen(World& w, Entity& queen);
	void Update(World & w);
	auto  GetAttrMap()
	{
		std::vector<float*> map = {
		&AntSpawnRate								 ,	
		&ColonySpawnRate						     ,
		&ColonySpawnRateError						 ,
		&PheremoneFood.DecayRate                     ,
		&PheremoneHome.DecayRate                     ,
		&PheremoneDensity.DecayRate                  ,
		&PheremoneKilled.DecayRate                   ,
		&PheremoneAttack.DecayRate                   ,
		&PheremoneFood.DiffusionConstant             ,
		&PheremoneHome.DiffusionConstant             ,
		&PheremoneDensity.DiffusionConstant          ,
		&PheremoneAttack.DiffusionConstant           ,
		&PheremoneKilled.DiffusionConstant           ,
		&WalkHomeTime                                ,
		&WalkHomeRandom                              ,
		&WalkHomeRandomSpeed                         ,
		&WalkHomePHFollowHome                        ,
		&WalkDiscoverRandom                          ,
		&WalkDiscoverRandomSpeed                     ,
		&WalkDiscoverPHLayHome                       ,
		&WalkDiscoverPHFollowFood                    ,
		&WalkDiscoverPHFollowHome                    ,
		&WalkFoodPHLayFood                           ,
		&WalkFoodRandom                              ,
		&WalkFoodRandomSpeed                         ,
		&WalkFoodPHFollowHome                        ,
		&FindKilled                                  ,
		&FindDead                                    ,
		&WarnKilled                                  ,
		&WarnDead                                    ,
		&DensityLay                                  ,
		&DensityAvoid                                ,
		&FollowFood                                  ,      
		&AttackDensityMult	                         ,
		&AttackRepropogateMult						 ,
		&AttackFollow
		};
		return map;
	}
	void Copy(AntColonyController& source)
	{
		auto map = GetAttrMap();
		auto sourcemap = source.GetAttrMap();
		auto mstart = map.begin();
		auto sstart = sourcemap.begin();
		for (int i = 0;i < map.size();++i)
		{
			**mstart = **sstart;
			++mstart;
			++sstart;
		}
	}
};

