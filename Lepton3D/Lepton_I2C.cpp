#include "Lepton_I2C.h"

#include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"
#include "leptonSDKEmb32PUB/LEPTON_AGC.h"

#include <stdio.h>

bool _connected = false;

LEP_CAMERA_PORT_DESC_T _port;

int lepton_connect() {
	LEP_RESULT result = LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
	if (result != LEP_OK) printf("LEP_OpenPort(): %i\n", result);
	_connected = true;
	return 0;
}

void lepton_perform_ffc() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_RunSysFFCNormalization(&_port);
	if (result != LEP_OK) printf("LEP_RunSysFFCNormalization(): %i\n", result);
}

void lepton_toggle_agc() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_AGC_ENABLE_E _isAGCEnabled;

	LEP_RESULT result = LEP_GetAgcEnableState(&_port, &_isAGCEnabled);
	if (result != LEP_OK) printf("LEP_GetAgcEnableState(): %i\n", result);

	if (_isAGCEnabled == LEP_AGC_DISABLE) {
		_isAGCEnabled = LEP_AGC_ENABLE;
		printf("AGC is now enabled.\n");
	}
	else {
		_isAGCEnabled = LEP_AGC_DISABLE;
		printf("AGC is now disabled.\n");
	}

	result = LEP_SetAgcEnableState(&_port, _isAGCEnabled);
	if (result != LEP_OK) printf("LEP_SetAgcEnableState(): %i\n", result);
}

//presumably more commands could go here if desired
