#include "stdafx.h"
#include "LuaScriptManager.h"
#include <strstream>
#include <iostream>
#include <fstream>

extern "C"
{
	#include <lauxlib.h> 
	#include <lualib.h>
}

#include <luabind/object.hpp>
#include <luabind/class_info.hpp>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
#endif

#include "MathExports.h"
#include "OgreExports.h"

namespace scripting
{
	//-----------------------------------------------------------------------
	Manager::Manager() : mMasterState(NULL)
	{
		luabind::disable_super_deprecation();

		mMasterState = lua_open(); 
		lua_cpcall(mMasterState, &Manager::loadLuaLibs, NULL);
		lua_atpanic(mMasterState, &Manager::panicHandler);

		luabind::open(mMasterState);

		lua_gc(mMasterState, LUA_GCSTOP, 0);
		MathExports::extendState(mMasterState);
		OgreExports::extendState(mMasterState);
		lua_gc(mMasterState, LUA_GCRESTART, 0);

		luabind::bind_class_info(mMasterState);
	}
	//-----------------------------------------------------------------------	
	Manager::~Manager()
	{
		lua_close(mMasterState);
	}
	//-----------------------------------------------------------------------
	Manager::Manager(const Manager &other)
	{
		*this = other;
	}
	//-----------------------------------------------------------------------
	Manager& Manager::operator= (const Manager &other)
	{
		if (this == &other)
			return *this;

		mMasterState = other.mMasterState;
		return *this;
	}
	//-----------------------------------------------------------------------
	int Manager::panicHandler(lua_State* L)
	{
		std::string str = lua_tostring(L, 1);
#ifdef WIN32
		MessageBox(NULL, str.c_str(), "Scripting error...", MB_ICONERROR | MB_OKCANCEL);
#else
		std::cout << "Scripting error: " << str << std::endl;
#endif
		return 0;
	}
	//-----------------------------------------------------------------------
	int Manager::loadLuaLibs (lua_State *L)
	{
		lua_gc(L, LUA_GCSTOP, 0);  // stop collector during initialization 
		luaL_openlibs(L);  // open libraries 
		luaopen_debug(L);
		lua_gc(L, LUA_GCRESTART, 0);
		return 0;
	}
	//-----------------------------------------------------------------------
	void Manager::runScriptFile(const std::string& file)
	{
		std::ifstream ifs(file);
		runScript(std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>()));
	}
	//-----------------------------------------------------------------------
	void Manager::runScript(const std::string& code)
	{
		std::strstream luaCodeReader;
		luaCodeReader << code;

		if(lua_load( mMasterState, &Manager::chunkReader, &luaCodeReader, "LuaScriptManager" ) == 0) 
		{
			if (lua_pcall(mMasterState, 0, 0, 0) != 0)
			{
				std::string errStr = lua_tostring(mMasterState, -1);
#ifdef WIN32
				MessageBox(NULL, errStr.c_str(), "Scripting error...", MB_ICONERROR | MB_OKCANCEL);
#else
				std::cout << "Scripting error: " << errStr << std::endl;
#endif
			}
		}
		else
		{
			// the top of the stack should be the error string
			if (!lua_isstring(mMasterState, lua_gettop(mMasterState)))
				return;

			// get the top of the stack as the error and pop it off
			std::string errStr = lua_tostring(mMasterState, lua_gettop(mMasterState));
			lua_pop(mMasterState, 1);

#ifdef WIN32
			MessageBox(NULL, errStr.c_str(), "Scripting error...", MB_ICONERROR | MB_OKCANCEL);
#else
			std::cout << "Scripting error: " << errStr << std::endl;
#endif
		}
	}
	//-----------------------------------------------------------------------
	const char* Manager::chunkReader(lua_State *L, void *data, size_t *size)
	{
		const size_t kBufferSize = 1024;
		static char s_buffer[kBufferSize];

		std::istream* pis = reinterpret_cast<std::istream*>(data);
		if ( !pis )
			return NULL;
		pis->read( &s_buffer[0], kBufferSize );
		*size = (size_t)pis->gcount();
		return s_buffer;
	}
	//-----------------------------------------------------------------------
	luabind::object Manager::GetGlobalTable() const
	{
		return luabind::globals(mMasterState);
	}
	//-----------------------------------------------------------------------
	bool Manager::hasGlobalFunction(const std::string& funcName)
	{
		lua_getfield(mMasterState, LUA_GLOBALSINDEX, funcName.c_str());
		bool bFound = !lua_isnil(mMasterState, -1);
		lua_pop(mMasterState,1);
		return bFound;
	}
	//-----------------------------------------------------------------------
	const lua_State* Manager::GetInterpreter() const
	{
		return mMasterState;
	}
	//-----------------------------------------------------------------------
}