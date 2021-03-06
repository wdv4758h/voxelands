/************************************************************************
* Minetest-c55
* Copyright (C) 2011 celeron55, Perttu Ahola <celeron55@gmail.com>
*
* guiDeathScreen.h
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

#ifndef GUIMESSAGEMENU_HEADER
#define GUIMESSAGEMENU_HEADER

#include "common_irrlicht.h"
#include "modalMenu.h"
#include "utility.h"
#include <string>

class IRespawnInitiator
{
public:
	virtual ~IRespawnInitiator() {}
	virtual void respawn() = 0;
};

class GUIDeathScreen : public GUIModalMenu
{
public:
	GUIDeathScreen(gui::IGUIEnvironment* env,
			gui::IGUIElement* parent, s32 id,
			IMenuManager *menumgr, IRespawnInitiator *respawner);
	~GUIDeathScreen();

	void removeChildren();
	/*
		Remove and re-add (or reposition) stuff
	*/
	void regenerateGui(v2u32 screensize);

	void drawMenu();

	bool OnEvent(const SEvent& event);

	void respawn();

private:
	IRespawnInitiator *m_respawner;
	v2u32 m_screensize;
};

#endif

