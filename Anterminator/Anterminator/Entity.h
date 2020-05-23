#pragma once
#include <glm/glm.hpp>
class Entity
{
	glm::vec2 Pos;
	glm::vec2 PosOld;
	glm::vec2 Acceleration;
	float Size = 1;
	//0 == ffa
	//>10 ant colonies
	int Affiliation = 0;
	enum class EntityType {
		Default,
		Ant,
		Prey,
		Predator
	} Type = EntityType::Default;
	void Update();
};

