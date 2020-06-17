#include "Chunk.h"
#include "World.h"
Chunk::Chunk(float x,float y) : X_Real(x),Y_Real(y){
	X = X_Real;
	Y = Y_Real;
}
void Chunk::Update() {
	//for (int i = 0; i < EntityList.ElementCount; ++i)
	//{
	//	EntityList.GetParticle(i).Update();
	//}
}
