#pragma once
class GUIBase
{
public:
	bool EventRaised = false;
	int EventID = 0;
	virtual void Update();
	virtual void Render();
	void RaiseEvent(int id)
	{
		EventRaised = true;
		EventID = id;
	}
};

