/************************************************************************
* Minetest-c55
* Copyright (C) 2010-2011 celeron55, Perttu Ahola <celeron55@gmail.com>
*
* player.h
* voxelands - 3d voxel world sandbox game
* Copyright (C) Lisa 'darkrose' Milne 2013-2014 <lisa@ltmnet.com>
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

#ifndef PLAYER_HEADER
#define PLAYER_HEADER

#include "common_irrlicht.h"
#ifndef SERVER
#include <IAnimatedMesh.h>
#endif
#include "inventory.h"
#include "collision.h"

#define PLAYERNAME_SIZE 20

#define PLAYERNAME_ALLOWED_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_"

#define PLAYERANIM_STAND 0
#define PLAYERANIM_WALK 1
#define PLAYERANIM_DIG 2
#define PLAYERANIM_WALKDIG 3
#define PLAYERANIM_DIE 4

class Map;

class Player
{
public:


	Player();
	virtual ~Player();

	void resetInventory();
	void checkInventory();

	//void move(f32 dtime, Map &map);
	virtual void move(f32 dtime, Map &map, f32 pos_max_d) = 0;

	v3f getSpeed()
	{
		return m_speed;
	}

	void setSpeed(v3f speed)
	{
		m_speed = speed;
	}

	// Y direction is ignored
	void accelerate(v3f target_speed, f32 max_increase);

	v3f getPosition()
	{
		return m_position;
	}

	v3s16 getLightPosition() const
	{
		return floatToInt(m_position + v3f(0,BS+BS/2,0), BS);
	}

	v3f getEyeOffset()
	{
		// This is at the height of the eyes of the current figure
		// return v3f(0, BS+BS/2, 0);
		// This is more like in minecraft
		return v3f(0,BS+(5*BS)/8,0);
	}

	v3f getEyePosition()
	{
		return m_position + getEyeOffset();
	}

	virtual void setPosition(const v3f &position)
	{
		m_position = position;
	}

	void setPitch(f32 pitch)
	{
		m_pitch = pitch;
	}

	virtual void setYaw(f32 yaw)
	{
		m_yaw = yaw;
	}

	f32 getPitch()
	{
		return m_pitch;
	}

	f32 getYaw()
	{
		return m_yaw;
	}

	virtual void updateName(const char *name)
	{
		snprintf(m_name, PLAYERNAME_SIZE, "%s", name);
	}

	virtual void wieldItem(u16 item);
	virtual const InventoryItem *getWieldItem() const
	{
		const InventoryList *list = inventory.getList("main");
		if (list)
			return list->getItem(m_selected_item);
		return NULL;
	}
	virtual u16 getSelectedItem() {return m_selected_item;}

	const char * getName()
	{
		return m_name;
	}

	virtual bool isLocal() const = 0;

	virtual void updateLight(u8 light_at_pos)
	{
		light = light_at_pos;
	}
	virtual std::string getCookie() {return "";}

	virtual void updateAnim(u8 anim_id)
	{}

	// NOTE: Use peer_id == 0 for disconnected
	/*virtual bool isClientConnected() { return false; }
	virtual void setClientConnected(bool) {}*/

	/*
		serialize() writes a bunch of text that can contain
		any characters except a '\0', and such an ending that
		deSerialize stops reading exactly at the right point.
	*/
	void serialize(std::ostream &os);
	void deSerialize(std::istream &is);

	bool touching_ground;
	// This oscillates so that the player jumps a bit above the surface
	bool in_water;
	// This is more stable and defines the maximum speed of the player
	bool in_water_stable;
	bool is_climbing;
	bool swimming_up;

	u8 light;

	Inventory inventory;
	// Actual inventory is backed up here when creative mode is used
	Inventory *inventory_backup;

	bool craftresult_is_preview;

	u16 hp;
	u16 air;
	u16 hunger;

	u16 peer_id;

	bool getHome(v3f &h);
	void setHome(v3f h);
	void unsetHome() {m_hashome = false;}

	f32 getArmourProtection()
	{
		f32 v = 0;
		const char* list[4] = {"hat","shirt","pants","boots"};
		for (int j=0; j<4; j++) {
			InventoryList *l = inventory.getList(list[j]);
			if (l == NULL)
				continue;
			InventoryItem *i = l->getItem(0);
			if (i == NULL)
				continue;
			v += content_clothesitem_features(i->getContent()).armour;
		}
		return v;
	}
	f32 getWarmthProtection()
	{
		f32 v = 0;
		const char* list[4] = {"hat","shirt","pants","boots"};
		for (int j=0; j<4; j++) {
			InventoryList *l = inventory.getList(list[j]);
			if (l == NULL)
				continue;
			InventoryItem *i = l->getItem(0);
			if (i == NULL)
				continue;
			v += content_clothesitem_features(i->getContent()).warmth;
		}
		return v;
	}
	f32 getVacuumProtection()
	{
		f32 v = 0;
		const char* list[4] = {"hat","shirt","pants","boots"};
		for (int j=0; j<4; j++) {
			InventoryList *l = inventory.getList(list[j]);
			if (l == NULL)
				continue;
			InventoryItem *i = l->getItem(0);
			if (i == NULL)
				continue;
			v += content_clothesitem_features(i->getContent()).vacuum;
		}
		return v;
	}
	f32 getSuffocationProtection()
	{
		InventoryList *l = inventory.getList("hat");
		if (l == NULL)
			return 0;
		InventoryItem *i = l->getItem(0);
		if (i == NULL)
			return 0;
		return content_clothesitem_features(i->getContent()).suffocate;
	}

	// character def used for skin creation and model scaling
	void setCharDef(std::string d) {m_character = d;}
	std::string getCharDef() {return m_character;}

	v3f getScale();

	// the unclothed player skin
	std::string getSkin();

protected:
	char m_name[PLAYERNAME_SIZE];
	u16 m_selected_item;
	f32 m_pitch;
	f32 m_yaw;
	v3f m_speed;
	v3f m_position;
	v3f m_home;
	bool m_hashome;
	std::string m_character;

public:

};

/*
	Player on the server
*/

class ServerRemotePlayer : public Player
{
public:
	ServerRemotePlayer():
		m_addr(""),
		m_cookie("")
	{
	}
	virtual ~ServerRemotePlayer()
	{
	}

	virtual bool isLocal() const
	{
		return false;
	}

	virtual void move(f32 dtime, Map &map, f32 pos_max_d)
	{
	}

	void setAddress(std::string addr) {m_addr = addr;}
	std::string getAddress() {return m_addr;}

private:
	virtual std::string getCookie()
	{
		if (m_cookie == "") {
			char buff[256];
			snprintf(buff,256,"%s%.4X",getName(),peer_id);
			m_cookie = std::string(buff);
		}
		return m_cookie;
	}

	std::string m_addr;
	std::string m_cookie;
};

#ifndef SERVER
#include "path.h"
#include "mesh.h"

/*
	All the other players on the client are these
*/

class RemotePlayer : public Player, public scene::ISceneNode
{
public:
	RemotePlayer(
		scene::ISceneNode* parent=NULL,
		IrrlichtDevice *device=NULL,
		s32 id=0);

	virtual ~RemotePlayer();

	/*
		ISceneNode methods
	*/

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render()
	{
		// Do nothing
	}

	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return m_box;
	}

	void setPosition(const v3f &position)
	{
		m_oldpos = m_showpos;

		if(m_pos_animation_time < 0.001 || m_pos_animation_time > 1.0)
			m_pos_animation_time = m_pos_animation_time_counter;
		else
			m_pos_animation_time = m_pos_animation_time * 0.9
					+ m_pos_animation_time_counter * 0.1;
		m_pos_animation_time_counter = 0;
		m_pos_animation_counter = 0;

		Player::setPosition(position);
	}

	virtual void setYaw(f32 yaw)
	{
		Player::setYaw(yaw);
		ISceneNode::setRotation(v3f(0, -yaw, 0));
	}

	bool isLocal() const
	{
		return false;
	}

	void updateName(const char *name);

	virtual void wieldItem(u16 item);

	virtual void updateAnim(u8 anim_id) {m_anim_id = anim_id;}

	virtual void updateLight(u8 light_at_pos)
	{
		Player::updateLight(light_at_pos);

		if(m_node == NULL)
			return;

		u8 li = decode_light(light_at_pos);
		video::SColor color(255,li,li,li);
		setMeshVerticesColor(m_node->getMesh(), color);
		m_wield->updateLight(li);
	}

	void updateCameraOffset(v3s16 camera_offset)
	{
		m_camera_offset = intToFloat(camera_offset,BS);
	}

	void move(f32 dtime, Map &map, f32 pos_max_d);

	video::ITexture* getTexture()
	{
		const char* list[4] = {"hat","shirt","pants","boots"};
		std::string tex = getSkin();
		for (int j=0; j<4; j++) {
			InventoryList *l = inventory.getList(list[j]);
			if (l == NULL)
				continue;
			InventoryItem *i = l->getItem(0);
			if (i == NULL)
				continue;
			tex += "^" + content_clothesitem_features(i->getContent()).overlay_texture;
		}
		return g_texturesource->getTextureRaw(tex);
	}

private:
	scene::IAnimatedMeshSceneNode *m_node;
	scene::ITextSceneNode* m_text;
	class ExtrudedSpriteSceneNode *m_wield;
	core::aabbox3d<f32> m_box;

	v3f m_oldpos;
	f32 m_pos_animation_counter;
	f32 m_pos_animation_time;
	f32 m_pos_animation_time_counter;
	v3f m_showpos;
	v3f m_camera_offset;
	u8 m_anim_id;
};

#endif // !SERVER

#ifndef SERVER
struct PlayerControl
{
	PlayerControl()
	{
		up = false;
		down = false;
		left = false;
		right = false;
		jump = false;
		aux1 = false;
		sneak = false;
		pitch = 0;
		yaw = 0;
	}
	PlayerControl(
		bool a_up,
		bool a_down,
		bool a_left,
		bool a_right,
		bool a_jump,
		bool a_aux1,
		bool a_sneak,
		float a_pitch,
		float a_yaw
	)
	{
		up = a_up;
		down = a_down;
		left = a_left;
		right = a_right;
		jump = a_jump;
		aux1 = a_aux1;
		sneak = a_sneak;
		pitch = a_pitch;
		yaw = a_yaw;
	}
	bool up;
	bool down;
	bool left;
	bool right;
	bool jump;
	bool aux1;
	bool sneak;
	float pitch;
	float yaw;
};

class LocalPlayer : public Player
{
public:
	LocalPlayer();
	virtual ~LocalPlayer();

	bool isLocal() const
	{
		return true;
	}

	void move(f32 dtime, Map &map, f32 pos_max_d,
			core::list<CollisionInfo> *collision_info);
	void move(f32 dtime, Map &map, f32 pos_max_d);

	void applyControl(float dtime);

	video::ITexture* getTexture()
	{
		const char* list[4] = {"hat","shirt","pants","boots"};
		std::string tex = getSkin();
		for (int j=0; j<4; j++) {
			InventoryList *l = inventory.getList(list[j]);
			if (l == NULL)
				continue;
			InventoryItem *i = l->getItem(0);
			if (i == NULL)
				continue;
			tex += "^" + content_clothesitem_features(i->getContent()).overlay_texture;
		}
		return g_texturesource->getTextureRaw(tex);
	}

	PlayerControl control;

private:
	// This is used for determining the sneaking range
	v3s16 m_sneak_node;
	bool m_refresh_sneak_node;
	// Whether the player is allowed to sneak
	bool m_sneak_node_exists;
	v3s16 m_old_node_below;
	content_t m_old_node_below_type;

};
#endif // !SERVER

#endif

