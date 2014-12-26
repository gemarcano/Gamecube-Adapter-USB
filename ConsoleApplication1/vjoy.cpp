
#include <Windows.h>
#include <public.h>
#include <vjoyinterface.h>
#include <cstdio>
#include <iostream>

#include "USBDriver.h"
#include "VJoyGCController.h"

namespace GCC
{
	VJoyGCController::VJoyGCController(const USBDriver& aDriver)
	:mStatus(Status::OFF), mDriver(aDriver)
	{
		//Set up joystick
		if (!vJoyEnabled())
		{
			mStatus = Status::ERR;
			return;
		}
		
		UINT joyInterface = 2;
		VjdStat status = GetVJDStatus(joyInterface);

		switch (status)
		{
		case VJD_STAT_OWN:
			std::cout << "vJoy Device " << joyInterface << " %d is already owned by this feeder\n";
			break;
		case VJD_STAT_FREE:
			printf("vJoy Device %d is free\n", joyInterface);
			break;
		case VJD_STAT_BUSY:
			std::cout << "vJoy Device " << joyInterface << " is already owned by another feeder\nCannot continue\n";
			return;
		case VJD_STAT_MISS:
			printf("vJoy Device %d is not installed or disabled\nCannot continue\n", joyInterface);
			return;
		default:
			printf("vJoy Device %d general error\nCannot continue\n", joyInterface);
			return;
		}

		if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(joyInterface))))
		{
			printf("Failed to acquire vJoy device number %d.\n", joyInterface);
			return;
		}
		else
		{
			printf("Acquired: vJoy device number %d.\n", joyInterface);
		}

		//setup thread
		auto updateThread = [this, joyInterface]()
		{
			int s = 0;
			int nButtons = GetVJDButtonNumber(joyInterface);
			while (mEnabled)
			{
				//Grab data from driver
				auto controllers = mDriver.getState();
				for (int i = 0; i < controllers.size(); ++i)
				{
					
					const GCController& controller = controllers[i];
					if (!controller.enabled)
					{
						continue;
					}

					s = SetBtn(controller.buttons.a, joyInterface, 1);
					s = SetBtn(controller.buttons.b, joyInterface, 2);
					s = SetBtn(controller.buttons.x, joyInterface, 3);
					s = SetBtn(controller.buttons.y, joyInterface, 4);

					s = SetBtn(controller.buttons.dpad_left, joyInterface, 5);
					s = SetBtn(controller.buttons.dpad_up, joyInterface, 6);
					s = SetBtn(controller.buttons.dpad_right, joyInterface, 7);
					s = SetBtn(controller.buttons.dpad_down, joyInterface, 8);

					s = SetBtn(controller.buttons.l_shoulder, joyInterface, 9);
					s = SetBtn(controller.buttons.r_shoulder, joyInterface, 10);
					s = SetBtn(controller.buttons.z, joyInterface, 11);
					
					//set axis
					s = SetAxis(controller.axis.left_x * 127, joyInterface, HID_USAGE_X);
					s = SetAxis((255-controller.axis.left_y) * 127, joyInterface, HID_USAGE_Y);
					s = SetAxis(controller.axis.right_x * 127, joyInterface, HID_USAGE_RX);
					s = SetAxis(controller.axis.right_y * 127, joyInterface, HID_USAGE_RY);
					s = SetAxis(controller.axis.l_axis * 127, joyInterface, HID_USAGE_SL0);
					s = SetAxis(controller.axis.r_axis * 127, joyInterface, HID_USAGE_SL1);
				}

			}
		};

		mThread.swap(std::thread(updateThread));
	}

	void tmp()
	{
		using namespace std;
		UINT joyInterface = 1; //This is the default interface

		// Get the state of the requested device
		VjdStat status = GetVJDStatus(joyInterface);
		

		// Check which axes are supported
		BOOL AxisX = GetVJDAxisExist(joyInterface, HID_USAGE_X);
		BOOL AxisY = GetVJDAxisExist(joyInterface, HID_USAGE_Y);
		BOOL AxisZ = GetVJDAxisExist(joyInterface, HID_USAGE_Z);
		BOOL AxisRX = GetVJDAxisExist(joyInterface, HID_USAGE_RX);
		BOOL AxisRZ = GetVJDAxisExist(joyInterface, HID_USAGE_RZ);
		// Get the number of buttons and POV Hat switchessupported by this vJoy device
		int nButtons = GetVJDButtonNumber(joyInterface);
		int ContPovNumber = GetVJDContPovNumber(joyInterface);
		int DiscPovNumber = GetVJDDiscPovNumber(joyInterface);

		// Print results
		printf("\nvJoy Device %d capabilities:\n", joyInterface);
		printf("Numner of buttons\t\t%d\n", nButtons);
		printf("Numner of Continuous POVs\t%d\n", ContPovNumber);
		printf("Numner of Descrete POVs\t\t%d\n", DiscPovNumber);
		printf("Axis X\t\t%s\n", AxisX ? "Yes" : "No");
		printf("Axis Y\t\t%s\n", AxisX ? "Yes" : "No");
		printf("Axis Z\t\t%s\n", AxisX ? "Yes" : "No");
		printf("Axis Rx\t\t%s\n", AxisRX ? "Yes" : "No");
		printf("Axis Rz\t\t%s\n", AxisRZ ? "Yes" : "No");

		// Acquire the target
		
	
	}
}