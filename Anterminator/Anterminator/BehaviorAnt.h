#pragma once
#include "Entity.h"
#include "BehaviorRandomWalk.h"
#include "BehaviorFollowPheremone.h"
#include "BehaviorPheremonePlace.h"
#include "BehaviorPheremoneHillClimb.h"
#include "PheremoneGrid.h"
#include "AntColonyController.h"
namespace BehaviorAnt {
	void Update(Entity& e, AntColonyController& c)
	{
		//	BehaviorRandomWalk::Update(e, 0.01, 100);
			///ColonyController& c = ColonyArray[af];
		if (e.Alive)
		{
			BehaviorPheremonePlace::Update(e, c.PheremoneDensity, 1, c.DensityLay, 0);
			//c.behaviour_ph_density.Update(*this, e, c.Pheremone_Density,c.DensityLay,0);
			if (e.HeldFood > 0.5) {
				//Bring food back to colony
				BehaviorPheremonePlace::Update(e, c.PheremoneFood, 1, c.WalkFoodPHLayFood * e.HeldFood, 10);
				///c.behaviour_ph_food.Update(*this, e, c.Pheremone_Food,c.WalkFoodPHLayFood * e.HeldFood,10);
				BehaviorRandomWalk::Update(e, c.WalkFoodRandom, c.WalkFoodRandomSpeed);
				///c.behaviour_walk.Update(*this, e, c.WalkFoodRandom);
				BehaviorFollowPheremone::Update(e, c.PheremoneHome, c.WalkFoodPHFollowHome);
				///c.behaviour_home.Update(*this, e, c.Pheremone_Home, c.WalkFoodPHFollowHome);
				BehaviorPheremoneHillClimb::Update(e, c.PheremoneDensity, c.DensityAvoid);
				///c.behaviour_density.Update(*this, e, c.Pheremone_Density,c.DensityAvoid);
				//c.behaviour_followkilled.Update(*this, e, c.Pheremone_Density, c.AvoidDenstiy);
			}
			else {
				if (e.TimeFromHome > c.WalkHomeTime)
				{

					//Walk back to colony
					BehaviorRandomWalk::Update(e, c.WalkHomeRandom, c.WalkHomeRandomSpeed);

					///c.behaviour_walk.Update(*this, e, c.WalkHomeRandom);
					BehaviorFollowPheremone::Update(e, c.PheremoneHome, c.WalkHomePHFollowHome);
					///c.behaviour_home.Update(*this, e, c.Pheremone_Home, c.WalkHomePHFollowHome);
					///c.behaviour_density.Update(*this, e, c.Pheremone_Density,c.DensityAvoid)
					BehaviorPheremoneHillClimb::Update(e, c.PheremoneDensity, c.DensityAvoid);
				}
				else {
					//Explore - follow trails
					float PhereStrengthThres = 50;
					float MaxPhere = 
						std::max(
						std::max(
						c.PheremoneHome.GetPheremone(e.Position.x, e.Position.y).Strength,
						c.PheremoneFood.GetPheremone(e.Position.x, e.Position.y).Strength),
						std::max(
						c.PheremoneKilled.GetPheremone(e.Position.x, e.Position.y).Strength,
						c.PheremoneAttack.GetPheremone(e.Position.x, e.Position.y).Strength)
					);
					MaxPhere = std::min(MaxPhere, 100.0f);
					if (MaxPhere > PhereStrengthThres)
					{
						e.TimeFromHome -= e.UpdateBehaviorMax * World::DeltaTime * 0.7 * MaxPhere;
						e.TimeFromHome = std::max(0.0f, e.TimeFromHome);
					}
					BehaviorRandomWalk::Update(e, c.WalkDiscoverRandom, c.WalkDiscoverRandomSpeed);
					///c.behaviour_walk.Update(*this, e, c.WalkDiscoverRandom);

					BehaviorPheremonePlace::Update(e, c.PheremoneHome,1, c.WalkDiscoverPHLayHome, 1);
					///c.behaviour_ph_home.Update(*this, e, c.Pheremone_Home, c.WalkDiscoverPHLayHome,1);

					BehaviorFollowPheremone::Update(e, c.PheremoneFood, c.WalkDiscoverPHFollowFood);
					///c.behaviour_food.Update(*this, e, c.Pheremone_Food, c.);

					BehaviorFollowPheremone::Update(e, c.PheremoneHome, c.WalkDiscoverPHFollowHome);
					///c.behaviour_home.Update(*this, e, c.Pheremone_Home, c.);

					BehaviorPheremoneHillClimb::Update(e, c.PheremoneDensity, c.DensityAvoid);
					BehaviorPheremoneHillClimb::Update(e, c.PheremoneKilled, c.FindKilled);
					float AttackMult = (c.PheremoneKilled.GetPheremone(e.Position.x, e.Position.y).Strength * c.AttackDensityMult)
						+ (c.PheremoneAttack.GetPheremone(e.Position.x, e.Position.y).Strength * c.AttackRepropogateMult);
					BehaviorPheremonePlace::Update(e, c.PheremoneAttack, 1, AttackMult, 0.1);
					BehaviorFollowPheremone::Update(e, c.PheremoneAttack, c.AttackFollow);
					///c.behaviour_followkilled.Update(*this, e, c.Pheremone_Killed, c.FindKilled);
					///c.behaviour_density.Update(*this, e, c.Pheremone_Density,c.DensityAvoid);
					///c.behaviour_followfood.Update(*this, e, FoodScent,c.FollowFood);

				}
			}
		}
		else
		{
			if (e.Attacked)
			{
				BehaviorPheremonePlace::Update(e, c.PheremoneKilled, 10, c.WarnKilled, 0);
			}
		}
	}
};
