#pragma once
#include <array>
#include <vector>
#include "ControlRandomWalk.h"
#include "ControlFollowPheremone.h"
#include "ControlHillClimbPheremone.h"
#include "ControlPlacePheremone.h"
class ColonyController
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float>  random_number;

	float AggressionControl = 0;
	//Random movement
	float DiscoveryControl = 0;
	//Following pheremone lines
	float FollowControl = 0;
	//When having food when to lay pheremone
	float PheremoneLaying = 0;
	//How frequently an ant needs to be created
	float AntCreation = 0.1;
	float AntCreationEnergyFactor = 1;
	//Queen creation
	float QueenCreation = 0;
	//Hive creation
	float HiveCreation = 0;
	static constexpr int GridSize = 1000;
	static constexpr float WorldSize = 1000;
	float EnergyCost = 50;
	float EnergyUsage = 10;
public:
	bool ColonyAlive = true;
	//Attacking non affilate entities
	int Affiliation = 0;

	float WalkHomeTime = 10;
	float WalkHomeRandom = 1200;
	float WalkHomePHFollowHome = 1;

	float WalkDiscoverRandom = 1200;
	float WalkDiscoverPHLayHome = 1;
	float WalkDiscoverPHFollowFood = 1;
	float WalkDiscoverPHFollowHome = -0.1;

	float WalkFoodPHLayFood = 2.5;
	float WalkFoodRandom = 1000;
	float WalkFoodPHFollowHome = 1;

	float FindKilled = 10;
	float FindDead = -1;
	float WarnKilled = 100;
	float WarnDead = 1;
	float DensityLay = 0.1;
	float DensityAvoid = -1;
	float FollowFood = 10;

	glm::vec2 HiveLocation = glm::vec2(0,0);
	glm::vec3 Colour = glm::vec3(0, 0, 0);
	float TotalEnergy = EnergyCost*500;
	PheremoneGrid Pheremone_Home;
	PheremoneGrid Pheremone_Food;
	PheremoneGrid Pheremone_Killed;
	PheremoneGrid Pheremone_Density;
	ControlRandomWalk behaviour_walk;
	ControlFollowPheremone behaviour_home;
	ControlFollowPheremone behaviour_food;
//	ControlFollowPheremone behaviour_food;
	ControlHillClimbPheremone behaviour_followkilled;
	ControlHillClimbPheremone behaviour_density;
	ControlHillClimbPheremone behaviour_followfood;
	ControlPlacePheremone behaviour_ph_food;
	ControlPlacePheremone behaviour_ph_home;
	ControlPlacePheremone behaviour_ph_density;
	ControlPlacePheremone behaviour_ph_killed;
	ColonyController();
	void Randomise(float randomfactor);
	void AddAnt(World & world);
	void Update(World & world);
	void Write(std::string outfile);
	void Read(std::string infile);
	void Copy(ColonyController& colony);
};

