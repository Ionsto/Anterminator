#include "Chunk.h"
void Chunk::Update() {
	for (int i = 0; i < EntityList.ElementCount; ++i)
	{
		EntityList.GetParticle(i).Update();
	}
}
