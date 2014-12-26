#ifndef _GCC_USBDRIVER_H_
#define _GCC_USBDRIVER_H_

#include "GCC.h"
#include "libusb.h"
#include <thread>

namespace GCC
{
	class USBDriver
	{
	public:
		enum class Status
		{
			READY, ERR
		};
		USBDriver();
		~USBDriver();

		GCController getState() const;
		GCController setState(const GCController& aController);
		Status getStatus() const;

	private:
		GCController mController;
		Status mStatus;
		libusb_context *mUSBContext;

	};
}
#endif//_GCC_USBDRIVER_H_