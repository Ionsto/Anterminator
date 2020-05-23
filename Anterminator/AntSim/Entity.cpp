#include "Entity.h"
#include <glm/glm.hpp>
#include "World.h"
#include <algorithm>

void Entity::Update(World& world)
{
	float headingdamp = 0.99;
	Heading.x *= headingdamp;
	Heading.y *= headingdamp;
	HeldFood = std::clamp(HeldFood, 0.0f, FoodCarryAmount);
	Energy = std::clamp(Energy, 0.0f, MaxEnergy);
	if (glm::dot(Heading, Heading) > MaxSpeed * MaxSpeed)
	{
		Heading = glm::normalize(Heading) * MaxSpeed;
	}
	if (!Dead)
	{
		Energy -= MovmentCost * glm::length(Heading) * World::DeltaTime * World::DeltaTime;
		Acceleration += Heading - (Position - OldPosition);
		Age += World::DeltaTime;
		if (HeldFood < 0.5)
		{
			TimeFromHome += World::DeltaTime;
		}
		if (HeldFood > 0.5)
		{
			TimeFromFood += World::DeltaTime;
		}
	}
	else {
		Energy -= DecayRate * World::DeltaTime;
	}
	Intergrate(world);
	CheckBounds(world);
	if (Age > MaxAge || Health < 0)
	{
		Dead = true;
		Energy += HeldFood;
		HeldFood = 0;
	}
	/*if (Age > MaxAge && rand()% 10000 == 0)
	{
		std::cout << "Died from old age\n";
	}
	if (Health < 0 && rand()% 10000 == 0)
	{
		std::cout << "Died from health\n";
	}
	if (Energy <= 0 && rand()% 10000 == 0)
	{
		std::cout << "Died from energy loss\n";
	}*/
	if (Energy <= 0)
	{
		ToRemove = true;
		Dead = true;
	}
}
void Entity::Intergrate(World& world)
{
	auto temppos = Position;
	static constexpr float Damping = 0.95;
	Position += ((Position - OldPosition) * Damping) + (world.DeltaTime * world.DeltaTime * Acceleration);
	OldPosition = temppos;
	Acceleration.x = 0;
	Acceleration.y = 0;
}
void Entity::CheckBounds(World& world)
{
	Position = glm::clamp(Position, glm::vec2(-world.WorldSize, -world.WorldSize), glm::vec2(world.WorldSize, world.WorldSize));
}
