#include "ColonyController.h"
#include <time.h>
#include "World.h"
#include <algorithm>
#include <iostream>
#include <fstream>
ColonyController::ColonyController() :
	random_number(-1, 1)
{
	generator.seed(rand() % 2134125419);
	Colour = glm::vec3(rand() % 255, rand() % 255, rand() % 255);
	this->behaviour_ph_killed.SpreadSize = 20;
	AntCreationEnergyFactor = 1;
	AntCreation = 10;
	Pheremone_Food.DecayRate = 2;
	Pheremone_Home.DecayRate = 1;
	Pheremone_Density.DecayRate = 10;
	Pheremone_Killed.DecayRate = 10;
	Pheremone_Food.DiffusionConstant = 0.1;
	Pheremone_Home.DiffusionConstant = 0.1;
	Pheremone_Density.DiffusionConstant = 1;
	Pheremone_Killed.DiffusionConstant = 80;
	WalkHomeTime = 10;
	WalkHomeRandom = 1200;
	WalkHomePHFollowHome = 1;
	WalkDiscoverRandom = 1200;
	WalkDiscoverPHLayHome = 1;
	WalkDiscoverPHFollowFood = 1;
	WalkDiscoverPHFollowHome = -0.1;
	WalkFoodPHLayFood = 2.5;
	WalkFoodRandom = 1000;
	WalkFoodPHFollowHome = 1;
	FindKilled = 10;
	FindDead = -1;
	WarnKilled = 100;
	WarnDead = 1;
	DensityLay = 0.1;
	DensityAvoid = -1;
	FollowFood = 0.02;
	DensityLay = 0.1;
	DensityAvoid = -10;
	//Randomise(1);
}
void ColonyController::Randomise(float random)
{
	auto map = {
	&AntCreationEnergyFactor                     ,
	&AntCreation                                 ,
	&Pheremone_Food.DecayRate                    ,
	&Pheremone_Home.DecayRate                    ,
	&Pheremone_Density.DecayRate                 ,
	&Pheremone_Killed.DecayRate                  ,
	&Pheremone_Food.DiffusionConstant            ,
	&Pheremone_Home.DiffusionConstant            ,
	&Pheremone_Density.DiffusionConstant         ,
	&Pheremone_Killed.DiffusionConstant          ,
	&WalkHomeTime                                ,
	&WalkHomeRandom                              ,
	&WalkHomePHFollowHome                        ,
	&WalkDiscoverRandom                          ,
	&WalkDiscoverPHLayHome                       ,
	&WalkDiscoverPHFollowFood                    ,
	&WalkDiscoverPHFollowHome                    ,
	&WalkFoodPHLayFood                           ,
	&WalkFoodRandom                              ,
	&WalkFoodPHFollowHome                        ,
	&FindKilled                                  ,
	&FindDead                                    ,
	&WarnKilled                                  ,
	&WarnDead                                    ,
	&DensityLay                                  ,
	&DensityAvoid                                ,
	&FollowFood                                  ,
	&DensityLay                                  ,
	&DensityAvoid                                
	};
	for (auto& m : map)
	{
		*m *= 1.0f + (random_number(generator)* random);
	}
};
void ColonyController::Update(World & world) {
	Pheremone_Home.Update(world.DeltaTime);
	Pheremone_Food.Update(world.DeltaTime);
	Pheremone_Density.Update(world.DeltaTime);
	Pheremone_Killed.Update(world.DeltaTime);
	TotalEnergy -= EnergyCost * world.DeltaTime;
	TotalEnergy = std::max(0.0f, TotalEnergy);
	float WarnKilled = 1;
	int SpawnCount = ((random_number(generator) + 1.0) / 2.0) * this->AntCreation * world.DeltaTime * std::pow(TotalEnergy / EnergyCost, AntCreationEnergyFactor);
	for (int i = 0; i < SpawnCount; ++i)
	{
		if (TotalEnergy > EnergyCost)
		{
			AddAnt(world);
		}
	}
}
void ColonyController::AddAnt(World & world) {
		Entity e;
		e.Position = HiveLocation;
		e.OldPosition = HiveLocation;
		e.Position += glm::vec2(world.random_number(world.generator), world.random_number(world.generator)) * 20.0f;
		e.OldPosition = e.Position;
//		e.OldPosition += (e.Position - HiveLocation) * world.DeltaTime * (0.9999f);
		e.OldPosition -= (e.Position - HiveLocation) * (1-0.9999f) * (0.01f/world.DeltaTime);
		e.Affiliation = Affiliation;
		e.MaxAge = 120;
		e.Energy = EnergyCost;
		if (world.AddEntity(e))
		{
			TotalEnergy -= EnergyCost;
		}
}
void ColonyController::Write(std::string outfile)
{
	auto map = {
	AntCreationEnergyFactor                     ,
	AntCreation                                 ,
	Pheremone_Food.DecayRate                    ,
	Pheremone_Home.DecayRate                    ,
	Pheremone_Density.DecayRate                 ,
	Pheremone_Killed.DecayRate                  ,
	Pheremone_Food.DiffusionConstant            ,
	Pheremone_Home.DiffusionConstant            ,
	Pheremone_Density.DiffusionConstant         ,
	Pheremone_Killed.DiffusionConstant          ,
	WalkHomeTime                                ,
	WalkHomeRandom                              ,
	WalkHomePHFollowHome                        ,
	WalkDiscoverRandom                          ,
	WalkDiscoverPHLayHome                       ,
	WalkDiscoverPHFollowFood                    ,
	WalkDiscoverPHFollowHome                    ,
	WalkFoodPHLayFood                           ,
	WalkFoodRandom                              ,
	WalkFoodPHFollowHome                        ,
	FindKilled                                  ,
	FindDead                                    ,
	WarnKilled                                  ,
	WarnDead                                    ,
	DensityLay                                  ,
	DensityAvoid                                ,
	FollowFood                                  ,
	DensityLay                                  ,
	DensityAvoid                                
	};
	std::cout<<"Save\n";
	std::ofstream file(outfile);
	if (file.is_open())
	{
		std::cout<<"Save\n";
		for (auto& m : map)
		{
			file << m << "\n";
		}
	}
}
void ColonyController::Read(std::string infile)
{

	auto map = {
	&AntCreationEnergyFactor                     ,
	&AntCreation                                 ,
	&Pheremone_Food.DecayRate                    ,
	&Pheremone_Home.DecayRate                    ,
	&Pheremone_Density.DecayRate                 ,
	&Pheremone_Killed.DecayRate                  ,
	&Pheremone_Food.DiffusionConstant            ,
	&Pheremone_Home.DiffusionConstant            ,
	&Pheremone_Density.DiffusionConstant         ,
	&Pheremone_Killed.DiffusionConstant          ,
	&WalkHomeTime                                ,
	&WalkHomeRandom                              ,
	&WalkHomePHFollowHome                        ,
	&WalkDiscoverRandom                          ,
	&WalkDiscoverPHLayHome                       ,
	&WalkDiscoverPHFollowFood                    ,
	&WalkDiscoverPHFollowHome                    ,
	&WalkFoodPHLayFood                           ,
	&WalkFoodRandom                              ,
	&WalkFoodPHFollowHome                        ,
	&FindKilled                                  ,
	&FindDead                                    ,
	&WarnKilled                                  ,
	&WarnDead                                    ,
	&DensityLay                                  ,
	&DensityAvoid                                ,
	&FollowFood                                  ,
	&DensityLay                                  ,
	&DensityAvoid                                
	};
	std::ifstream file(infile);
	if (file.is_open())
	{
		std::cout<<"Reading file\n";
		std::string line;
		float val;
		auto m = map.begin();
		while(file >> val)
		{
			**m = val;
			++m;
		}
	}
}
void ColonyController::Copy(ColonyController& source)
{

	auto map = {
	&AntCreationEnergyFactor                     ,
	&AntCreation                                 ,
	&Pheremone_Food.DecayRate                    ,
	&Pheremone_Home.DecayRate                    ,
	&Pheremone_Density.DecayRate                 ,
	&Pheremone_Killed.DecayRate                  ,
	&Pheremone_Food.DiffusionConstant            ,
	&Pheremone_Home.DiffusionConstant            ,
	&Pheremone_Density.DiffusionConstant         ,
	&Pheremone_Killed.DiffusionConstant          ,
	&WalkHomeTime                                ,
	&WalkHomeRandom                              ,
	&WalkHomePHFollowHome                        ,
	&WalkDiscoverRandom                          ,
	&WalkDiscoverPHLayHome                       ,
	&WalkDiscoverPHFollowFood                    ,
	&WalkDiscoverPHFollowHome                    ,
	&WalkFoodPHLayFood                           ,
	&WalkFoodRandom                              ,
	&WalkFoodPHFollowHome                        ,
	&FindKilled                                  ,
	&FindDead                                    ,
	&WarnKilled                                  ,
	&WarnDead                                    ,
	&DensityLay                                  ,
	&DensityAvoid                                ,
	&FollowFood                                  ,
	&DensityLay                                  ,
	&DensityAvoid                                
	};
	auto sourcemap = {
	&source.AntCreationEnergyFactor                     ,
	&source.AntCreation                                 ,
	&source.Pheremone_Food.DecayRate                    ,
	&source.Pheremone_Home.DecayRate                    ,
	&source.Pheremone_Density.DecayRate                 ,
	&source.Pheremone_Killed.DecayRate                  ,
	&source.Pheremone_Food.DiffusionConstant            ,
	&source.Pheremone_Home.DiffusionConstant            ,
	&source.Pheremone_Density.DiffusionConstant         ,
	&source.Pheremone_Killed.DiffusionConstant          ,
	&source.WalkHomeTime                                ,
	&source.WalkHomeRandom                              ,
	&source.WalkHomePHFollowHome                        ,
	&source.WalkDiscoverRandom                          ,
	&source.WalkDiscoverPHLayHome                       ,
	&source.WalkDiscoverPHFollowFood                    ,
	&source.WalkDiscoverPHFollowHome                    ,
	&source.WalkFoodPHLayFood                           ,
	&source.WalkFoodRandom                              ,
	&source.WalkFoodPHFollowHome                        ,
	&source.FindKilled                                  ,
	&source.FindDead                                    ,
	&source.WarnKilled                                  ,
	&source.WarnDead                                    ,
	&source.DensityLay                                  ,
	&source.DensityAvoid                                ,
	&source.FollowFood                                  ,
	&source.DensityLay                                  ,
	&source.DensityAvoid                                
	};
	auto mstart = map.begin();
	auto sstart = sourcemap.begin();
	for (int i = 0;i < map.size();++i)
	{
		**mstart = **sstart;
		++mstart;
		++sstart;
	}
}
