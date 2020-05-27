#include "World.h"
#include "ColonyController.h"
#include <numeric>
#include <time.h>

World::World():random_number(-1,1) {
	generator.seed(time(NULL));
	for (int i = 0; i < AntColonyMax; ++i)
	{
		ColonyArray[i].HiveLocation = glm::vec2(random_number(generator) * WorldSize,random_number(generator) * WorldSize);
		ColonyArray[i].Affiliation = i;
		ColonyArray[i].AddAnt(*this);
		ColonyArray[i].Read("BestColony.txt");
		ColonyArray[i].Randomise(0.1);
	}
	for(int i = 0;i < 100;++i)
	{
		FoodCounter = 0;
		Entity e = Entity();
		e.Position.x = random_number(generator) * WorldSize;
		e.Position.y = random_number(generator) * WorldSize;
		e.OldPosition = e.Position;
		e.Size = 6;
		e.Mass = 100;
		e.MaxEnergy = 50000;
		e.Energy = e.MaxEnergy;
		e.Health = 100;
		e.Affiliation = -1;
		AddEntity(e);
	}
	FoodScent.DiffuseTimeMax = 20;
	FoodScent.DiffusionConstant = 1000;
	FoodScent.DecayRate = 1;
	FoodScentPlace.SpreadSize = 5;
}
void World::Update(float realdt)
{
	TimeSpacialHash = 0;
	TimeCollisions = 0;
	TimeUpdateEntity = 0;
	TimeUpdateColonies = 0;
	DtAccumulator += realdt * TimeScalingFactor;
	int dtstepcount = std::min(500, static_cast<int>(DtAccumulator / this->DeltaTime));
	DtAccumulator -= this->DeltaTime * static_cast<float>(dtstepcount);
	DtAccumulator = std::min(this->DeltaTime * 100, DtAccumulator);
	for (int i = 0; i < dtstepcount;++i)
	{
		ElapsedTime += this->DeltaTime;
		UpdateColonies();
		UpdateEntities();
		UpdateSpacialHash();
		UpdateCollisions();
		UpdateDeadEntities();
		FoodCounter += this->DeltaTime;
		if (FoodCounter > 10 * FoodDecrement)
		{
			FoodDecrement++;
			FoodCounter = 0;
			Entity e = Entity();
			e.Position.x = random_number(generator) * WorldSize;
			e.Position.y = random_number(generator) * WorldSize;
			e.OldPosition = e.Position;
			e.Size = 50;
			e.Mass = 500;
			e.MaxEnergy = 500'000;
			e.Energy = e.MaxEnergy;
			e.Health = 100;
			e.Affiliation = -1;
			AddEntity(e);
		}
		if (GraphSampleCounter++ == 100)
		{
			GraphSampleCounter = 0;
			if (std::accumulate(++AffiliationCounter.begin(),AffiliationCounter.end(), 0) != 0)
			{
				for (int c = 0; c < AntColonyMax; ++c)
				{
					//AntCountHistory[c].push_back(AffiliationCounter[c+1]);
					AntCountHistory[c].push_back(std::max(0.0f,ColonyArray[c].TotalEnergy + (AffiliationCounter[c+1] * 100)));
				}
			}
		}
	}
}
void World::UpdateSpacialHash()
{
	auto start = std::chrono::high_resolution_clock::now();
	for (auto& c : Chunks.raw_data)
	{
		c.EntityCount = 0;
	}
#pragma omp parallel for
	for (int i = 0; i < EntityList.ParticleCount; ++i)
	{
		auto& e = EntityList.GetParticle(i);
		auto hashchunk = [=](float dx,float dy) {
			auto& c = Chunks.GetChunk(e.Position.x + dx, e.Position.y + dy);
			if (c.EntityCount < c.MaxEntities)
			{
				c.Entitiy[c.EntityCount++] = i;
			}
			else {
				std::cout << "Spacial hash ran out of space\n";
			}
		};
		auto [ixmin, iymin] = Chunks.GetChunkPos(e.Position.x - e.Size, e.Position.y - e.Size);
		auto [ixmax, iymax] = Chunks.GetChunkPos(e.Position.x + e.Size, e.Position.y + e.Size);
		for (int dx = ixmin; dx <= ixmax;++dx)
		{
			for (int dy = iymin; dy <= iymax;++dy)
			{
				auto& c = Chunks.GetChunkGrid(dx,dy);
				if (c.EntityCount < c.MaxEntities)
				{
					c.Entitiy[c.EntityCount++] = i;
				}
				else {
					std::cout << "Spacial hash ran out of space\n";
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	TimeSpacialHash += (end - start).count()/1e9;
}
void World::UpdateDeadEntities()
{
	for (int i = 0; i < EntityList.ParticleCount; ++i)
	{
		auto& e = EntityList.GetParticle(i);
		if (e.ToRemove)
		{
			AffiliationCounter[e.Affiliation + 1]--;
			EntityList.RemoveParticle(i--);
		}
	}
}
void World::UpdateCollisionEntities(Entity& e, Entity& ej)
{
	auto dir = e.Position - ej.Position;
	float distsquare = glm::dot(dir, dir);
	float combsize = (e.Size + ej.Size);
	if (distsquare < combsize * combsize && distsquare != 0)
	{
		float distance = sqrtf(distsquare);
		auto norm = dir / distance;
		float displacementajustment = (combsize - distance);
		float totalmass = e.Mass + ej.Mass;
		e.Position += norm * (displacementajustment * (ej.Mass / totalmass));
		ej.Position -= norm * (displacementajustment * (e.Mass / totalmass));
		if (e.Dead || ej.Dead && e.Dead != ej.Dead)
		{
			if (e.Dead)
			{
				float de = std::min(e.Energy, ej.MaxEnergy - ej.Energy);
				e.Energy -= de;
				ej.Energy += de;

				de = std::min(e.Energy, ej.FoodCarryAmount);
				e.Energy -= de;
				ej.HeldFood += de;
				ej.TimeFromFood = 0;
			}
			else {
				float de = std::min(ej.Energy, e.MaxEnergy - e.Energy);
				ej.Energy -= de;
				e.Energy += de;

				de = std::min(ej.Energy, e.FoodCarryAmount);
				ej.Energy -= de;
				e.HeldFood += de;
				e.TimeFromFood = 0;
			}
		}
		else {
			if (e.Affiliation != ej.Affiliation)
			{
				e.Health -= 1 * ((rand()%10)/5.0f);
				ej.Health -= 1 * ((rand()%10)/5.0f);
				if (e.Affiliation == -1)
				{
					//ej.HeldFood = 1;
					//ej.TimeFromFood = 0;
				}
				if (ej.Affiliation == -1)
				{
					//e.HeldFood = 1;
					//e.TimeFromFood = 0;
				}
			}
		}
	}
}
void World::UpdateCollisionSingleChunk(int x, int y)
{
	auto& c = Chunks.GetChunkGrid(x, y);
	for (int i = 0; i < c.EntityCount-1; ++i)
	{
		int id = c.Entitiy[i];
		auto & e = EntityList.GetParticle(id);
		for (int j = i+1; j < c.EntityCount; ++j)
		{
			int idj = c.Entitiy[j];
			auto & ej = EntityList.GetParticle(idj);
			UpdateCollisionEntities(e, ej);
		}
	}
}
void World::UpdateCollisionChunk(int x, int y, int xn, int yn)
{
	auto& c = Chunks.GetChunkGrid(x, y);
	auto& cn = Chunks.GetChunkGrid(xn, yn);
	for (int i = 0; i < c.EntityCount; ++i)
	{
		int id = c.Entitiy[i];
		auto& e = EntityList.GetParticle(id);
		if (abs(e.Position.x - (xn * Chunk::ChunkSize)) < Chunk::ChunkSize || abs(e.Position.y - (yn * Chunk::ChunkSize)) < Chunk::ChunkSize)
		{
			for (int j = 0; j < cn.EntityCount; ++j)
			{
				int idj = cn.Entitiy[j];
				auto& ej = EntityList.GetParticle(idj);
				UpdateCollisionEntities(e, ej);
			}
		}
	}
}
void World::UpdateCollisions()
{
	auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
	for (int x = 0; x < Chunks.ChunkCount; ++x)
	{
		for (int y = 0; y < Chunks.ChunkCount; ++y)
		{
			UpdateCollisionSingleChunk(x, y);
			if (x + 1 != Chunks.ChunkCount)
			{
				UpdateCollisionChunk(x, y, x + 1, y);
			}
			if (y + 1 != Chunks.ChunkCount)
			{
				UpdateCollisionChunk(x, y, x, y + 1);
			}
			if (x + 1 != Chunks.ChunkCount && y + 1 != Chunks.ChunkCount)
			{
				UpdateCollisionChunk(x, y, x + 1, y + 1);
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	TimeCollisions += (end - start).count()/1e9;
}
void World::UpdateColonies()
{
	auto start = std::chrono::high_resolution_clock::now();
	for (auto& c : ColonyArray)
	{
		if (c.ColonyAlive)
		{
			c.Update(*this);
		}
		if (c.ColonyAlive && c.TotalEnergy <= 0 && AffiliationCounter[c.Affiliation + 1] == 0) {
			c.ColonyAlive = false;
			--AntColonyCount;
			std::cout << "Colony " << c.Affiliation << " died\n";
			for (int i = 0; i < c.Pheremone_Home.GridSize * c.Pheremone_Home.GridSize; ++i)
			{
				c.Pheremone_Killed.cached_data[c.Pheremone_Killed.SwapBuffer].Strength[i] = 0;
				c.Pheremone_Density.cached_data[c.Pheremone_Density.SwapBuffer].Strength[i] = 0;
				c.Pheremone_Home.cached_data[c.Pheremone_Home.SwapBuffer].Strength[i] = 0;
				c.Pheremone_Food.cached_data[c.Pheremone_Food.SwapBuffer].Strength[i] = 0;
			}
			if (AntColonyCount == 0)
			{
				c.Write("BestColony.txt");
			}
		}
	}
	FoodScent.Update(this->DeltaTime);
	auto end = std::chrono::high_resolution_clock::now();
	TimeUpdateColonies += (end - start).count()/1e9;
}
void World::UpdateEntities()
{
auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
	for (int i = 0; i < EntityList.ParticleCount; ++i)
	{
		auto& e = EntityList.GetParticle(i);
		int af = e.Affiliation;
		if (af != -1)
		{
			ColonyController& c = ColonyArray[af];
			if(!e.Dead)
			{
				c.behaviour_ph_density.Update(*this, e, c.Pheremone_Density,c.DensityLay,0);
				if (e.HeldFood > 0.5) {
					//Bring food back to colony
					c.behaviour_ph_food.Update(*this, e, c.Pheremone_Food,c.WalkFoodPHLayFood * e.HeldFood,10);
					c.behaviour_walk.Update(*this, e, c.WalkFoodRandom);
					c.behaviour_home.Update(*this, e, c.Pheremone_Home, c.WalkFoodPHFollowHome);
					c.behaviour_density.Update(*this, e, c.Pheremone_Density,c.DensityAvoid);
					//c.behaviour_followkilled.Update(*this, e, c.Pheremone_Density, c.AvoidDenstiy);
				}
				else {
					if (e.TimeFromHome > c.WalkHomeTime)
					{
						//Walk back to colony
						c.behaviour_walk.Update(*this, e, c.WalkHomeRandom);
						c.behaviour_home.Update(*this, e, c.Pheremone_Home, c.WalkHomePHFollowHome);
						c.behaviour_density.Update(*this, e, c.Pheremone_Density,c.DensityAvoid);
					}
					else {
						//Explore - follow trails
						float PhereStrengthThres = 50;
						float MaxPhere = std::max(c.Pheremone_Home.GetStrength(e.Position.x, e.Position.y),
							c.Pheremone_Food.GetStrength(e.Position.x, e.Position.y));
						if (MaxPhere > PhereStrengthThres)

						{
							e.TimeFromHome -= DeltaTime * 0.7 * MaxPhere / PhereStrengthThres;
							e.TimeFromHome = std::max(0.0f, e.TimeFromHome);
						}
						c.behaviour_walk.Update(*this, e, c.WalkDiscoverRandom);
						c.behaviour_ph_home.Update(*this, e, c.Pheremone_Home, c.WalkDiscoverPHLayHome,1);
						c.behaviour_food.Update(*this, e, c.Pheremone_Food, c.WalkDiscoverPHFollowFood);
						c.behaviour_home.Update(*this, e, c.Pheremone_Home, c.WalkDiscoverPHFollowHome);
						c.behaviour_followkilled.Update(*this, e, c.Pheremone_Killed, c.FindKilled);
						c.behaviour_density.Update(*this, e, c.Pheremone_Density,c.DensityAvoid);
						c.behaviour_followfood.Update(*this, e, FoodScent,c.FollowFood);
//						c.behaviour_escapedead.Update(*this, e, c.Pheremone_Dead, c.FindDead);

					}
	//				c.behaviour_home.Update(*this, e, c.Pheremone_Home, -0.001);
				}
				for (int j = 0; j < AntColonyMax; ++j)
				{
					if (glm::distance(e.Position, ColonyArray[j].HiveLocation) < 20.0)
					{
						if (j == e.Affiliation)
						{
							c.TotalEnergy += e.HeldFood;
							e.HeldFood = 0;
							e.TimeFromHome = 0;
							float de = std::min(e.MaxEnergy - e.Energy, c.TotalEnergy);
							e.Energy += de;
							c.TotalEnergy -= de;
						}
						else
						{
							if (ColonyArray[j].ColonyAlive == true)
							{
								float de = std::min(e.FoodCarryAmount - e.HeldFood, ColonyArray[j].TotalEnergy);
								if (de > 0)
								{
									e.TimeFromFood = 0;
								}
								e.HeldFood += de;
								float damage = de;// std::min(e.MaxEnergy * 5, ColonyArray[j].TotalEnergy);
								ColonyArray[j].TotalEnergy -= damage;
							}
							else {
								//ColonyArray[j] = ColonyController();
								//ColonyArray[j].Affiliation = j;
							}
						}
					}
				}
			}
		}
		else {
			if (!e.Dead)
			{
				NPCWalking.RandomValue = 0.005;
				NPCWalking.Update(*this, e, 1000);
				FoodScentPlace.SpreadSize = std::ceil(e.Size/PheremoneGrid::Size);
				FoodScentPlace.Update(*this, e, FoodScent,10,0);
			}
			else {
				FoodScentPlace.SpreadSize = std::ceil(e.Size/PheremoneGrid::Size);
				FoodScentPlace.Update(*this, e, FoodScent,5,0);
			}
		}
		e.Update(*this);
		if (af != -1)
		{
			if(e.Dead)
			{
				ColonyController& c = ColonyArray[af];
				if (e.Health <= 0)
				{
					//We've been killed
					if(!e.DeadSpread)
					{
						e.DeadSpread = true;
						c.behaviour_ph_killed.Update(*this, e, c.Pheremone_Killed,c.WarnKilled * 10,0);
					}
				}
				else
				{
					//Naturally expired
//					c.behaviour_ph_dead.Update(*this, e, c.Pheremone_Dead,c.WarnDead,0);
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	TimeUpdateEntity += (end - start).count()/1e9;
}
void World::PrintEntityInfo(float x, float y) {
	auto& c = Chunks.GetChunk(x, y);
	auto pos = glm::vec2(x, y);
	for (int i = 0; i < c.EntityCount; ++i)
	{
		auto& e = EntityList.GetParticle(c.Entitiy[i]);
		if (glm::length(pos - e.Position) < e.Size) {
			std::cout << "Info about entity:" << c.Entitiy[i] << "\n";
			std::cout << "Position:" << e.Position.x<<", " << e.Position.y << "\n";
			std::cout << "Health:" << e.Health << "\n";
			std::cout << "Energy:" << e.Energy << "\n";
			std::cout << "Age:" << e.Age << "\n";
			std::cout << "Held food:" << e.HeldFood << "\n";
			std::cout << "Time from home:" << e.TimeFromHome << "\n";
			std::cout << "Time from food:" << e.TimeFromFood << "\n";
		}
	}
}
bool World::AddEntity(Entity e)
{
	if (EntityList.AddParticle(e)) {
		AffiliationCounter[e.Affiliation + 1]++;
		return true;
	}
	return false;
}
