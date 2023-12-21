#pragma once

class Main
{
public:

	static Main* GetSingleton()
	{
		static Main main;
		return &main;
	}

	void Setup();
	void SetupLog();

};
