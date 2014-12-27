
#pragma warning(disable:4996)
#include "GCController.h"
#include <vector>

#include "USBDriver.h"

#include <iostream>
#include <Windows.h>
#include "public.h"
#include "vjoyinterface.h"
#include "libusb.h"

#include <algorithm>
#include <cstdint>
#include <mutex>
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
		:mPollRate(1000)
	{ 
		int s = libusb_init(&mUSBContext);

		if (s < 0)
		{
			mStatus = Status::ERR;
			return;
		}

		libusb_set_debug(mUSBContext, 3);

		libusb_device_handle *handle = libusb_open_device_with_vid_pid(mUSBContext, GCC_VENDOR_ID, GCC_PRODUCT_ID);

		if (!handle)
		{
			mStatus = Status::ERR;
			return;
		}
		
		if (
			libusb_set_configuration(handle, 1) < 0 || //I don't know why it is 1, but that's what I'm seeing elsewhere being used...
			libusb_claim_interface(handle, 0) < 0 ||
			libusb_control_transfer(handle, 0x21, 11, 0x0001, 0, nullptr, 0, 0) < 0
			) 
		{
			mStatus = Status::ERR;
			return;
		}
		
		std::array<unsigned char, 37> data = { 0x13 };
		
		int transferred = 0;
		if (libusb_bulk_transfer(handle, 2, data.data(), 1, &transferred, 0) < 0) //Send 0x13 to begin controller polling
		{
			mStatus = Status::ERR;
			return;
		}

		/*s = libusb_bulk_transfer(handle, 0x81, data.data(), 37, &transferred, 0); //Read data (Bulk)
		readRawInput(data);

		s = libusb_bulk_transfer(handle, 0x81, data.data(), 37, &transferred, 0); //Read data (Bulk)
		readRawInput(data);*/

		//Setup driver thread
		auto driverThread = [this, handle]()
		{
			int s = 0, transferred = 0;
			std::array<unsigned char, 37> data{};
			while (mEnabled)
			{
				std::chrono::milliseconds pollInterval(static_cast<long long>(1000. / mPollRate));
				auto start = std::chrono::steady_clock::now();
				
				//Get current status
				s = libusb_bulk_transfer(handle, 0x81, data.data(), data.size(), &transferred, 0); //FIXME have this time out?
				auto end = std::chrono::steady_clock::now();

				double count = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

				if (s == 0)
				{
					std::unique_lock<std::mutex> lock(mMutex);
					mControllers = readRawInput(data);
				}
				
				//assert(count < 1./mPollRate );
				std::this_thread::sleep_for(pollInterval - std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
			}

		};

		mEnabled = true;
		mThread.swap(std::thread(driverThread));
		
	}

	USBDriver::~USBDriver()
	{
		mEnabled = false;
		mThread.join();
		libusb_exit(mUSBContext);
	}

	std::vector<GCController> USBDriver::getState() const
	{
		std::vector<GCController> result;
		std::unique_lock<std::mutex> lock(mMutex);
		result = mControllers;
		lock.unlock();
		return result;
	}

	USBDriver::Status USBDriver::getStatus() const
	{
		return mStatus;
	}

}
