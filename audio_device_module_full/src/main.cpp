#include <opendaq/opendaq.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace daq;

int main()
{
	std::string modulePath = AUDIO_MODULE_DIR;
	auto instance = Instance(modulePath);

	for (const auto& dev : instance.getAvailableDeviceTypes())
		std::cout << dev.first << std::endl;
	
	// Connect to all devices
	for (const auto& dev : instance.getAvailableDevices())
	{
		std::cout << dev.getConnectionString() << std::endl;
		std::cout << instance.addDevice(dev.getConnectionString()).getName() << std::endl;
	}

	using namespace std::chrono_literals;

	// Create reader for 1st visible device signal
	auto signal = instance.getSignalsRecursive()[0];
	std::cout << signal.getName() << std::endl;
	auto reader = StreamReader(signal);
	
	// Allocate a buffer large enough to hold 50000 samples per second
	double data[10000];
	for (int i = 0; i < 10; ++i)
	{
		size_t cnt = reader.getAvailableCount();
		reader.read(&data, &cnt);

		// Print last value
		if (cnt > 0)
			std::cout << data[cnt - 1] << std::endl;

		std::this_thread::sleep_for(200ms);
	}

	return 0;
}