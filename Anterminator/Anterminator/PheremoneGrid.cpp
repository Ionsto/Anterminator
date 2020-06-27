#include "PheremoneGrid.h"
void PheremoneChunk::Update(float DecayRate,float dt)
{
	ZeroValue = true;
	for (auto& ph : raw_data)
	{
		ph.Strength -= DecayRate * dt;
		ph.Strength = std::clamp(ph.Strength, 0.0f, MaxValue);
		if (ph.Strength < 0.001) {
			ph.Strength = 0;
			ph.Direction.x = 0;
			ph.Direction.y = 0;
		}
		else
		{
			ZeroValue = false;
		}
		//Maybe diffuse too
	}
}
void PheremoneGrid::Update(float dt) {
	if (Alive) {
		UpdateCounter += dt;
		if (UpdateCounter >= UpdateCounterMax)
		{
			MarkActive();
			UpdateChunks(UpdateCounter);
			UpdateCounter = 0;
		}
	}
}
void PheremoneGrid::UpdateChunks(float dt) 
{
	DiffuseTime += dt;
	for (auto& c : Chunks)
	{
		if (c.Active) 
		{
			c.Update(DecayRate,dt);
			if (DiffuseTime > DiffuseTimeMax)
			{
				Difffuse(c, dt);
			}
		}
	}
	if (DiffuseTime > DiffuseTimeMax)
	{
		DiffuseTime = 0;
	}
}
void PheremoneGrid::MarkActive() 
{
	for (int x = 0; x < ChunkCount; ++x)
	{
		for (int y = 0; y < ChunkCount; ++y)
		{
			GetChunkGrid(x, y).Active = false;
			for (int dx = -1; dx <= 1; ++dx)
			{
				for (int dy = -1; dy <= 1; ++dy)
				{
					//if (x + dx > 0 && x + dx < ChunkCount)
					{
						//if (y + dy > 0 && y + dy < ChunkCount)
						{
							GetChunkGrid(x, y).Active |= !GetChunkGrid(x + dx, y + dy).ZeroValue;
						}
					}
				}
			}
		}
	}
}
