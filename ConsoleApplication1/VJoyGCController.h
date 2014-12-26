#ifndef _GCC_VJOYGCCONTROLLER_H_
#define _GCC_VJOYGCCONTROLLER_H_

#include "USBDriver.h"
#include <thread>

namespace GCC
{
	class VJoyGCController
	{
	public:
		enum class Status
		{
			READY, OFF, ERR
		};
		VJoyGCController(const USBDriver& aDriver);

	private:
		Status mStatus;
		const USBDriver& mDriver;

		bool mEnabled;
		std::thread mThread;
	};
}

#endif//_GCC_VJOYGCCONTROLLER_H_