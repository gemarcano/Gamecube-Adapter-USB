
#pragma warning(disable:4996)
#include "GCC.h"
#include <vector>

#include "USBDriver.h"

#include <iostream>
#include <Windows.h>
#include "public.h"
#include "vjoyinterface.h"
#include "libusb.h"

#include <algorithm>
#include <cstdint>
#pragma warning(default:4996)

//WUP-028
//VENDORID 0x57E
//PRODUCT ID 0x337

const uint_fast16_t GCC_VENDOR_ID = 0x57E;
const uint_fast16_t GCC_PRODUCT_ID = 0x337;

namespace GCC
{

	std::vector<GCController> readRawInput(const std::array<unsigned char, 37>& data)
	{
		std::vector<GCController> results;
		auto port = std::begin(data) + 1;
		for (int i = 0; i < 4; port += 9, ++i)
		{
			std::array<unsigned char, 9> param;
			std::copy(port, port + param.size(), std::begin(param));
			results.emplace_back(GCController(param));
		}
		return results;
	}

	USBDriver::USBDriver()
	{ 
		int s = libusb_init(&mUSBContext);
		libusb_set_debug(mUSBContext, 3);

		libusb_device_handle *handle = libusb_open_device_with_vid_pid(mUSBContext, GCC_VENDOR_ID, GCC_PRODUCT_ID);

		if (!handle)
		{
			mStatus = Status::ERR;
		}
		
		s = libusb_set_configuration(handle, 1); //I don't know why it is 1, but that's what I'm seeing elsewhere being used...
		s = libusb_claim_interface(handle, 0);

		s = libusb_control_transfer(handle, 0x21, 11, 0x0001, 0, nullptr, 0, 0);

		std::array<unsigned char, 37> data = { 0x13 };
		
		int transferred = 0;
		s = libusb_bulk_transfer(handle, 2, data.data(), 1, &transferred, 0); //Send 0x13 to begin controller polling

		/*s = libusb_bulk_transfer(handle, 0x81, data.data(), 37, &transferred, 0); //Read data (Bulk)
		readRawInput(data);

		s = libusb_bulk_transfer(handle, 0x81, data.data(), 37, &transferred, 0); //Read data (Bulk)
		readRawInput(data);*/




		using namespace std;
		UINT joyInterface = 1; //This is the default interface

		// Get the driver attributes (Vendor ID, Product ID, Version Number)
		if (!vJoyEnabled())
		{
			mStatus = Status::ERR;
			return;
		}
		else
		{
			printf("Vendor: %S\nProduct :%S\nVersion Number:%S\n", (const char*)((GetvJoyManufacturerString())), (const char*)(GetvJoyProductString()), (const char*)(GetvJoySerialNumberString()));
		};

		// Get the state of the requested device
		VjdStat status = GetVJDStatus(joyInterface);
		switch (status)
		{
		case VJD_STAT_OWN:
			cout << "vJoy Device " << joyInterface << " %d is already owned by this feeder\n";
			break;
		case VJD_STAT_FREE:
			printf("vJoy Device %d is free\n", joyInterface);
			break;
		case VJD_STAT_BUSY:
			cout << "vJoy Device " << joyInterface << " is already owned by another feeder\nCannot continue\n";
			return;
		case VJD_STAT_MISS:
			printf("vJoy Device %d is not installed or disabled\nCannot continue\n", joyInterface);
			return;
		default:
			printf("vJoy Device %d general error\nCannot continue\n", joyInterface);
			return;
		};

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
		if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(joyInterface))))
		{
			printf("Failed to acquire vJoy device number %d.\n", joyInterface);
			return;
		}
		else
		{
			printf("Acquired: vJoy device number %d.\n", joyInterface);
		}
	}

	USBDriver::~USBDriver()
	{
		libusb_exit(mUSBContext);
	}

}
