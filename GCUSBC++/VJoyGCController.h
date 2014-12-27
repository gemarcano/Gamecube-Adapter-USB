#ifndef _GCC_VJOYGCCONTROLLER_H_
#define _GCC_VJOYGCCONTROLLER_H_

#include "USBDriver.h"
#include <thread>

namespace GCC
{
	typedef unsigned int VJoyDevice;

	class VJoyGCController
	{
	public:
		enum class Status
		{
			READY, OFF, ERR
		};
		VJoyGCController(VJoyDevice aDevice);
		void update(const GCController& aController);

	private:
		VJoyDevice mDevice;
		Status mStatus;
	};

	class VJoyGCControllers
	{
	public:
		VJoyGCControllers(const USBDriver& aDriver);
		void mUpdateThread();
	private:
		bool mEnabled;
		std::thread mThread;
		const USBDriver& mDriver;

		std::array<VJoyGCController, 4> mControllers;
	};
}

#endif//_GCC_VJOYGCCONTROLLER_H_