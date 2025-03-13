#include "Emulator.hpp"

int Emulator::emulate(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	char const *romFilename = argv[2];
	Graphics platform("CHIP-8 Emulator");
	Chip8 chip8;
	chip8.LoadROM(romFilename);
	platform.setCycleDelay(std::stoi(argv[1]));

	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		
		quit = platform.ProcessInput(chip8.keypad);
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		int cycle = platform.getCycleDelay();
		if (dt > cycle)
		{
			lastCycleTime = currentTime;
			chip8.Cycle();
			//Display
			platform.Update(chip8.video, videoPitch);
			platform.DrawDebugBordrer();
			platform.DisplayRegisters(chip8.getRegisters());
			platform.DisplayStack(chip8.getStack());
			platform.DisplayPC(chip8.getPC());
			platform.DisplaySP(chip8.getSP());
			platform.DisplayCycleDelay();
			platform.DisplayMemory(chip8.getMemory());
			platform.DistplayInstructions(chip8.getInstruction());
			platform.EndDraw();
		}
	}
	return 0;
}
