#ifndef _SEGFAULT_HANDLER_HPP_
#define _SEGFAULT_HANDLER_HPP_

#define NAPI_VERSION 9

#include <addon-tools.hpp>

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
