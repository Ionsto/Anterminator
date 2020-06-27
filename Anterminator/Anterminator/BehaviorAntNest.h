#pragma once
#include "Entity.h"
#include "BehaviorRandomWalk.h"
#include "BehaviorFollowPheremone.h"
#include "AntColonyController.h"
#include "World.h"
namespace BehaviorAntNest {
	std::default_random_engine generator(rand());
	std::uniform_real_distribution<float>  distr;
	void Update(World & world, Entity& e, AntColonyController& c)
	{
		if (e.Alive)
		{
			if (e.Health < 100)
			{
				float MaxEnergyHeal = 10;
				float de = std::min(MaxEnergyHeal, e.Energy);
				e.Health += World::DeltaTime * de;
				e.Energy -= de;
			}
			int SpawnCount = static_cast<int>(distr(generator) * (1+c.AntSpawnRate));
			for(int i = 0; i < SpawnCount;++i)
			{
				Entity ant = c.CreateAnt(world);
				float Angle = distr(generator) * 3.14 * 2;
				float Distance = (1+(distr(generator) * 0.5)) * e.Size;
				glm::vec2 randplace = glm::vec2(cosf(Angle), sinf(Angle)) * Distance;
				ant.Position = e.Position + randplace;
				ant.PositionOld = ant.Position;
				if (e.Energy - c.MinColonyEnergy > ant.Energy)
				{
					if (c.AddAnt(world, std::move(ant))) {
						e.Energy -= ant.Energy;
					}
				}
			}
			SpawnCount = static_cast<int>(distr(generator) * (1+(c.ColonySpawnRate/c.ColonySpawnRateError)));
			for(int i = 0; i < SpawnCount;++i)
			{
				int aff = -1;
				for (int j = 0; j < world.AntFactionCount; ++j)
				{
					if (!world.FactionArray[j].Alive)
					{
						aff = j;
						break;
					}
				}
				if (aff != -1)
				{
					auto& cnew = world.FactionArray[aff];
					cnew.Copy(c);
					cnew.Randomise(0.1);
					cnew.Colour = c.Colour + (((glm::vec3(distr(generator),distr(generator),distr(generator)) * 2.0f) - 1.0f) * 0.1f);
					cnew.Colour = glm::clamp(cnew.Colour, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
					Entity antnest = cnew.CreateColony(world);
					antnest.Energy = cnew.MinColonyEnergy;
					float Angle = distr(generator) * 3.14 * 2;
					float Distance = (3+(distr(generator) * 5)) * e.Size;
					//glm::vec2 randplace = glm::vec2(cosf(Angle), sinf(Angle)) * Distance;
					glm::vec2 randompos = world.WorldSize * glm::vec2(distr(generator), distr(generator));
					antnest.Position = randompos;
					antnest.PositionOld = antnest.Position;
					if (e.Energy - c.MinColonyEnergy > antnest.Energy)
					{
						if (world.AddEntity(std::move(antnest))) {
							e.Energy -= antnest.Energy;
						}
					}
				}
			}
			SpawnCount = static_cast<int>(distr(generator) * (1+c.ColonySpawnRate));
			for(int i = 0; i < SpawnCount;++i)
			{
				Entity antnest = c.CreateColony(world);
				antnest.Energy = c.ColonyStartEnergy;
				float Angle = distr(generator) * 3.14 * 2;
				float Distance = (3+(distr(generator) * 2)) * e.Size;
				glm::vec2 randplace = glm::vec2(cosf(Angle), sinf(Angle)) * Distance;
				antnest.Position = e.Position + randplace;
				antnest.PositionOld = antnest.Position;
				if (e.Energy - c.MinColonyEnergy > antnest.Energy)
				{
					if (world.AddEntity(std::move(antnest))) {
						e.Energy -= antnest.Energy;
					}
				}
			}
		}
	}
};
