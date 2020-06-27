#include "World.h"
#include "Behaviour.h"
#include <ctime>

World::World():random_number(-1,1)
{
	generator.seed(time(NULL));
	for (int i = 0; i < AntFactionCount; ++i)
	{
		FactionArray[i].Affiliation = i;
		FactionArray[i].Alive = false;
	}
	for (int i = 0; i < 10; ++i)
	{
		FactionArray[i].AddColony(*this,glm::vec2(random_number(generator),random_number(generator)) * WorldSize);
	}
	PreySpawnCount = 1000;
	for (int i = 0; i < static_cast<int>((WorldSize * WorldSize) / 500'000); ++i) {
		Entity e;
		e.Type = Entity::EntityType::Prey;
		e.Position = (glm::vec2(random_number(generator), random_number(generator)) * WorldSize);
		e.PositionOld = e.Position;
		e.Colour = glm::vec3(1, 0, 0);
		e.Size = 20;
		e.Health = 5;
		e.Mass = 100;
		e.MaxSpeed = 1;
		e.MaxEnergy = 100'000;
		e.Energy = e.MaxEnergy;
		AddEntity(std::move(e));
	}
	for (int i = 0; i < EntityList.MaxElementsPerElement; ++i) {
		Entity e;
		e.Position = (glm::vec2(random_number(generator), random_number(generator)) * WorldSize);
		e.PositionOld = e.Position;
//		AddEntity(std::move(e));
	}
}
void World::Update(float realdt)
{
	//DtAccumulator += 0.01;
	TimeScalingFactor = std::clamp(TimeScalingFactor, 1.0f / (1 << 5), (float)(1 << 0));
	DtAccumulator += realdt * TimeScalingFactor;
	int dtstepcount = static_cast<int>(DtAccumulator / this->DeltaTime);//std::min(100, static_cast<int>(DtAccumulator / this->DeltaTime));
	DtAccumulator -= this->DeltaTime * static_cast<float>(dtstepcount);
	//DtAccumulator = std::min(1.0f, DtAccumulator);
	for (int i = 0; i < dtstepcount;++i)
	{
		UpdateColonies();
		UpdateEntities();
		UpdateSpacialHash();
		UpdateCollision();
		UpdateDeadEntities();
		AddPrey();
	}
}

void World::AddPrey()
{
	PreySpawnCount += DeltaTime;
	if (PreySpawnCount > 0.8)
	{
		static constexpr float AreaPerSpawn = 10'000'000;
		for (int i = 0; i < static_cast<int>((WorldSize * WorldSize) / AreaPerSpawn); ++i) {
			Entity e;
			e.Type = Entity::EntityType::Prey;
			e.Position = (glm::vec2(random_number(generator), random_number(generator)) * WorldSize);
			e.PositionOld = e.Position;
			e.Colour = glm::vec3(1, 0, 0);
			e.Size = 5 * (1+(powf(random_number(generator),4)*30));
			e.Mass = 10 * e.Size * e.Size;
			e.Health = 5;
			e.MaxSpeed = 10 / e.Size;
			e.MaxEnergy = 100 * e.Size * e.Size;
			e.Energy = e.MaxEnergy;
			e.Affiliation = -1;
			//e.Heading = glm::vec2(random_number(generator), random_number(generator)) * 100.0f;
			AddEntity(std::move(e));
			PreySpawnCount = 0;
		}
	}
}
bool World::AddEntity(Entity&& entity)
{
	if (entity.Affiliation >= 0 && entity.Affiliation < AntFactionCount)
	{
		if (entity.Type == Entity::EntityType::Ant)
		{
			FactionArray[entity.Affiliation].AntCount += 1;
		}
		if (entity.Type == Entity::EntityType::AntNest)
		{
			FactionArray[entity.Affiliation].ColonyCount += 1;
		}
	}
	return EntityList.AddElement(std::move(entity)) != -1;
}
void World::UpdateColonies() 
{
#pragma omp parallel for
	for (int i = 0; i < AntFactionCount; ++i) 
	{
		FactionArray[i].Update(*this);
	}
}
void World::UpdateSpacialHash()
{
#pragma omp parallel for
	for (int i = 0; i < Chunks.ChunkCount * Chunks.ChunkCount; ++i)
	{
		Chunks.raw_data[i].EntityCount = 0;
	}
#pragma omp parallel for
	for (int i = 0; i < EntityList.ElementCount; ++i)
	{
		auto& e = EntityList.GetElement(i);
		auto hashchunk = [=](float dx,float dy) {
			auto& c = Chunks.GetChunkPos(e.Position.x + dx, e.Position.y + dy);
			if (c.EntityCount < c.MaxEntities)
			{
				c.EntityIDList[c.EntityCount++] = i;
			}
			else 
			{
				std::cout << "Spacial hash ran out of space\n";
			}
		};
		/*auto& c = Chunks.GetChunkPos(e.Position.x,e.Position.y);
		if (c.EntityCount < c.MaxEntities)
		{
			c.EntityIDList[c.EntityCount++] = i;
		}
		else {
			std::cout << "Spacial hash ran out of space\n";
		}*/
		auto [ixmin, iymin] = Chunks.GetChunkId(e.Position.x - e.Size, e.Position.y - e.Size);
		auto [ixmax, iymax] = Chunks.GetChunkId(e.Position.x + e.Size, e.Position.y + e.Size);
		for (int dx = ixmin; dx <= ixmax;++dx)
		{
			for (int dy = iymin; dy <= iymax;++dy)
			{
				auto& c = Chunks.GetChunk(dx,dy);
				if (c.EntityCount < c.MaxEntities)
				{
					c.EntityIDList[c.EntityCount++] = i;
				}
				else {
					std::cout << "Spacial hash ran out of space\n";
				}
			}
		}
	}
}
void World::UpdateCollisionEntities(Entity& e, Entity& ej)
{
	auto dir = glm::vec2(WrapDistance(e.Position.x, ej.Position.x), WrapDistance(e.Position.y, ej.Position.y));
	float distsquare = glm::dot(dir, dir);
	float combsize = (e.Size + ej.Size);
	if (distsquare < combsize * combsize && distsquare != 0)
	{
		float distance = sqrtf(distsquare);
		auto norm = dir / distance;
		float displacementajustment = (combsize - distance);
		float totalmass = e.Mass + ej.Mass;
		if (e.PhysicsUpdate != ej.PhysicsUpdate)
		{
			displacementajustment *= 2;
		}
		if (e.PhysicsUpdate)
		{
			e.Position += norm * (displacementajustment * (ej.Mass / totalmass));
		}
		if (ej.PhysicsUpdate)
		{
			ej.Position -= norm * (displacementajustment * (e.Mass / totalmass));
		}
		if (abs(displacementajustment)/e.Size > 0.8 ||abs(displacementajustment)/ej.Size > 0.8)
		{
			e.PositionOld += norm * (displacementajustment * (ej.Mass / totalmass));
			ej.PositionOld -= norm * (displacementajustment * (e.Mass / totalmass));
		}
		if (e.Alive != ej.Alive)
		{
				
			Entity& Alive = e.Alive ? e : ej;
			Entity& Dead = !e.Alive ? e : ej;
			float de = std::min(Dead.Energy, Alive.MaxEnergy - Alive.Energy);
			Dead.Energy -= de;
			Alive.Energy += de;
			if (e.Affiliation != ej.Affiliation)
			{
				de = std::min(Dead.Energy, Alive.FoodCarryAmount);
				Dead.Energy -= de;
				Alive.HeldFood += de;
				Alive.TimeFromFood = 0;
			}
		}
		if (e.Affiliation != ej.Affiliation)
		{
			if (e.Alive && ej.Alive)
			{
				float Damage = 0.5 * (1 + random_number(generator)); 
				e.Health -= Damage;
				float de = std::min(e.Energy, ej.FoodCarryAmount - ej.HeldFood);
				//de = 0;
				//e.Energy -= de;
				//ej.HeldFood += de;
				//ej.TimeFromFood = 0;

				Damage = 0.5 * (1 + random_number(generator)); 
				ej.Health -= Damage;
				de = std::min(ej.Energy, e.FoodCarryAmount - e.HeldFood);
				//de = 0;
				//ej.Energy -= de;
				//e.HeldFood += de;
				//e.TimeFromFood = 0;

				e.Attacked = true;
				ej.Attacked = true;
			}
		}
		if (e.Affiliation == ej.Affiliation && 
			e.Alive == ej.Alive &&
				(
					(ej.Type == Entity::EntityType::Ant && e.Type == Entity::EntityType::AntNest)
					||
					(e.Type == Entity::EntityType::Ant && ej.Type == Entity::EntityType::AntNest)
				)
			)
		{
			Entity& ant = (e.Type == Entity::EntityType::Ant) ? e : ej;
			Entity& antnest = (e.Type == Entity::EntityType::AntNest) ? e : ej;

			antnest.Energy += ant.HeldFood;
			ant.TimeFromHome = 0;
			ant.HeldFood = 0;
			float de = std::min(e.MaxEnergy - e.Energy, antnest.Energy);
			e.Energy += de;
			antnest.Energy -= de;

			float Angle = random_number(generator) * 3.14 * 2;
			float Distance = (1.01+(random_number(generator)*0.5)) * antnest.Size;
//				float Distance = (1.01) * antnest.Size;
			glm::vec2 randplace = glm::vec2(cosf(Angle), sinf(Angle)) * Distance;
			ant.Position = antnest.Position + randplace;
			ant.PositionOld = ant.Position;
			//ant.Heading = glm::vec2(0, 0);
			ant.Heading *= 0.5;
		}
	}
}
void World::UpdateCollisionChunk(int i)
{
	auto& c = Chunks.raw_data[i];
	for (int i = 0; i < c.EntityCount-1; ++i)
	{
		int id = c.EntityIDList[i];
		auto & e = EntityList.GetElement(id);
		for (int j = i+1; j < c.EntityCount; ++j)
		{
			int idj = c.EntityIDList[j];
			auto & ej = EntityList.GetElement(idj);
			//if(id != idj)
			{
				UpdateCollisionEntities(e, ej);
			}
		}
	}
}
void World::UpdateCollisionSingleChunk(int x, int y)
{
	auto& c = Chunks.GetChunk(x, y);
	for (int i = 0; i < c.EntityCount-1; ++i)
	{
		int id = c.EntityIDList[i];
		auto & e = EntityList.GetElement(id);
		for (int j = i+1; j < c.EntityCount; ++j)
		{
			int idj = c.EntityIDList[j];
			auto & ej = EntityList.GetElement(idj);
			//if(id != idj)
			{
				UpdateCollisionEntities(e, ej);
			}
		}
	}
}
void World::UpdateCollisionChunk(int x, int y, int xn, int yn)
{
	auto& c = Chunks.GetChunk(x, y);
	auto& cn = Chunks.GetChunk(xn, yn);
	for (int i = 0; i < c.EntityCount; ++i)
	{
		int id = c.EntityIDList[i];
		auto& e = EntityList.GetElement(id);
		//if (abs(e.Position.x - (xn * Chunk::Size)) < Chunk::Size || abs(e.Position.y - (yn * Chunk::Size)) < Chunk::Size)
		{
			for (int j = 0; j < cn.EntityCount; ++j)
			{
				int idj = cn.EntityIDList[j];
				auto& ej = EntityList.GetElement(idj);
				if (id != idj)
				{
					UpdateCollisionEntities(e, ej);
				}
			}
		}
	}
}
void World::UpdateCollision()
{
#pragma omp parallel for
	for (int x = 0; x < Chunks.raw_data.size(); ++x)
	{
		UpdateCollisionChunk(x);
	}
/*	for (int x = 0; x < Chunks.ChunkCount; ++x)
	{
		for (int y = 0; y < Chunks.ChunkCount; ++y)
		{
			UpdateCollisionSingleChunk(x, y);
			//UpdateCollisionChunk(x, y, x + 1, y);
			//UpdateCollisionChunk(x, y, x, y + 1);
			//UpdateCollisionChunk(x, y, x + 1, y + 1);
		}
	}
	*/
		
}
void World::UpdateEntities() 
{
#pragma omp parallel for
	for (int i = 0; i < EntityList.ElementCount; ++i)
	{
		auto& e = EntityList.GetElement(i);
		//if (e.UpdateTick)
		{
			e.Update(*this);
			if (e.UpdateBehaviorCounter++ >= e.UpdateBehaviorMax)
			{
				ExecuteBehavior(*this,e);
				e.UpdateBehaviorCounter = 0;
			}
		}
		//e.UpdateTick = !e.UpdateTick;
	}
}
void World::UpdateDeadEntities()
{
	for (int i = 0; i < EntityList.ElementCount; ++i)
	{
		auto& e = EntityList.GetElement(i);
		if (e.ToRemove)
		{
			if (e.Affiliation >= 0 && e.Affiliation < AntFactionCount)
			{
				if (e.Type == Entity::EntityType::Ant)
				{
					FactionArray[e.Affiliation].AntCount -= 1;
				}
				if (e.Type == Entity::EntityType::AntNest)
				{
					FactionArray[e.Affiliation].ColonyCount -= 1;
				}
			}
			//AffiliationCounter[e.Affiliation + 1]--;
			EntityList.RemoveElement(i--);
		}
	}
}
Entity World::PrintEntityInfo(float x, float y) {
	auto& c = Chunks.GetChunkPos(x, y);
	auto pos = glm::vec2(x, y);
	for (int i = 0; i < c.EntityCount; ++i)
	{
		auto& e = EntityList.GetElement(c.EntityIDList[i]);
		auto dist = glm::vec2(WrapDistance(pos.x , e.Position.x), WrapDistance(pos.y , e.Position.y));
		if (glm::length(dist) < e.Size) {
			std::cout << "-----------------------------\n";
			std::cout << "Info about entity:" << c.EntityIDList[i] << "\n";
			std::cout << "Alive:" << e.Alive << "\n";
			std::cout << "Position:" << e.Position.x<<", " << e.Position.y << "\n";
			std::cout << "Health:" << e.Health << "\n";
			std::cout << "Energy:" << e.Energy << "\n";
			std::cout << "Age:" << e.Age << "\n";
			std::cout << "Held food:" << e.HeldFood << "\n";
			std::cout << "Time from home:" << e.TimeFromHome << "\n";
			std::cout << "Time from food:" << e.TimeFromFood << "\n";
			return e;
		}
	}
	return Entity();
}

