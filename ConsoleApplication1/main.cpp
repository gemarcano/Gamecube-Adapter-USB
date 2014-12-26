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
	/*USHORT X, Y, Z, ZR, XR;							// Position of several axes

	X = 20;
	Y = 30;
	Z = 40;
	XR = 60;
	ZR = 80;

	JOYSTICK_POSITION	iReport;					// The structure that holds the full position data
	BYTE id = 1;										// ID of the target vjoy device (Default is 1)
	UINT iInterface = 1;								// Default target vJoy device
	BOOL ContinuousPOV = FALSE;						// Continuous POV hat (or 4-direction POV Hat)
	int count = 0;


	long value = 0;
	BOOL res = FALSE;

	USBDriver driver;

	// Reset this device to default values
	ResetVJD(iInterface);

	// Feed the device in endless loop
	int ContPovNumber = GetVJDContPovNumber(iInterface);
	int DiscPovNumber = GetVJDDiscPovNumber(iInterface);
	while (1)
	{
		// Set position of 4 axes
		res = SetAxis(X, iInterface, HID_USAGE_X);
		res = SetAxis(Y, iInterface, HID_USAGE_Y);
		res = SetAxis(Z, iInterface, HID_USAGE_Z);
		res = SetAxis(XR, iInterface, HID_USAGE_RX);
		res = SetAxis(ZR, iInterface, HID_USAGE_RZ);

		// Press/Release Buttons
		res = SetBtn(TRUE, iInterface, count / 50);
		res = SetBtn(FALSE, iInterface, 1 + count / 50);

		// If Continuous POV hat switches installed - make them go round
		// For high values - put the switches in neutral state
		if (ContPovNumber)
		{
			if ((count * 70) < 30000)
			{
				res = SetContPov((DWORD)(count * 70), iInterface, 1);
				res = SetContPov((DWORD)(count * 70) + 2000, iInterface, 2);
				res = SetContPov((DWORD)(count * 70) + 4000, iInterface, 3);
				res = SetContPov((DWORD)(count * 70) + 6000, iInterface, 4);
			}
			else
			{
				res = SetContPov(-1, iInterface, 1);
				res = SetContPov(-1, iInterface, 2);
				res = SetContPov(-1, iInterface, 3);
				res = SetContPov(-1, iInterface, 4);
			};
		};

		// If Discrete POV hat switches installed - make them go round
		// From time to time - put the switches in neutral state
		if (DiscPovNumber)
		{
			if (count < 550)
			{
				SetDiscPov(((count / 20) + 0) % 4, iInterface, 1);
				SetDiscPov(((count / 20) + 1) % 4, iInterface, 2);
				SetDiscPov(((count / 20) + 2) % 4, iInterface, 3);
				SetDiscPov(((count / 20) + 3) % 4, iInterface, 4);
			}
			else
			{
				SetDiscPov(-1, iInterface, 1);
				SetDiscPov(-1, iInterface, 2);
				SetDiscPov(-1, iInterface, 3);
				SetDiscPov(-1, iInterface, 4);
			};
		};

		Sleep(20);
		X += 150;
		Y += 250;
		Z += 350;
		ZR -= 200;
		count++;
		if (count > 640) count = 0;
	} // While

	cout << "OK\n";

	return 0;*/

	USBDriver driver;
	VJoyGCController joy(driver);

	for (;;)
	{
		auto controllers = driver.getState();
		
		for (int i = 0; i < controllers.size(); ++i)
		{ 
			cout << "Control " << i << " enabled: " << controllers[i].enabled << endl;
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