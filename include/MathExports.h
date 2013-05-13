#pragma once

// Library Include
extern "C"
{
	#include <lua.h> 
}

namespace scripting
{
	class MathExports
	{
	public:
		static void extendState(lua_State* pState);
	};
}