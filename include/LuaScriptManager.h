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
	private:
		lua_State* mMasterState;

		static int panicHandler(lua_State* L);
		static int loadLuaLibs(lua_State *L);
		static const char* chunkReader(lua_State *L, void *data, size_t *size);
	};
}