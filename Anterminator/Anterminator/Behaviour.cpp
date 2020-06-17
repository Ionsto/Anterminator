#include "Behaviour.h"
#include "BehaviorRandomWalk.h"
#include "BehaviorAnt.h"
#include "BehaviorPrey.h"
#include "BehaviorPredator.h"
#include "BehaviorAntNest.h"
#include "World.h"
void ExecuteBehavior(World & world, Entity& e)
{
	switch (e.Type)
	{
	case Entity::EntityType::Ant:
		BehaviorAnt::Update(e,world.FactionArray[e.Affiliation]);
		break;
	case Entity::EntityType::Prey:
		BehaviorPrey::Update(e);
		break;
	case Entity::EntityType::Predator:
		BehaviorPredator::Update(e);
		break;
	case Entity::EntityType::AntNest:
		BehaviorAntNest::Update(world,e,world.FactionArray[e.Affiliation]);
		break;
	case Entity::EntityType::Default:
		BehaviorRandomWalk::Update(e, 0.01, 100);
		break;
	}
}
