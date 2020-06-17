#pragma once
#include <glm/glm.hpp>
class World;
class Entity
{
public:
	glm::vec2 Position = glm::vec2(0,0);
	glm::vec2 PositionOld = glm::vec2(0,0);
	glm::vec2 Acceleration = glm::vec2(0,0);
	glm::vec2 Heading = glm::vec2(0,0);
	glm::vec3 Colour = glm::vec3(1,1,1);
	float HeadingAcc = 50;
	bool ToRemove = false;
	bool Alive = true;
	bool PhysicsUpdate = true;
	float Health = 100;
	float Size = 1;
	float Mass = 1;
	float Drag = 0.00;
	float MaxEnergy = 100;
	float Energy = MaxEnergy;
	float MovmentCost = 0.01;
	float Age = 0;
	float MaxAge = 60;
	float DecayRate = 10;
	float TimeFromHome = 0;
	float TimeFromFood = 0;
	float WalkHomeTime = 20;
	float FoodCarryAmount = 200;
	bool QueenAnt = false;
	bool NoAging = false;
	float MaxSpeed = 5;
	float HeldFood = 0;
	bool Rendered = false;
	bool Attacked = false;
	bool UpdateTick = rand() % 1;
	int UpdateBehaviorMax = 5;
	int UpdateBehaviorCounter = rand()%UpdateBehaviorMax;
	//0 == ffa
	//>10 ant colonies
	int Affiliation = -1;
	enum class EntityType {
		Default,
		Ant,
		AntNest,
		Prey,
		Predator
	} Type = EntityType::Default;
	void SetVelocity(World& world, glm::vec2 velocity);
	void UpdateHeading(World& world);
	void Update(World & world);
	void Intergrate(World& world);
	void CheckBounds(World& world);
};

