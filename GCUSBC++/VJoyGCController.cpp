
#include <Windows.h>
#include <public.h>
#include <vjoyinterface.h>
#include <cstdio>
#include <iostream>

#include "USBDriver.h"
#include "VJoyGCController.h"

namespace GCC
{

	std::array<VJoyDevice, 4> mDevices{ {1,2,3,4} };
	VJoyGCController::VJoyGCController(VJoyDevice aDevice, const USBDriver& aDriver)
		:mDevice(aDevice), mStatus(Status::OFF), mDriver(aDriver)
	{
		//Set up joystick
		if (!vJoyEnabled())
		{
			mStatus = Status::ERR;
			return;
		}
		//TODO check that the controller selected is a good fit!
		
		VjdStat status = GetVJDStatus(mDevice);

		switch (status)
		{
		case VJD_STAT_OWN:
			std::cout << "vJoy Device " << mDevice << " %d is already owned by this feeder\n";
			break;
		case VJD_STAT_FREE:
			printf("vJoy Device %d is free\n", mDevice);
			break;
		case VJD_STAT_BUSY:
			std::cout << "vJoy Device " << mDevice << " is already owned by another feeder\nCannot continue\n";
			return;
		case VJD_STAT_MISS:
			printf("vJoy Device %d is not installed or disabled\nCannot continue\n", mDevice);
			return;
		default:
			printf("vJoy Device %d general error\nCannot continue\n", mDevice);
			return;
		}

		if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(mDevice))))
		{
			printf("Failed to acquire vJoy device number %d.\n", mDevice);
			return;
		}
		else
		{
			printf("Acquired: vJoy device number %d.\n", mDevice);
		}

		//setup thread
		

		mThread = std::thread(&VJoyGCController::mUpdateThread, this);
	}

	void VJoyGCController::mUpdateThread()
	{
		
		int s = 0;
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

				s = SetBtn(controller.buttons.a, mDevice, 1);
				s = SetBtn(controller.buttons.b, mDevice, 2);
				s = SetBtn(controller.buttons.x, mDevice, 3);
				s = SetBtn(controller.buttons.y, mDevice, 4);

				s = SetBtn(controller.buttons.dpad_left, mDevice, 5);
				s = SetBtn(controller.buttons.dpad_up, mDevice, 6);
				s = SetBtn(controller.buttons.dpad_right, mDevice, 7);
				s = SetBtn(controller.buttons.dpad_down, mDevice, 8);

				s = SetBtn(controller.buttons.l_shoulder, mDevice, 9);
				s = SetBtn(controller.buttons.r_shoulder, mDevice, 10);
				s = SetBtn(controller.buttons.z, mDevice, 11);
				s = SetBtn(controller.buttons.start, mDevice, 12);

				//set axis
				s = SetAxis(controller.axis.left_x * 127, mDevice, HID_USAGE_X);
				s = SetAxis((255 - controller.axis.left_y) * 127, mDevice, HID_USAGE_Y);
				s = SetAxis(controller.axis.right_x * 127, mDevice, HID_USAGE_RX);
				s = SetAxis(controller.axis.right_y * 127, mDevice, HID_USAGE_RY);
				s = SetAxis(controller.axis.l_axis * 127, mDevice, HID_USAGE_SL0);
				s = SetAxis(controller.axis.r_axis * 127, mDevice, HID_USAGE_SL1);
			}

		}
	}
}
