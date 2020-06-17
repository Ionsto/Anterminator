#include "Entity.h"
#include "World.h"
void Entity::Update(World & world) {
	CheckBounds(world);
	if (Alive) {
		UpdateHeading(world);
	}
	HeldFood = std::clamp(HeldFood, 0.0f, FoodCarryAmount);
	Energy = std::clamp(Energy, 0.0f, MaxEnergy);
	if (PhysicsUpdate)
	{
		Intergrate(world);
	}
	if (NoAging)
	{
		Age = 0;
	}
	if (Alive)
	{
		Energy -= MovmentCost * HeadingAcc * glm::length(Heading) * World::DeltaTime * World::DeltaTime;
		Acceleration += Heading - (Position - PositionOld);
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
	CheckBounds(world);
	if (Age >= MaxAge || Health <= 0)
	{
		Alive = false;
		Energy += HeldFood;
		//Assumed latent energy
		//Energy += MaxEnergy * 0.1;
		HeldFood = 0;
	}
	if (Energy <= 0)
	{
		ToRemove = true;
		Alive = false;
	}
	if (!Alive) {
		Colour = glm::vec3(0.9, 0.9, 0.9);
	}
	if (Alive)
	{
		Attacked = false;
	}
}
void Entity::UpdateHeading(World& world)
{
	if (glm::dot(Heading, Heading) > MaxSpeed * MaxSpeed)
	{
		Heading = glm::normalize(Heading) * MaxSpeed;
	}
	Acceleration += Heading * HeadingAcc;
}
void Entity::Intergrate(World& world)
{
	auto Vel = (PositionOld - Position) / world.DeltaTime;
	Vel *= glm::length(Vel);
	Acceleration -= Vel * Drag;
	auto temppos = Position;
	static constexpr float Damping = 0.95;
	Position += ((Position - PositionOld) * Damping) + (world.DeltaTime * world.DeltaTime * Acceleration);
	PositionOld = temppos;
	Acceleration.x = 0;
	Acceleration.y = 0;
}
void Entity::CheckBounds(World& world)
{
	//Position = glm::clamp(Position, glm::vec2(-world.WorldSize, -world.WorldSize), glm::vec2(world.WorldSize, world.WorldSize));
	for(int i = 0;i < 2;i++)
	{
		if (Position[i] < 0)
		{
			Position[i] += World::WorldSize;
			PositionOld[i] += World::WorldSize;
		} 
		if (Position[i] >= World::WorldSize)
		{
			Position[i] -= World::WorldSize;
			PositionOld[i] -= World::WorldSize;
		} 
	}
}
void Entity::SetVelocity(World &world,glm::vec2 velocity)
{
	PositionOld = Position - (velocity * world.DeltaTime);
}
