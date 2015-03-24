/************************************************************************
* Minetest-c55
* Copyright (C) 2010 celeron55, Perttu Ahola <celeron55@gmail.com>
*
* porting.cpp
* voxelands - 3d voxel world sandbox game
* Copyright (C) Lisa 'darkrose' Milne 2014 <lisa@ltmnet.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*
* License updated from GPLv2 or later to GPLv3 or later by Lisa Milne
* for Voxelands.
************************************************************************/

/*
	Random portability stuff

	See comments in porting.h
*/

#include "porting.h"
#include "config.h"
#include "debug.h"
#include "filesys.h"

#ifdef __APPLE__
	#include "CoreFoundation/CoreFoundation.h"
#endif

namespace porting
{

/*
	Signal handler (grabs Ctrl-C on POSIX systems)
*/

bool g_killed = false;

bool * signal_handler_killstatus(void)
{
	return &g_killed;
}

#if !defined(_WIN32) // POSIX
	#include <signal.h>

void sigint_handler(int sig)
{
	if(g_killed == false)
	{
		dstream<<DTIME<<"INFO: sigint_handler(): "
				<<"Ctrl-C pressed, shutting down."<<std::endl;

		dstream<<DTIME<<"INFO: sigint_handler(): "
				<<"Printing debug stacks"<<std::endl;
		debug_stacks_print();

		g_killed = true;
	}
	else
	{
		(void)signal(SIGINT, SIG_DFL);
	}
}

void signal_handler_init(void)
{
	dstream<<"signal_handler_init()"<<std::endl;
	(void)signal(SIGINT, sigint_handler);
}

#else // _WIN32
	#include <signal.h>
	#include <windows.h>

	BOOL WINAPI event_handler(DWORD sig)
	{
		switch(sig)
		{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:

			if(g_killed == false)
			{
				dstream<<DTIME<<"INFO: event_handler(): "
						<<"Ctrl+C, Close Event, Logoff Event or Shutdown Event, shutting down."<<std::endl;
				dstream<<DTIME<<"INFO: event_handler(): "
						<<"Printing debug stacks"<<std::endl;
				debug_stacks_print();

				g_killed = true;
			}
			else
			{
				(void)signal(SIGINT, SIG_DFL);
			}

			break;
		case CTRL_BREAK_EVENT:
			break;
		}

		return TRUE;
	}

void signal_handler_init(void)
{
	dstream<<"signal_handler_init()"<<std::endl;
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE)event_handler,TRUE);
}

#endif

/*
	Path mangler
*/

std::string path_data = ".." DIR_DELIM "data";
std::string path_userdata = "..";

std::string getDataPath(const char *subpath)
{
	return path_data + DIR_DELIM + subpath;
}

void pathRemoveFile(char *path, char delim)
{
	// Remove filename and path delimiter
	int i;
	for(i = strlen(path)-1; i>=0; i--)
	{
		if(path[i] == delim)
			break;
	}
	path[i] = 0;
}

void initializePaths()
{
#ifdef RUN_IN_PLACE
	/*
		Use relative paths if RUN_IN_PLACE
	*/

	dstream<<"Using relative paths (RUN_IN_PLACE)"<<std::endl;

	/*
		Windows
	*/
	#if defined(_WIN32)
		#include <windows.h>

	const DWORD buflen = 1000;
	char buf[buflen];
	DWORD len;

	// Find path of executable and set path_data relative to it
	len = GetModuleFileName(GetModuleHandle(NULL), buf, buflen);
	assert(len < buflen);
	pathRemoveFile(buf, '\\');

	// Use "./bin/../data"
	path_data = std::string(buf) + DIR_DELIM ".." DIR_DELIM "data";

	// Use "./bin/.."
	path_userdata = std::string(buf) + DIR_DELIM "..";

	/*
		Linux
	*/
	#elif defined(linux)
		#include <unistd.h>

	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	// Get path to executable
	assert(readlink("/proc/self/exe", buf, BUFSIZ-1) != -1);

	pathRemoveFile(buf, '/');

	// Use "./bin/../data"
	path_data = std::string(buf) + "/../data";

	// Use "./bin/../"
	path_userdata = std::string(buf) + "/..";

	/*
		OS X
	*/
	#elif defined(__APPLE__) || defined(__FreeBSD__)

	//TODO: Get path of executable. This assumes working directory is bin/
	dstream<<"WARNING: Relative path not properly supported on OS X and FreeBSD"
			<<std::endl;
	path_data = std::string("../data");
	path_userdata = std::string("..");

	#endif

#else // RUN_IN_PLACE

	/*
		Use platform-specific paths otherwise
	*/

	dstream<<"Using system-wide paths (NOT RUN_IN_PLACE)"<<std::endl;

	/*
		Windows
	*/
	#if defined(_WIN32)
		#include <windows.h>

	const DWORD buflen = 1000;
	char buf[buflen];
	DWORD len;

	// Find path of executable and set path_data relative to it
	len = GetModuleFileName(GetModuleHandle(NULL), buf, buflen);
	assert(len < buflen);
	pathRemoveFile(buf, '\\');

	// Use "./bin/../data"
	path_data = std::string(buf) + DIR_DELIM ".." DIR_DELIM "data";
	//path_data = std::string(buf) + "/../share/" + PROJECT_NAME;

	// Use "C:\Documents and Settings\user\Application Data\<PROJECT_NAME>"
	len = GetEnvironmentVariable("APPDATA", buf, buflen);
	assert(len < buflen);
	path_userdata = std::string(buf) + DIR_DELIM + PROJECT_NAME;

	/*
		Linux
	*/
	#elif defined(linux)
		#include <unistd.h>

	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	// Get path to executable
	assert(readlink("/proc/self/exe", buf, BUFSIZ-1) != -1);

	pathRemoveFile(buf, '/');

	path_data = std::string(buf) + "/../share/" + PROJECT_NAME;
	//path_data = std::string(INSTALL_PREFIX) + "/share/" + PROJECT_NAME;
	if (!fs::PathExists(path_data)) {
		dstream<<"WARNING: data path " << path_data << " not found!";
		path_data = std::string(buf) + "/../data";
		dstream<<" Trying " << path_data << std::endl;
	}

	path_userdata = std::string(getenv("HOME")) + "/." + PROJECT_NAME;

	/*
		OS X
	*/
	#elif defined(__APPLE__)
		#include <unistd.h>

	// Code based on
	// http://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c
	CFBundleRef main_bundle = CFBundleGetMainBundle();
	CFURLRef resources_url = CFBundleCopyResourcesDirectoryURL(main_bundle);
	char path[PATH_MAX];
	if(CFURLGetFileSystemRepresentation(resources_url, TRUE, (UInt8 *)path, PATH_MAX))
	{
		dstream<<"Bundle resource path: "<<path<<std::endl;
		//chdir(path);
		path_data = std::string(path) + "/share/" + PROJECT_NAME;
	}
	else
	{
		// error!
		dstream<<"WARNING: Could not determine bundle resource path"<<std::endl;
	}
	CFRelease(resources_url);

	path_userdata = std::string(getenv("HOME")) + "/Library/Application Support/" + PROJECT_NAME;

	#elif defined(__FreeBSD__)

	path_data = std::string(INSTALL_PREFIX) + "/share/" + PROJECT_NAME;
	path_userdata = std::string(getenv("HOME")) + "/." + PROJECT_NAME;

	#endif

#endif // RUN_IN_PLACE

	dstream<<"path_data = "<<path_data<<std::endl;
	dstream<<"path_userdata = "<<path_userdata<<std::endl;
}

std::string getUser()
{
#ifdef _WIN32

	char buff[1024];
	int size = 1024;

	if (GetUserName(buff,&size))
		return std::string(buff);
#else
	char* u = getenv("USER");
	if (u)
		return std::string(u);
#endif
	return std::string("someone");
}

} //namespace porting

