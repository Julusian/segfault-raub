#ifndef _SEGFAULT_HANDLER_HPP_
#define _SEGFAULT_HANDLER_HPP_

#define NAPI_VERSION 9
#define NODE_ADDON_API_DISABLE_DEPRECATED
#define NAPI_DISABLE_CPP_EXCEPTIONS

#include <napi.h>

namespace segfault
{
	void init();
	void registerHandler();

	Napi::Value causeSegfault(const Napi::CallbackInfo &info);
	Napi::Value causeDivisionInt(const Napi::CallbackInfo &info);
	Napi::Value causeOverflow(const Napi::CallbackInfo &info);
	Napi::Value causeIllegal(const Napi::CallbackInfo &info);
	Napi::Value setSignal(const Napi::CallbackInfo &info);
}

#endif /* _SEGFAULT_HANDLER_HPP_ */
