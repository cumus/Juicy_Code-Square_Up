#include "Console.h"
#include "Log.h"
#include "FontManager.h"
#include "Application.h"

Console::Console() : Module("console")
{
	
}

Console::~Console()
{
}

bool Console::Awake()
{
	LOG("Starting Console");

	return true;
}

bool Console::Start()
{
	opened = false;
	

	return true;
}

bool Console::CleanUp()
{
	return false;
}

bool Console::Update(float dt)
{
	return false;
}

void Console::Write(const char*)
{
}

Commands Console::ReturnCommands(Commands)
{
	return Commands();
}

void Console::Open()
{
}

void Console::Close()
{
}

