#include <opendaq/opendaq.h>
#include <iostream>

using namespace daq;

int main()
{
	std::string modulePath = AUDIO_MODULE_DIR;
	auto instance = Instance(modulePath);

	for (const auto& dev : instance.getAvailableDeviceTypes())
		std::cout << dev.first << std::endl;
	
	for (const auto& dev : instance.getAvailableDevices())
		std::cout << dev.getConnectionString() << std::endl;

	return 0;
}