#include <cstdint>
#include <atomic>
#include "GCC.h"

//[0] joystick enabled
//[1] upper end D-Pad, lower end a,b,x,y
//[2] R button, L Button, z, start
//[3] analog X
//[4] analog Y
//[5] c-stick X
//[6] c-stick Y
//[7] L axis
//[8] R Axis

//[1] [0]: A
//    [1]: B
//    [2]: X
//    [3]: Y
//    [4]: Left
//    [5]: Right
//    [6]: Down
//    [7]: Up

//[2] [0]: start
//    [1]: z
//    [2]: R button
//    [3]: L Button
//    [4]: not used
//    [5]: not used
//    [6]: not used
//    [7]: not used

// aData 1: bytes 02-09
// aData 2: bytes 11-17
// aData 3: bytes 20-27
// aData 4: bytes 29-36l

namespace GCC
{
	GCController::GCController()
		:enabled(false), rumble(false), buttons{}, axis{}
	{
		
	}

	//Taken from https://bitbucket.org/elmassivo/gcn-usb-adapter/src
	// PORT 1: bytes 02-09
	// PORT 2: bytes 11-17
	// PORT 3: bytes 20-27
	// PORT 4: bytes 29-36l

	//[0] joystick enabled
	//[1] upper end D-Pad, lower end a,b,x,y
	//[2] R button, L Button, z, start
	//[3] analog X
	//[4] analog Y
	//[5] c-stick X
	//[6] c-stick Y
	//[7] L axis
	//[8] R Axis

	//[1] [0]: A
	//    [1]: B
	//    [2]: X
	//    [3]: Y
	//    [4]: Left
	//    [5]: Right
	//    [6]: Down
	//    [7]: Up

	//[2] [0]: start
	//    [1]: z
	//    [2]: R button
	//    [3]: L Button
	//    [4]: not used
	//    [5]: not used
	//    [6]: not used
	//    [7]: not used
	GCController::GCController(const std::array<unsigned char, 9>& aData)
	{
		enabled = !!aData[0];
		axis.left_x = aData[3];
		axis.left_y = aData[4];
		axis.right_x = aData[5];
		axis.right_y = aData[6];
		axis.l_axis = aData[7];
		axis.r_axis = aData[8];

		buttons.a = !!(aData[1] & 0x01);
		buttons.b = !!(aData[1] & (1 << 1));
		buttons.x = !!(aData[1] & (1 << 2));
		buttons.y = !!(aData[1] & (1 << 3));
		buttons.dpad_left = !!(aData[1] & (1 << 4));
		buttons.dpad_right = !!(aData[1] & (1 << 5));
		buttons.dpad_down = !!(aData[1] & (1 << 6));
		buttons.dpad_up = !!(aData[1] & (1 << 7));
	}
}