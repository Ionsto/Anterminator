#include "AntColonyController.h"
#include "World.h"
#include "Entity.h"

AntColonyController::AntColonyController() {
	generator.seed(rand());
	Colour = glm::vec3(distr(generator),distr(generator),distr(generator));
//	AntCreationEnergyFactor = 1;
//	AntCreation = 10;
	PheremoneFood.DecayRate = 2;
	PheremoneHome.DecayRate = 0.1;
	PheremoneDensity.DecayRate = 1;
	PheremoneKilled.DecayRate = 10;
	PheremoneAttack.DecayRate = 2;
//	Pheremone_Killed.DecayRate = 10;
	PheremoneFood.DiffusionConstant = 1;
	PheremoneHome.DiffusionConstant = 1;
	PheremoneDensity.DiffusionConstant = 5;
	PheremoneKilled.DiffusionConstant = 10;
	PheremoneAttack.DiffusionConstant = 3;
//	Pheremone_Killed.DiffusionConstant = 80;
	WalkHomeTime = 1;
	WalkHomeRandom = 0.1;
	WalkHomeRandomSpeed = 10;
	WalkHomePHFollowHome = 1;
	WalkDiscoverRandom = 0.1;
	WalkDiscoverRandomSpeed = 10;
	WalkDiscoverPHLayHome = 1;
	WalkDiscoverPHFollowFood = 1;
	WalkDiscoverPHFollowHome = -0.1;
	WalkFoodPHLayFood = 2.5;
	WalkFoodRandom = 0.1;
	WalkFoodRandomSpeed = 10;
	WalkFoodPHFollowHome = 1;
	WarnKilled = 10;
	FindKilled = 5;
	WarnDead = 1;
	FindDead = -1;
	FollowFood = 0.02;
	DensityLay = 0.1;
	DensityAvoid = -10;
	AntSpawnRate = 1;
	ColonySpawnRate = 0.001;
	ColonySpawnRateError = 2;
	AttackDensityMult = 1;
	AttackRepropogateMult = 0.1;
	AttackFollow = 1;
	Randomise(1);
}
#pragma optimize("", off)
void AntColonyController::Randomise(float random)
{
	auto map = GetAttrMap();
	for (auto& m : map)
	{
		*m *= 0.5 + (1.5 * (distr(generator)* random));
	}
}
#pragma optimize("", on)
Entity&& AntColonyController::CreateAnt(World& w)
{
	Entity e;
	e.Type = Entity::EntityType::Ant;
	e.Colour = Colour;
	e.Size = 0.5;
	e.Health = 5;
	e.MaxAge = 50;
	e.Affiliation = Affiliation;
	e.PhysicsUpdate = true;
	return std::move(e);
}
bool AntColonyController::AddAnt(World& w, Entity&& e)
{
	return w.AddEntity(std::move(e));
}
Entity && AntColonyController::CreateColony(World& w)
{
	Entity e;
	e.Type = Entity::EntityType::AntNest;
	e.Size = 40;
	e.NoAging = true;
	e.Health = 100;
	e.MaxEnergy = 100'000;
	e.Energy = e.MaxEnergy;
	e.MaxAge = 120;
	e.Mass = 1;
	e.PhysicsUpdate = false;
	e.Colour = Colour;
	e.Affiliation = Affiliation;
	//e.MaxAge = 1000;
	return std::move(e);
}
bool AntColonyController::AddColony(World& w, glm::vec2 pos)
{
	auto e = CreateColony(w);
	e.Position = pos;
	e.PositionOld = e.Position;
	return w.AddEntity(std::move(e));
}
bool AntColonyController::AddColonyQueen(World& w, Entity& queen)
{
	queen.ToRemove = true;
	queen.Alive = false;
	return AddColony(w, queen.Position);
}
void AntColonyController::Update(World& world)
{
	Alive = AntCount + ColonyCount > 0;
	if (Alive)
	{
		PheremoneHome.Update(world.DeltaTime);
		PheremoneFood.Update(world.DeltaTime);
		PheremoneDensity.Update(world.DeltaTime);
		PheremoneKilled.Update(world.DeltaTime);
		PheremoneAttack.Update(world.DeltaTime);
	}
}
