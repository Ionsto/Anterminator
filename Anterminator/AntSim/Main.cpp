#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <numeric>
#include <algorithm>
#include <glm/glm.hpp>
#include <chrono>
#include <vector>
#include "World.h"
#include <iostream>
#include <algorithm>
#include "Entity.h"
float FrameCount = 0;
std::chrono::time_point<std::chrono::steady_clock> StartTime;
std::chrono::time_point<std::chrono::steady_clock> DtCounter;
float DeltaTime = 0;
float RenderTime = 0;
float UpdateTime = 0;
float TimeSpacialHash = 0;
float TimeCollisions = 0;
float TimeUpdateEntity = 0;
float TimeUpdateColonies = 0;
bool WindowFocused = true;
glm::vec2 WorldMousePos;
glm::vec2 WorldMousePosOld;
bool Running = true;
glm::vec2 ScreenSize(800,800);
glm::vec2 CameraPosition;
int TexUpdate = 0;
float CameraWidth = 100;
std::unique_ptr<World> world;
std::unique_ptr<sf::RenderWindow> window;
sf::CircleShape shape(1.0f);
sf::RectangleShape PheremoneMap(sf::Vector2f(World::WorldSize * 2,World::WorldSize * 2));
std::unique_ptr<sf::Texture> PheremoneTexture = std::make_unique<sf::Texture>();
static constexpr int PHGridSize = PheremoneGrid::GridSize;
std::vector<sf::Uint8> Pixels = std::vector<sf::Uint8>(PHGridSize * PHGridSize*4);
std::vector<float> tex_grid = std::vector<float>(PHGridSize * PHGridSize);
std::vector<sf::Vertex> GraphVertexBuffer;
sf::RectangleShape GraphArea;
float GraphWidth = 200;
float GraphHeight = 200;
float GraphRollingMax = 0;
int DisplayPheremone = 0;
void RenderGraph(std::vector<std::vector<float>> & databuffer) {
	GraphVertexBuffer.resize(databuffer[0].size());
	window->draw(GraphArea);
	float MaxAntCount = GraphRollingMax;
	for (int i = 0; i < world->AntColonyMax; ++i)
	{
		if (databuffer[i].size() != 0)
		{
			MaxAntCount = std::max(databuffer[i].back(),MaxAntCount);
		}
	}
	GraphRollingMax = MaxAntCount;
	for (int i = 0; i < world->AntColonyMax; ++i)
	{
		for (int c = 0; c < databuffer[i].size(); ++c) {
			GraphVertexBuffer[c].position = sf::Vector2f(c*(GraphWidth / static_cast<float>(GraphVertexBuffer.size())), GraphHeight - ((static_cast<float>(GraphHeight)/static_cast<float>(MaxAntCount)) * (databuffer[i][c])));
			GraphVertexBuffer[c].color.r = world->ColonyArray[i].Colour.x;
			GraphVertexBuffer[c].color.g = world->ColonyArray[i].Colour.y;
			GraphVertexBuffer[c].color.b = world->ColonyArray[i].Colour.z;
		}
		window->draw(GraphVertexBuffer.data(), GraphVertexBuffer.size(), sf::LineStrip);
	}
}
void Render() {
	auto start = std::chrono::high_resolution_clock::now();
		window->clear();
		float scale = 100.0 / CameraWidth;
		if (DisplayPheremone != 0) {
			if (TexUpdate++ >= 50)
			{
#pragma omp parallel for
				for (int x = 0; x < PHGridSize; ++x)
				{
					for (int y = 0; y < PHGridSize; ++y)
					{
						int i = y + (x * PHGridSize);
						i = x + (y * PHGridSize);
						Pixels[(i * 4)] = 0;// world->FoodScent.raw_data[itex].Strength;
						Pixels[(i * 4) + 1] = 0;
						Pixels[(i * 4) + 2] = 0;
						Pixels[(i * 4) + 3] = 255;
						if (DisplayPheremone == 5)
						{
							float MaxValue = 100;
							float value = world->FoodScent.GetStrengthGrid(x, y);
							float Scale = std::min(value, MaxValue) / MaxValue;
							glm::vec3 Colour = glm::vec3(0, 0, 0);
							Pixels[(i * 4)] = (255 - Colour.x)*Scale;
							Pixels[(i * 4) + 1] = (255 - Colour.y)*Scale;
							Pixels[(i * 4) + 2] = (255 - Colour.z)*Scale;
						}
						else {
							for (int j = 0; j < world->AntColonyMax; ++j)
							{
								float MaxValue = 100;
								auto& c = world->ColonyArray[j];
								if (c.ColonyAlive) {
									float value = c.Pheremone_Home.GetStrengthGrid(x, y);
									switch (DisplayPheremone) {
									case 1:
										value = c.Pheremone_Home.GetStrengthGrid(x, y);
										break;
									case 2:
										value = c.Pheremone_Food.GetStrengthGrid(x, y);
										break;
									case 3:
										value = c.Pheremone_Density.GetStrengthGrid(x, y);
										break;
									case 4:
										value = c.Pheremone_Killed.GetStrengthGrid(x, y);
										break;
									}
									//value = world->FoodScent.raw_data[itex].Strength/world->AntColonyMax;
									float Scale = std::min(value, MaxValue) / MaxValue;
									//Pixels[(i * 4)] = static_cast<sf::Uint8>(255.0 * (std::min(tex_grid[itex], MaxValue) / MaxValue));
									Pixels[(i * 4)] += (255 - c.Colour.x) * Scale;
									Pixels[(i * 4) + 1] += (255 - c.Colour.y) * Scale;
									Pixels[(i * 4) + 2] += (255 - c.Colour.z) * Scale;
								}
							}
						}
					}
				}
				/*for (int i = 0; i < tex_grid.size(); ++i)
				{
					tex_grid[i] = 0;
					for (int j = 0; j < world->AntColonyMax; ++j)
					{
						tex_grid[i] += world->ColonyArray[j].Pheremone_Home.raw_data[i].Strength;
					}
				}*/
				TexUpdate = 0;
			}
			float MaxValue = 100;
			/*for (int x = 0; x < PheremoneGrid::GridSize; ++x)
			{
				for (int y = 0; y < PheremoneGrid::GridSize; ++y)
				{
					int i = y + (x * PheremoneGrid::GridSize);
					int itex = x + (y * PheremoneGrid::GridSize);
					Pixels[(i * 4)] = 255 - static_cast<sf::Uint8>(255.0 * (std::min(tex_grid[itex], MaxValue) / MaxValue));
					Pixels[(i * 4) + 1] = 0;
					Pixels[(i * 4) + 2] = 0;
					Pixels[(i * 4) + 3] = 255;
				}
			}
			*/
			PheremoneTexture->update(Pixels.data());
			PheremoneMap.setTexture(PheremoneTexture.get());
			//PheremoneTexture->update(Pixels.data(), 0, 0, PheremoneGrid::GridSize, PheremoneGrid::GridSize);
//			PheremoneTexture->loadFromMemory(tex_grid, PheremoneGrid::GridSize * PheremoneGrid::GridSize);
			PheremoneMap.setScale(scale,scale);
			auto dist = (glm::vec2(-World::WorldSize,-World::WorldSize) - CameraPosition) * scale;
			PheremoneMap.setPosition(dist.x + (ScreenSize.x * 0.5f),dist.y + (ScreenSize.y * 0.5f));
			window->draw(PheremoneMap);
		}
		for (int i = 0; i < world->ColonyArray.size(); ++i)
		{
			auto& colony = world->ColonyArray[i];
			float AntHillSize = 20;
			auto dist = (colony.HiveLocation - CameraPosition) * scale;
			//dist.y *= -1.0;
			if (std::abs(dist.x) < ScreenSize.x / 2 && std::abs(dist.y) < ScreenSize.y / 2)
			{
				shape.setScale(scale * AntHillSize, scale * AntHillSize);
				shape.setFillColor(sf::Color(colony.Colour.x, colony.Colour.y, colony.Colour.z));
				if (!colony.ColonyAlive)
				{
					shape.setFillColor(sf::Color(200, 200, 200));
				}
				shape.setPosition(dist.x + (ScreenSize.x * 0.5f), dist.y + (ScreenSize.y * 0.5f));
				window->draw(shape);
			}
		}
		for (int i = 0; i < world->EntityList.ParticleCount; ++i)
		{
			Entity entity = world->EntityList.GetParticle(i);
			auto dist = (entity.Position - CameraPosition) * scale;
			//dist.y *= -1.0;
			if (std::abs(dist.x) < ScreenSize.x/2 && std::abs(dist.y) < ScreenSize.y/2)
			{
				shape.setScale(scale * entity.Size,scale * entity.Size);
				switch (entity.Affiliation) {
				case -1:
					shape.setFillColor(sf::Color(0, 255, 255));
					break;
				default:
					shape.setFillColor(sf::Color(world->ColonyArray[entity.Affiliation].Colour.x,world->ColonyArray[entity.Affiliation].Colour.y,world->ColonyArray[entity.Affiliation].Colour.z));
					break;
				}
				if(entity.Dead)
				{
					shape.setFillColor(sf::Color(100, 100, 100));
				}
				/*if (entity.Affiliation == -1)
				{
				}
				else {
					shape.setFillColor(sf::Color(0, (1 - entity.HeldFood) * 255, entity.HeldFood * 255));
				}*/
				shape.setPosition(dist.x + (ScreenSize.x * 0.5f),dist.y + (ScreenSize.y * 0.5f));
				window->draw(shape);
			}
		}
		RenderGraph(world->AntCountHistory);
		window->display();
	auto end = std::chrono::high_resolution_clock::now();
	RenderTime += (end - start).count()/1e9;
}
void Update() {
	auto start = std::chrono::high_resolution_clock::now();
	world->Update(DeltaTime);
	auto end = std::chrono::high_resolution_clock::now();
	UpdateTime += (end - start).count()/1e9;
	TimeSpacialHash += world->TimeSpacialHash;
	TimeCollisions += world->TimeCollisions;
	TimeUpdateEntity += world->TimeUpdateEntity;
	TimeUpdateColonies += world->TimeUpdateColonies;
}
void PollInput() {
	sf::Event event;
	while (window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window->close();
		}
		if (event.type == sf::Event::GainedFocus) {
			WindowFocused = true;
		}
		if (event.type == sf::Event::LostFocus) {
			WindowFocused = false;
		}
			
		if (event.type == sf::Event::Resized)
		{
			ScreenSize.x = event.size.width;
			ScreenSize.y = event.size.height;
		}
		if (event.type == sf::Event::MouseWheelMoved)
		{
			CameraWidth += event.mouseWheel.delta * 10;
			CameraWidth = std::max(CameraWidth, 1.0f);
		}
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Num1) {
				DisplayPheremone = 0;
			}
			if (event.key.code == sf::Keyboard::Num2) {
				DisplayPheremone = 1;
			}
			if (event.key.code == sf::Keyboard::Num3) {
				DisplayPheremone = 2;
			}
			if (event.key.code == sf::Keyboard::Num4) {
				DisplayPheremone = 3;
			}
			if (event.key.code == sf::Keyboard::Num5) {
				DisplayPheremone = 4;
			}
			if (event.key.code == sf::Keyboard::Num6) {
				DisplayPheremone = 5;
			}
			if (event.key.code == sf::Keyboard::Equal) {
				world->TimeScalingFactor *= 2;
				std::cout << "Time scale:" << world->TimeScalingFactor << "\n";
			}
			if (event.key.code == sf::Keyboard::Dash) {
				world->TimeScalingFactor /= 2;
				std::cout << "Time scale:" << world->TimeScalingFactor << "\n";
			}
			if (event.key.code == sf::Keyboard::R) {
				world = std::make_unique<World>();
				GraphRollingMax = 0;
			}

		}
		if (event.type == sf::Event::MouseMoved)
		{
			WorldMousePosOld = WorldMousePos;
			auto pos = sf::Mouse::getPosition(*window.get());
			glm::vec2 mpos(pos.x, pos.y);
			mpos.x /= ScreenSize.x;
			mpos.y /= ScreenSize.y;
			mpos.x -= 0.5;
			mpos.y -= 0.5;
			mpos *= 8.0f;
			WorldMousePos = CameraPosition;
			WorldMousePos += glm::vec2(mpos.x, mpos.y) * CameraWidth;
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == 0)
			{
				for (int j = 0; j < world->AntColonyMax; ++j)
				{
					if (glm::distance(WorldMousePos, world->ColonyArray[j].HiveLocation) < 20.0)
					{
						std::cout << "Colony stats for colony:" << j << "\n";
						std::cout << "Ant stats\n";
						std::cout << "Ant count:" << world->AffiliationCounter[j+1] << "\n";
						std::cout << "Energy stats\n";
						std::cout << "Energy: " << world->ColonyArray[j].TotalEnergy << "\n";
					}
				}
				world->PrintEntityInfo(WorldMousePos.x, WorldMousePos.y);
			}
		}
	}
	if (WindowFocused) {
		float CameraSpeed = (1.0f / (100.0 / CameraWidth)) * 300.0 * DeltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			CameraPosition.x += CameraSpeed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			CameraPosition.x -= CameraSpeed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			CameraPosition.y += CameraSpeed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			CameraPosition.y -= CameraSpeed;
		}
		CameraPosition = glm::clamp(CameraPosition, -glm::vec2(world->WorldSize, world->WorldSize), glm::vec2(world->WorldSize, world->WorldSize));
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			int size = 5;
			for (int x = -size; x <= size; ++x)
			{
				for (int y = -size; y <= size; ++y)
				{
					world->ColonyArray[0].Pheremone_Killed.GetStrength(WorldMousePos.x + x, WorldMousePos.y + y) = 100;
					//world->ColonyArray[0].Pheremone_Food.GetPheremone(WorldMousePos.x + x, WorldMousePos.y + y).Strength = 100;
					//world->ColonyArray[0].Pheremone_Food.GetPheremone(WorldMousePos.x + x, WorldMousePos.y + y).Direction = WorldMousePos - WorldMousePosOld;
				}
			}
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			int size = 5;
			for (int x = -size; x <= size; ++x)
			{
				for (int y = -size; y <= size; ++y)
				{
					world->ColonyArray[0].Pheremone_Food.GetStrength(WorldMousePos.x + x, WorldMousePos.y + y) = 0;
					world->ColonyArray[0].Pheremone_Home.GetStrength(WorldMousePos.x + x, WorldMousePos.y + y) = 0;
				}
			}
		}
	}
}
int main(int argc, char** args)
{

	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(ScreenSize.x,ScreenSize.y), "Ant test");
	shape.setFillColor(sf::Color::Green);
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	GraphArea.setSize(sf::Vector2(GraphWidth, GraphHeight));
	if (!PheremoneTexture->create(PHGridSize, PHGridSize))
	{
		std::cout << "Created pheremone tex failed\n";
	}
	//PheremoneMap.setTexture(PheremoneTexture.get());
	world = std::make_unique<World>();
	
	while (Running)
	{
		DtCounter = std::chrono::high_resolution_clock::now();
		PollInput();
		Update();
		Render();
		Running &= window->isOpen();
		FrameCount += DeltaTime;
		if (FrameCount >= 10)
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto dt = now - StartTime;
			StartTime = now;
			std::cout << "Frametime:" << (dt.count() / (1000.0 * 1000000000)) << "\n fps:" << ((1000.0 * 1000000000) / dt.count()) << "\n";
			std::cout << "Frame breakdown times:\n";
			std::cout << "Render time:" << RenderTime/(dt.count()/1e9)<< "\n";
			std::cout << "Update time:" << UpdateTime/(dt.count()/1e9) << "\n";
			std::cout << "Spacial hash time:" << 100*TimeSpacialHash/UpdateTime << "%\n";
			std::cout << "Collision time:" << 100*TimeCollisions/UpdateTime << "%\n";
			std::cout << "Entity update time:" << 100*TimeUpdateEntity/UpdateTime << "%\n";
			std::cout << "Colonie update time:" << 100*TimeUpdateColonies/UpdateTime << "%\n";
			std::cout << "Total ant count:" << std::accumulate(++world->AffiliationCounter.begin(), world->AffiliationCounter.end(),0)<<"\n";
			RenderTime = 0;
			UpdateTime = 0;
			TimeSpacialHash = 0;
			TimeCollisions = 0;
			TimeUpdateEntity = 0;
			TimeUpdateColonies = 0;
			FrameCount = 0;
			if (world->AffiliationCounter[1] + world->AffiliationCounter[2] == 0)
			{
				//Running = false;
			}
			
		}
		if (world->AntColonyCount <= 1) {
			float max = -1000;
			for (int i = 0; i < world->AntColonyMax; ++i)
			{
				float energy = world->ColonyArray[i].TotalEnergy + (world->AffiliationCounter[i + 1] * 100);
				if (world->ColonyArray[i].ColonyAlive && energy > max)
				{
					std::cout<<"Saved colony: "<<i<<"\n";
					world->ColonyArray[i].Write("BestColony.txt");
					max = energy;
				}

			}
			std::cout << "---------\nElapsed time: " << world->ElapsedTime<<"\n";
			world = std::make_unique<World>();
			GraphRollingMax = 0;
			world->TimeScalingFactor *= 128;
		}
		auto end = std::chrono::high_resolution_clock::now();
		DeltaTime = (end - DtCounter).count()/1e9;
		DtCounter = end;
	}
	if (world->AntColonyCount > 0) {
		float max = -1000;
		for (int i = 0; i < world->AntColonyMax; ++i)
		{
			float energy = world->ColonyArray[i].TotalEnergy + (world->AffiliationCounter[i + 1] * 100);
			if (world->ColonyArray[i].ColonyAlive && energy > max)
			{
				std::cout<<"Saved colony: "<<i<<"\n";
				world->ColonyArray[i].Write("BestColony.txt");
				max = energy;
			}

		}
	}
	return 0;
}