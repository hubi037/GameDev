/**
* LuaScriptManager.h
*/

#pragma once

#include <luabind/luabind.hpp>

namespace scripting
{
	class Manager
	{
	public:

		Manager();
		~Manager();
		Manager(const Manager &other);
		Manager& operator= (const Manager &other);

		void runScriptFile(const std::string& file);
		void runScript(const std::string& code);

		bool hasGlobalFunction(const std::string& funcName);
		luabind::object GetGlobalTable() const;
		const lua_State* GetInterpreter() const;

		template <class RET, class ARG1>
		RET callFunction(const char* funcName, ARG1 arg1)
		{
			return luabind::call_function<RET>(mMasterState, funcName, arg1);
		}

		template <class RET, class ARG1, class ARG2>
		RET callFunction(const char* funcName, ARG1 arg1, ARG2 arg2)
		{
			return luabind::call_function<RET>(mMasterState, funcName, arg1, arg2);
		}

		template <class ARG1, class ARG2>
		void callFunctionVoid(const char* funcName, ARG1 arg1, ARG2 arg2)
		{
			luabind::call_function<void>(mMasterState, funcName, arg1, arg2);
		}

	private:
		lua_State* mMasterState;

		static int panicHandler(lua_State* L);
		static int loadLuaLibs(lua_State *L);
		static const char* chunkReader(lua_State *L, void *data, size_t *size);
	};
}