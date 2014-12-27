#include <iostream>
#include <Windows.h>
#include "public.h"
#include "vjoyinterface.h"
#include "USBDriver.h"
#include "VJoyGCController.h"

using namespace std;
using namespace GCC;

int main(int argc, char *argv[])
{
	USBDriver driver;
	VJoyGCControllers joys(driver);

	for (;;)
	{
		auto controllers = driver.getState();
		
		for (int i = 0; i < controllers.size(); ++i)
		{ 
			cout << "Control " << i << " enabled: " << controllers[i].enabled << endl;
			cout << "Power " << i << " enabled: " << controllers[i].powered << endl;
			cout << "Buttons:" << endl;
			cout << "\tA: " << controllers[i].buttons.a << endl;
			cout << "\tB: " << controllers[i].buttons.b << endl;
			cout << "\tY: " << controllers[i].buttons.y << endl;
			cout << "\tX: " << controllers[i].buttons.x << endl;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
			
	}
	return 0;
}