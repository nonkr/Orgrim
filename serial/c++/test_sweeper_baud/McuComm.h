#pragma once

#include "McuProtocol.h"
#include <string>

#define Sleep(X) std::this_thread::sleep_for(std::chrono::milliseconds((int)(X)))

namespace mcu_comm
{
	bool Init();
	std::string GetMcuVertion();
	bool SetShutdown(uint16_t timeDelayMs);
	bool SetRestart(uint16_t timeDelayMs);
	bool SetDormant(uint16_t timeDelayMs);
	bool SetWakeup();
	bool SetUpgrade();
	bool SetMotionReg(const Host2McuMotionReg& motionReg);
	bool SetSpeedParamReg(const Host2McuSpeedParamReg& speedReg);
	bool SetSensorReg(const Host2McuSensorReg& sensorReg);
	bool SetHostDebugReg(const Host2McuDebugReg& debugReg);
}
