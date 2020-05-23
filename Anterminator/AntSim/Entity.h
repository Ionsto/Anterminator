#pragma once
class World;
#include <glm/glm.hpp>
class Entity
{
public:
	//asking to be sweeped
	float Mass = 1;
	float Health = 5;
	float MovmentCost = 0.05;
	float DecayRate = 50;
	float MaxEnergy = 100;
	float Energy = 20;
	float FoodCarryAmount = 200;
	float Size = 1;
	bool Dead = false;
	bool DeadSpread = false;
	bool ToRemove = false;
	int Affiliation = 0;
	float Age = 0;
	float MaxAge = 60;
	float TimeFromHome = 0;
	float TimeFromFood = 0;
	bool QueenAnt = false;
	//Whether the ant is holding food currently
	float HeldFood = 0;
	float MaxSpeed = 100;
	glm::vec2 Position = glm::vec2(0,0);
	glm::vec2 OldPosition = glm::vec2(0,0);
	glm::vec2 Acceleration = glm::vec2(0,0);
	glm::vec2 Heading = glm::vec2(0,0);
	void Intergrate(World & world);
	void CheckBounds(World & world);
	void Update(World & world);
};

