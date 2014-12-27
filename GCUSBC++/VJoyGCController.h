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
		VJoyGCController(VJoyDevice aDevice, const USBDriver& aDriver);

	private:
		VJoyDevice mDevice;
		Status mStatus;
		const USBDriver& mDriver;

		bool mEnabled;
		std::thread mThread;
		void mUpdateThread();
	};
}

#endif//_GCC_VJOYGCCONTROLLER_H_