/************************************************************************
* Minetest-c55
* Copyright (C) 2010-2011 celeron55, Perttu Ahola <celeron55@gmail.com>
*
* content_nodemeta.cpp
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

#include "content_nodemeta.h"
#include "inventory.h"
#include "content_mapnode.h"
#include "content_craft.h"
#include "content_list.h"
#include "log.h"
#include "player.h"
#include "environment.h"
#include "settings.h"
#include "main.h"
#include "mapblock.h"

/*
	SignNodeMetadata
*/

// Prototype
SignNodeMetadata proto_SignNodeMetadata("");

SignNodeMetadata::SignNodeMetadata(std::string text):
	m_text(text)
{
	NodeMetadata::registerType(typeId(), create);
}
u16 SignNodeMetadata::typeId() const
{
	return CONTENT_SIGN_WALL;
}
NodeMetadata* SignNodeMetadata::create(std::istream &is)
{
	std::string text = deSerializeString(is);
	return new SignNodeMetadata(text);
}
NodeMetadata* SignNodeMetadata::clone()
{
	return new SignNodeMetadata(m_text);
}
void SignNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_text);
}
std::string SignNodeMetadata::getDrawSpecString()
{
	std::string spec("size[5,2.5]");
	spec += "field[0.75,0;4,1.5;text;;";
	spec += m_text;
	spec += "]";
	spec += "button_exit[1.25,2;3,1;save;";
	spec += gettext("Save");
	spec += "]";
	return spec;
}
bool SignNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_LOCKABLE_SIGN_WALL)
		return false;
	LockingSignNodeMetadata *l = (LockingSignNodeMetadata*)meta;
	m_text = l->getText();
	return true;
}

/*
	LockingSignNodeMetadata
*/

// Prototype
LockingSignNodeMetadata proto_LockingSignNodeMetadata("");

LockingSignNodeMetadata::LockingSignNodeMetadata(std::string text):
	m_text(text)
{
	NodeMetadata::registerType(typeId(), create);
}
u16 LockingSignNodeMetadata::typeId() const
{
	return CONTENT_LOCKABLE_SIGN_WALL;
}
NodeMetadata* LockingSignNodeMetadata::create(std::istream &is)
{
	std::string text = deSerializeString(is);
	LockingSignNodeMetadata *d = new LockingSignNodeMetadata(text);
	d->setOwner(deSerializeString(is));
	return d;
}
NodeMetadata* LockingSignNodeMetadata::clone()
{
	return new LockingSignNodeMetadata(m_text);
}
void LockingSignNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_text);
	os<<serializeString(m_owner);
}
std::wstring LockingSignNodeMetadata::infoText()
{
	return narrow_to_wide(std::string("(")+m_owner+")");
}
bool LockingSignNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	if (getOwner() != player->getName())
		return false;
	m_text = fields["text"];
	return true;
}
std::string LockingSignNodeMetadata::getDrawSpecString()
{
	std::string spec("size[5,2.5]");
	spec += "field[0.75,0;4,1.5;text;;";
	spec += m_text;
	spec += "]";
	spec += "button_exit[1.25,2;3,1;save;";
	spec += gettext("Save");
	spec += "]";
	return spec;
}
bool LockingSignNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_SIGN_WALL)
		return false;
	SignNodeMetadata *l = (SignNodeMetadata*)meta;
	m_text = l->getText();
	return true;
}

/*
	FlagNodeMetadata
*/

// Prototype
FlagNodeMetadata proto_FlagNodeMetadata();

FlagNodeMetadata::FlagNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);
}
u16 FlagNodeMetadata::typeId() const
{
	return CONTENT_FLAG;
}
NodeMetadata* FlagNodeMetadata::create(std::istream &is)
{
	FlagNodeMetadata *d = new FlagNodeMetadata();
	d->setOwner(deSerializeString(is));
	return d;
}
NodeMetadata* FlagNodeMetadata::clone()
{
	FlagNodeMetadata *d = new FlagNodeMetadata();
	d->m_owner = m_owner;
	return d;
}
void FlagNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_owner);
}
std::wstring FlagNodeMetadata::infoText()
{
	char buff[256];
	snprintf(buff,256,gettext("%s's Home Flag"),m_owner.c_str());
	return narrow_to_wide(buff);
}

/*
	ChestNodeMetadata
*/

// Prototype
ChestNodeMetadata proto_ChestNodeMetadata;

ChestNodeMetadata::ChestNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("0", 8*4);
}
ChestNodeMetadata::~ChestNodeMetadata()
{
	delete m_inventory;
}
u16 ChestNodeMetadata::typeId() const
{
	return CONTENT_CHEST;
}
NodeMetadata* ChestNodeMetadata::create(std::istream &is)
{
	ChestNodeMetadata *d = new ChestNodeMetadata();
	d->m_inventory->deSerialize(is);
	return d;
}
NodeMetadata* ChestNodeMetadata::clone()
{
	ChestNodeMetadata *d = new ChestNodeMetadata();
	*d->m_inventory = *m_inventory;
	return d;
}
void ChestNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
}
std::wstring ChestNodeMetadata::infoText()
{
	return wgettext("Chest");
}
bool ChestNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if chest contains something
	*/
	InventoryList *list = m_inventory->getList("0");
	if(list == NULL)
		return false;
	if(list->getUsedSlots() == 0)
		return false;
	return true;
}
std::string ChestNodeMetadata::getDrawSpecString()
{
	return
		"size[8,9]"
		"list[current_name;0;0,0;8,4;]"
		"list[current_player;main;0,5;8,4;]";
}
bool ChestNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_LOCKABLE_CHEST)
		return false;
	LockingChestNodeMetadata *l = (LockingChestNodeMetadata*)meta;
	*m_inventory = *l->getInventory();
	return true;
}

/*
	LockingChestNodeMetadata
*/

// Prototype
LockingChestNodeMetadata proto_LockingChestNodeMetadata;

LockingChestNodeMetadata::LockingChestNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("0", 8*4);
}
LockingChestNodeMetadata::~LockingChestNodeMetadata()
{
	delete m_inventory;
}
u16 LockingChestNodeMetadata::typeId() const
{
	return CONTENT_LOCKABLE_CHEST;
}
NodeMetadata* LockingChestNodeMetadata::create(std::istream &is)
{
	LockingChestNodeMetadata *d = new LockingChestNodeMetadata();
	d->setOwner(deSerializeString(is));
	d->m_inventory->deSerialize(is);
	return d;
}
NodeMetadata* LockingChestNodeMetadata::clone()
{
	LockingChestNodeMetadata *d = new LockingChestNodeMetadata();
	*d->m_inventory = *m_inventory;
	return d;
}
void LockingChestNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_owner);
	m_inventory->serialize(os);
}
std::wstring LockingChestNodeMetadata::infoText()
{
	char buff[256];
	snprintf(buff,256,gettext("Locking Chest owned by '%s'"),m_owner.c_str());
	return narrow_to_wide(buff);
}
bool LockingChestNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if chest contains something
	*/
	InventoryList *list = m_inventory->getList("0");
	if(list == NULL)
		return false;
	if(list->getUsedSlots() == 0)
		return false;
	return true;
}
std::string LockingChestNodeMetadata::getDrawSpecString()
{
	return
		"size[8,9]"
		"list[current_name;0;0,0;8,4;]"
		"list[current_player;main;0,5;8,4;]";
}
bool LockingChestNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_CHEST)
		return false;
	ChestNodeMetadata *l = (ChestNodeMetadata*)meta;
	*m_inventory = *l->getInventory();
	return true;
}

/*
	SafeNodeMetadata
*/

// Prototype
SafeNodeMetadata proto_SafeNodeMetadata;

SafeNodeMetadata::SafeNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("0", 8*4);
}
SafeNodeMetadata::~SafeNodeMetadata()
{
	delete m_inventory;
}
u16 SafeNodeMetadata::typeId() const
{
	return CONTENT_SAFE;
}
NodeMetadata* SafeNodeMetadata::create(std::istream &is)
{
	SafeNodeMetadata *d = new SafeNodeMetadata();
	d->setOwner(deSerializeString(is));
	d->m_inventory->deSerialize(is);
	return d;
}
NodeMetadata* SafeNodeMetadata::clone()
{
	SafeNodeMetadata *d = new SafeNodeMetadata();
	*d->m_inventory = *m_inventory;
	return d;
}
void SafeNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_owner);
	m_inventory->serialize(os);
}
std::wstring SafeNodeMetadata::infoText()
{
	char buff[256];
	snprintf(buff,256,gettext("Safe owned by '%s'"),m_owner.c_str());
	return narrow_to_wide(buff);
}
bool SafeNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if chest contains something
	*/
	InventoryList *list = m_inventory->getList("0");
	if(list == NULL)
		return false;
	if(list->getUsedSlots() == 0)
		return false;
	return true;
}
std::string SafeNodeMetadata::getDrawSpecString()
{
	return
		"size[8,9]"
		"list[current_name;0;0,0;8,4;]"
		"list[current_player;main;0,5;8,4;]";
}
bool SafeNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_CHEST)
		return false;
	ChestNodeMetadata *l = (ChestNodeMetadata*)meta;
	*m_inventory = *l->getInventory();
	return true;
}

/*
	ParcelNodeMetadata
*/

// Prototype
ParcelNodeMetadata proto_ParcelNodeMetadata;

ParcelNodeMetadata::ParcelNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("0", 8*4);
}
ParcelNodeMetadata::~ParcelNodeMetadata()
{
	delete m_inventory;
}
u16 ParcelNodeMetadata::typeId() const
{
	return CONTENT_PARCEL;
}
NodeMetadata* ParcelNodeMetadata::create(std::istream &is)
{
	ParcelNodeMetadata *d = new ParcelNodeMetadata();
	d->m_inventory->deSerialize(is);
	return d;
}
NodeMetadata* ParcelNodeMetadata::clone()
{
	ParcelNodeMetadata *d = new ParcelNodeMetadata();
	*d->m_inventory = *m_inventory;
	return d;
}
void ParcelNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
}
std::string ParcelNodeMetadata::getDrawSpecString()
{
	return
		"size[8,4]"
		"list[current_name;0;0,0;8,4;]";
}

/*
	CreativeChestNodeMetadata
*/

// Prototype
CreativeChestNodeMetadata proto_CreativeChestNodeMetadata;

CreativeChestNodeMetadata::CreativeChestNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_page = 0;

	m_inventory = new Inventory();
	m_inventory->addList("0", 8*4);
}
CreativeChestNodeMetadata::~CreativeChestNodeMetadata()
{
	delete m_inventory;
}
u16 CreativeChestNodeMetadata::typeId() const
{
	return CONTENT_CREATIVE_CHEST;
}
NodeMetadata* CreativeChestNodeMetadata::create(std::istream &is)
{
	CreativeChestNodeMetadata *d = new CreativeChestNodeMetadata();
	d->m_inventory->deSerialize(is);
	is>>d->m_page;
	return d;
}
NodeMetadata* CreativeChestNodeMetadata::clone()
{
	CreativeChestNodeMetadata *d = new CreativeChestNodeMetadata();
	*d->m_inventory = *m_inventory;
	InventoryList *l = d->m_inventory->getList("0");
	InventoryItem *t;
	l->clearItems();
	std::vector<content_t> &list = lists::get("creative");
	for (u16 i=0; i<list.size() && i < 32; i++) {
		t = InventoryItem::create(list[i],1);
		l->addItem(t);
	}
	return d;
}
void CreativeChestNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
	os<<itos(m_page) << " ";
}
std::wstring CreativeChestNodeMetadata::infoText()
{
	return wgettext("Creative Chest");
}
bool CreativeChestNodeMetadata::nodeRemovalDisabled()
{
	return false;
}
bool CreativeChestNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	if (fields["prev"] == "" && fields["next"] == "")
		return false;
	std::vector<content_t> &list = lists::get("creative");
	if (fields["prev"] != "") {
		if (m_page > 0) {
			m_page--;
		}else{
			m_page = list.size()/32;
		}
	}
	if (fields["next"] != "")
		m_page++;
	if (m_page < 0)
		m_page = 0;
	if (m_page > (list.size()/32))
		m_page = 0;
	InventoryList *l = m_inventory->getList("0");
	InventoryItem *t;
	l->clearItems();
	u16 start = m_page*32;
	u16 end = start+32;
	if (end > list.size())
		end = list.size();
	for (u16 i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		l->addItem(t);
	}
	return true;
}
std::string CreativeChestNodeMetadata::getDrawSpecString()
{
	char buff[256];
	std::vector<content_t> &list = lists::get("creative");
	snprintf(buff,256,gettext("Page %d of %d"),(int)(m_page+1),(int)((list.size()/32)+1));
	std::string spec("size[8,10]");
		spec += "list[current_name;0;0,0.5;8,4;]";
		spec += "button[0.25,5;2.5,0.75;prev;";
		spec += gettext("<< Previous Page");
		spec += "]";
		spec += "label[3.5,5;";
		spec += buff;
		spec += "]";
		spec += "button[6,5;2.5,0.75;next;";
		spec += gettext("Next Page >>");
		spec += "]";
		spec += "list[current_player;main;0,6;8,4;]";

	return spec;
}

/*
	BorderStoneNodeMetadata
*/

// Prototype
BorderStoneNodeMetadata proto_BorderStoneNodeMetadata;

BorderStoneNodeMetadata::BorderStoneNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);
}
BorderStoneNodeMetadata::~BorderStoneNodeMetadata()
{
}
u16 BorderStoneNodeMetadata::typeId() const
{
	return CONTENT_BORDERSTONE;
}
NodeMetadata* BorderStoneNodeMetadata::create(std::istream &is)
{
	BorderStoneNodeMetadata *d = new BorderStoneNodeMetadata();
	d->setOwner(deSerializeString(is));
	return d;
}
NodeMetadata* BorderStoneNodeMetadata::clone()
{
	BorderStoneNodeMetadata *d = new BorderStoneNodeMetadata();
	return d;
}
void BorderStoneNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_text);
}
std::wstring BorderStoneNodeMetadata::infoText()
{
	char buff[256];
	snprintf(buff,256,gettext("Border Stone owned by '%s'"),m_text.c_str());
	return narrow_to_wide(buff);
}

/*
	FurnaceNodeMetadata
*/

// Prototype
FurnaceNodeMetadata proto_FurnaceNodeMetadata;

FurnaceNodeMetadata::FurnaceNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("fuel", 1);
	m_inventory->addList("src", 1);
	m_inventory->addList("dst", 4);

	m_step_accumulator = 0;
	m_fuel_totaltime = 0;
	m_fuel_time = 0;
	m_src_totaltime = 0;
	m_src_time = 0;
}
FurnaceNodeMetadata::~FurnaceNodeMetadata()
{
	delete m_inventory;
}
u16 FurnaceNodeMetadata::typeId() const
{
	return CONTENT_FURNACE;
}
NodeMetadata* FurnaceNodeMetadata::clone()
{
	FurnaceNodeMetadata *d = new FurnaceNodeMetadata();
	*d->m_inventory = *m_inventory;
	d->m_fuel_totaltime = m_fuel_totaltime;
	d->m_fuel_time = m_fuel_time;
	d->m_src_totaltime = m_src_totaltime;
	d->m_src_time = m_src_time;
	return d;
}
NodeMetadata* FurnaceNodeMetadata::create(std::istream &is)
{
	FurnaceNodeMetadata *d = new FurnaceNodeMetadata();

	d->m_inventory->deSerialize(is);

	int temp;
	is>>temp;
	d->m_fuel_totaltime = (float)temp/10;
	is>>temp;
	d->m_fuel_time = (float)temp/10;

	return d;
}
void FurnaceNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
	os<<itos(m_fuel_totaltime*10)<<" ";
	os<<itos(m_fuel_time*10)<<" ";
}
std::wstring FurnaceNodeMetadata::infoText()
{
	//return "Furnace";
	if (m_fuel_time >= m_fuel_totaltime) {
		const InventoryList *src_list = m_inventory->getList("src");
		assert(src_list);
		const InventoryItem *src_item = src_list->getItem(0);

		if (src_item && src_item->isCookable()) {
			InventoryList *dst_list = m_inventory->getList("dst");
			if(!dst_list->roomForCookedItem(src_item))
				return wgettext("Furnace is overloaded");
			return wgettext("Furnace is out of fuel");
		}else{
			return wgettext("Furnace is inactive");
		}
	}else{
		std::wstring s = wgettext("Furnace is active");
		// Do this so it doesn't always show (0%) for weak fuel
		if (m_fuel_totaltime > 3) {
			s += L" (";
			s += itows(m_fuel_time/m_fuel_totaltime*100);
			s += L"%)";
		}
		return s;
	}
}
bool FurnaceNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if furnace is not empty
	*/
	InventoryList *list[3] = {m_inventory->getList("src"),
	m_inventory->getList("dst"), m_inventory->getList("fuel")};

	for (int i = 0; i < 3; i++) {
		if (list[i] == NULL)
			continue;
		if (list[i]->getUsedSlots() == 0)
			continue;
		return true;
	}
	return false;

}
void FurnaceNodeMetadata::inventoryModified()
{
	infostream<<"Furnace inventory modification callback"<<std::endl;
}
bool FurnaceNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	if(dtime > 60.0)
		infostream<<"Furnace stepping a long time ("<<dtime<<")"<<std::endl;
	// Update at a fixed frequency
	const float interval = 2.0;
	m_step_accumulator += dtime;
	bool changed = false;
	MapNode n = env->getMap().getNodeNoEx(pos);
	while (m_step_accumulator > interval) {
		m_step_accumulator -= interval;
		dtime = interval;

		//infostream<<"Furnace step dtime="<<dtime<<std::endl;

		InventoryList *dst_list = m_inventory->getList("dst");
		assert(dst_list);

		InventoryList *src_list = m_inventory->getList("src");
		assert(src_list);
		InventoryItem *src_item = src_list->getItem(0);

		bool room_available = false;

		if (src_item && src_item->isCookable())
			room_available = dst_list->roomForCookedItem(src_item);

		// Start only if there are free slots in dst, so that it can
		// accomodate any result item
		if (room_available) {
			m_src_totaltime = 3;
		}else{
			m_src_time = 0;
			m_src_totaltime = 0;
		}

		/*
			If fuel is burning, increment the burn counters.
			If item finishes cooking, move it to result.
		*/
		if (m_fuel_time < m_fuel_totaltime) {
			if (n.getContent() == CONTENT_FURNACE) {
				n.param2 = n.param1;
				n.setContent(CONTENT_FURNACE_ACTIVE);
				env->setPostStepNodeSwap(pos,n);
			}
			//infostream<<"Furnace is active"<<std::endl;
			m_fuel_time += dtime;
			m_src_time += dtime;
			if (m_src_time >= m_src_totaltime && m_src_totaltime > 0.001 && src_item) {
				InventoryItem *cookresult = src_item->createCookResult();
				dst_list->addItem(cookresult);
				src_list->decrementMaterials(1);
				m_src_time = 0;
				m_src_totaltime = 0;
			}
			changed = true;

			// If the fuel was not used up this step, just keep burning it
			if (m_fuel_time < m_fuel_totaltime) {
				continue;
			}else if (n.getContent() == CONTENT_FURNACE_ACTIVE) {
				n.param1 = n.param2;
				n.setContent(CONTENT_FURNACE);
				env->setPostStepNodeSwap(pos,n);
			}
		}else if (n.getContent() == CONTENT_FURNACE_ACTIVE) {
			n.param1 = n.param2;
			n.setContent(CONTENT_FURNACE);
			env->setPostStepNodeSwap(pos,n);
		}

		/*
			Get the source again in case it has all burned
		*/
		src_item = src_list->getItem(0);

		/*
			If there is no source item, or the source item is not cookable,
			or the furnace is still cooking, or the furnace became overloaded, stop loop.
		*/
		if (
			src_item == NULL
			|| !room_available
			|| m_fuel_time < m_fuel_totaltime
			|| dst_list->roomForCookedItem(src_item) == false
		) {
			m_step_accumulator = 0;
			break;
		}

		//infostream<<"Furnace is out of fuel"<<std::endl;

		InventoryList *fuel_list = m_inventory->getList("fuel");
		assert(fuel_list);
		InventoryItem *fuel_item = fuel_list->getItem(0);
		if (fuel_item && fuel_item->isFuel()) {
			if ((fuel_item->getContent()&CONTENT_CRAFTITEM_MASK) == CONTENT_CRAFTITEM_MASK) {
				m_fuel_totaltime = ((CraftItem*)fuel_item)->getFuelTime();
			}else if ((fuel_item->getContent()&CONTENT_TOOLITEM_MASK) == CONTENT_TOOLITEM_MASK) {
				m_fuel_totaltime = ((ToolItem*)fuel_item)->getFuelTime();
			}else{
				m_fuel_totaltime = ((MaterialItem*)fuel_item)->getFuelTime();
			}
			m_fuel_time = 0;
			content_t c = fuel_item->getContent();
			fuel_list->decrementMaterials(1);
			if (c == CONTENT_TOOLITEM_STEELBUCKET_LAVA) {
				fuel_list->addItem(0,new ToolItem(CONTENT_TOOLITEM_STEELBUCKET,0));
			}
			changed = true;
		}else{
			m_step_accumulator = 0;
			MapNode n = env->getMap().getNodeNoEx(pos).getContent();
			if (n.getContent() == CONTENT_FURNACE_ACTIVE) {
				n.param1 = n.param2;
				n.setContent(CONTENT_FURNACE);
				env->setPostStepNodeSwap(pos,n);
			}
		}
	}
	return changed;
}
std::string FurnaceNodeMetadata::getDrawSpecString()
{
	return
		"size[8,9]"
		"list[current_name;fuel;2,3;1,1;]"
		"list[current_name;src;2,1;1,1;]"
		"list[current_name;dst;5,1;2,2;]"
		"list[current_player;main;0,5;8,4;]";
}
bool FurnaceNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_LOCKABLE_FURNACE)
		return false;
	LockingFurnaceNodeMetadata *l = (LockingFurnaceNodeMetadata*)meta;
	*m_inventory = *l->getInventory();
	return true;
}

/*
	LockingFurnaceNodeMetadata
*/

// Prototype
LockingFurnaceNodeMetadata proto_LockingFurnaceNodeMetadata;

LockingFurnaceNodeMetadata::LockingFurnaceNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("fuel", 1);
	m_inventory->addList("src", 1);
	m_inventory->addList("dst", 4);

	m_step_accumulator = 0;
	m_fuel_totaltime = 0;
	m_fuel_time = 0;
	m_src_totaltime = 0;
	m_src_time = 0;
	m_lock = 0;
}
LockingFurnaceNodeMetadata::~LockingFurnaceNodeMetadata()
{
	delete m_inventory;
}
u16 LockingFurnaceNodeMetadata::typeId() const
{
	return CONTENT_LOCKABLE_FURNACE;
}
NodeMetadata* LockingFurnaceNodeMetadata::clone()
{
	LockingFurnaceNodeMetadata *d = new LockingFurnaceNodeMetadata();
	*d->m_inventory = *m_inventory;
	d->m_fuel_totaltime = m_fuel_totaltime;
	d->m_fuel_time = m_fuel_time;
	d->m_src_totaltime = m_src_totaltime;
	d->m_src_time = m_src_time;
	return d;
}
NodeMetadata* LockingFurnaceNodeMetadata::create(std::istream &is)
{
	LockingFurnaceNodeMetadata *d = new LockingFurnaceNodeMetadata();

	d->m_inventory->deSerialize(is);
	d->setOwner(deSerializeString(is));
	d->setInventoryOwner(deSerializeString(is));

	int temp;
	is>>temp;
	d->m_fuel_totaltime = (float)temp/10;
	is>>temp;
	d->m_fuel_time = (float)temp/10;
	is>>temp;
	d->m_lock = (float)temp/10;

	return d;
}
void LockingFurnaceNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
	os<<serializeString(m_owner);
	os<<serializeString(m_inv_owner);
	os<<itos(m_fuel_totaltime*10)<<" ";
	os<<itos(m_fuel_time*10)<<" ";
	os<<itos(m_lock*10)<<" ";
}
std::wstring LockingFurnaceNodeMetadata::infoText()
{
	//return "Furnace";
	std::string ostr = m_owner;
	if (m_inv_owner != "")
		ostr += ","+m_inv_owner;
	if (m_fuel_time >= m_fuel_totaltime) {
		const InventoryList *src_list = m_inventory->getList("src");
		assert(src_list);
		const InventoryItem *src_item = src_list->getItem(0);

		if(src_item && src_item->isCookable()) {
			InventoryList *dst_list = m_inventory->getList("dst");
			if (!dst_list->roomForCookedItem(src_item))
				return wgettext("Locking Furnace is overloaded")+std::wstring(L" (")+narrow_to_wide(ostr)+L")";
			return wgettext("Locking Furnace is out of fuel")+std::wstring(L" (")+narrow_to_wide(ostr)+L")";
		}else{
			return wgettext("Locking Furnace is inactive")+std::wstring(L" (")+narrow_to_wide(ostr)+L")";
		}
	}else{
		std::wstring s = wgettext("Locking Furnace is active")+std::wstring(L" (")+narrow_to_wide(ostr)+L")";
		// Do this so it doesn't always show (0%) for weak fuel
		if (m_fuel_totaltime > 3) {
			s += L" (";
			s += itows(m_fuel_time/m_fuel_totaltime*100);
			s += L"%)";
		}
		return s;
	}
}
bool LockingFurnaceNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if furnace is not empty
	*/
	InventoryList *list[3] = {m_inventory->getList("src"),
	m_inventory->getList("dst"), m_inventory->getList("fuel")};

	for(int i = 0; i < 3; i++) {
		if(list[i] == NULL)
			continue;
		if(list[i]->getUsedSlots() == 0)
			continue;
		return true;
	}
	return false;

}
void LockingFurnaceNodeMetadata::inventoryModified()
{
	infostream<<"LockingFurnace inventory modification callback"<<std::endl;
}
bool LockingFurnaceNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	if (dtime > 60.0)
		infostream<<"LockingFurnace stepping a long time ("<<dtime<<")"<<std::endl;
	// Update at a fixed frequency
	const float interval = 2.0;
	m_step_accumulator += dtime;
	bool changed = false;
	MapNode n = env->getMap().getNodeNoEx(pos);
	while(m_step_accumulator > interval) {
		m_step_accumulator -= interval;
		dtime = interval;

		//infostream<<"Furnace step dtime="<<dtime<<std::endl;

		InventoryList *dst_list = m_inventory->getList("dst");
		assert(dst_list);

		InventoryList *src_list = m_inventory->getList("src");
		assert(src_list);
		InventoryItem *src_item = src_list->getItem(0);

		bool room_available = false;

		if (src_item && src_item->isCookable()) {
			room_available = dst_list->roomForCookedItem(src_item);
			m_lock = 300.0;
			changed = true;
		}else if (m_lock < 0.0) {
			setInventoryOwner("");
			changed = true;
		}else{
			m_lock -= dtime;
			changed = true;
		}

		// Start only if there are free slots in dst, so that it can
		// accomodate any result item
		if (room_available) {
			m_src_totaltime = 3;
		}else{
			m_src_time = 0;
			m_src_totaltime = 0;
		}

		/*
			If fuel is burning, increment the burn counters.
			If item finishes cooking, move it to result.
		*/
		if (m_fuel_time < m_fuel_totaltime) {
			if (n.getContent() == CONTENT_LOCKABLE_FURNACE) {
				n.param2 = n.param1;
				n.setContent(CONTENT_LOCKABLE_FURNACE_ACTIVE);
				env->setPostStepNodeSwap(pos,n);
				changed = true;
			}
			//infostream<<"Furnace is active"<<std::endl;
			m_fuel_time += dtime;
			m_src_time += dtime;
			if (m_src_time >= m_src_totaltime && m_src_totaltime > 0.001 && src_item) {
				InventoryItem *cookresult = src_item->createCookResult();
				dst_list->addItem(cookresult);
				src_list->decrementMaterials(1);
				m_src_time = 0;
				m_src_totaltime = 0;
			}
			changed = true;

			// If the fuel was not used up this step, just keep burning it
			if (m_fuel_time < m_fuel_totaltime) {
				continue;
			}else if (n.getContent() == CONTENT_LOCKABLE_FURNACE_ACTIVE) {
				n.param1 = n.param2;
				n.setContent(CONTENT_LOCKABLE_FURNACE);
				env->setPostStepNodeSwap(pos,n);
				changed = true;
			}
		}else if (n.getContent() == CONTENT_LOCKABLE_FURNACE_ACTIVE) {
			n.param1 = n.param2;
			n.setContent(CONTENT_LOCKABLE_FURNACE);
			env->setPostStepNodeSwap(pos,n);
			changed = true;
		}

		/*
			Get the source again in case it has all burned
		*/
		src_item = src_list->getItem(0);

		/*
			If there is no source item, or the source item is not cookable,
			or the furnace is still cooking, or the furnace became overloaded, stop loop.
		*/
		if(src_item == NULL || !room_available || m_fuel_time < m_fuel_totaltime ||
			dst_list->roomForCookedItem(src_item) == false)
		{
			m_step_accumulator = 0;
			break;
		}

		//infostream<<"Furnace is out of fuel"<<std::endl;

		InventoryList *fuel_list = m_inventory->getList("fuel");
		assert(fuel_list);
		InventoryItem *fuel_item = fuel_list->getItem(0);
		if (fuel_item && fuel_item->isFuel()) {
			if ((fuel_item->getContent()&CONTENT_CRAFTITEM_MASK) == CONTENT_CRAFTITEM_MASK) {
				m_fuel_totaltime = ((CraftItem*)fuel_item)->getFuelTime();
			}else if ((fuel_item->getContent()&CONTENT_TOOLITEM_MASK) == CONTENT_TOOLITEM_MASK) {
				m_fuel_totaltime = ((ToolItem*)fuel_item)->getFuelTime();
			}else{
				m_fuel_totaltime = ((MaterialItem*)fuel_item)->getFuelTime();
			}
			m_fuel_time = 0;
			content_t c = fuel_item->getContent();
			fuel_list->decrementMaterials(1);
			if (c == CONTENT_TOOLITEM_STEELBUCKET_LAVA) {
				fuel_list->addItem(0,new ToolItem(CONTENT_TOOLITEM_STEELBUCKET,0));
			}
			changed = true;
		}else{
			m_step_accumulator = 0;
			if (n.getContent() == CONTENT_LOCKABLE_FURNACE_ACTIVE) {
				n.param1 = n.param2;
				n.setContent(CONTENT_LOCKABLE_FURNACE);
				env->setPostStepNodeSwap(pos,n);
				changed = true;
			}
		}
	}
	return changed;
}
std::string LockingFurnaceNodeMetadata::getDrawSpecString()
{
	return
		"size[8,9]"
		"list[current_name;fuel;2,3;1,1;]"
		"list[current_name;src;2,1;1,1;]"
		"list[current_name;dst;5,1;2,2;]"
		"list[current_player;main;0,5;8,4;]";
}
bool LockingFurnaceNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_FURNACE)
		return false;
	FurnaceNodeMetadata *l = (FurnaceNodeMetadata*)meta;
	*m_inventory = *l->getInventory();
	return true;
}

/*
	LockedDoorNodeMetadata
*/

// Prototype
TNTNodeMetadata proto_TNTNodeMetadata;

TNTNodeMetadata::TNTNodeMetadata():
	m_armed(false),
	m_time(0)
{
	NodeMetadata::registerType(typeId(), create);
}
TNTNodeMetadata::~TNTNodeMetadata()
{
}
u16 TNTNodeMetadata::typeId() const
{
	return CONTENT_TNT;
}
NodeMetadata* TNTNodeMetadata::create(std::istream &is)
{
	TNTNodeMetadata *d = new TNTNodeMetadata();
	int temp;
	is>>temp;
	d->m_time = (float)temp/10;
	is>>temp;
	d->m_armed = (bool)temp;
	return d;
}
NodeMetadata* TNTNodeMetadata::clone()
{
	TNTNodeMetadata *d = new TNTNodeMetadata();
	return d;
}
bool TNTNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	if (!m_armed)
		return false;
	m_time -= dtime;
	if (m_time < 0.0)
		m_time = 0.0;
	return true;
}
bool TNTNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	m_armed = true;
	m_time = 5.0;
	return true;
}
u8 TNTNodeMetadata::getEnergy()
{
	if (!m_armed)
		return 0;
	return ENERGY_MAX-((u8)m_time);
}
void TNTNodeMetadata::serializeBody(std::ostream &os)
{
	os<<itos(m_time*10) << " ";
	os<<itos(m_armed) << " ";
}
std::wstring TNTNodeMetadata::infoText()
{
	if (!m_armed)
		return L"";

	int s = (int)ceil(m_time);
	if (s < 1)
		return wgettext("Armed Explosive: about to detonate");

	char buff[512];
	snprintf(buff,512,ngettext("Armed Explosive: %d second till detonation","Armed Explosive: %d seconds till detonation",s),s);
	return narrow_to_wide(buff);
}

/*
	IncineratorNodeMetadata
*/

// Prototype
IncineratorNodeMetadata proto_IncineratorNodeMetadata;

IncineratorNodeMetadata::IncineratorNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("fuel", 1);
}
IncineratorNodeMetadata::~IncineratorNodeMetadata()
{
	delete m_inventory;
}
u16 IncineratorNodeMetadata::typeId() const
{
	return CONTENT_INCINERATOR;
}
NodeMetadata* IncineratorNodeMetadata::clone()
{
	IncineratorNodeMetadata *d = new IncineratorNodeMetadata();
	*d->m_inventory = *m_inventory;
	return d;
}
NodeMetadata* IncineratorNodeMetadata::create(std::istream &is)
{
	IncineratorNodeMetadata *d = new IncineratorNodeMetadata();

	d->m_inventory->deSerialize(is);

	return d;
}
void IncineratorNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
}
std::wstring IncineratorNodeMetadata::infoText()
{
	InventoryList *list = m_inventory->getList("fuel");
	InventoryItem *fitem;

	if (list && list->getUsedSlots() > 0 && (fitem = list->getItem(0)) != NULL && fitem->isFuel())
		return wgettext("Incinerator is active");
	return wgettext("Incinerator is inactive");
}
bool IncineratorNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if not empty
	*/
	InventoryList *list = m_inventory->getList("fuel");

	if (list && list->getUsedSlots() > 0)
		return true;
	return false;

}
void IncineratorNodeMetadata::inventoryModified()
{
	infostream<<"Incinerator inventory modification callback"<<std::endl;
}
bool IncineratorNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	MapNode n = env->getMap().getNodeNoEx(pos);
	InventoryList *list = m_inventory->getList("fuel");
	InventoryItem *fitem;

	if (list && list->getUsedSlots() > 0 && (fitem = list->getItem(0)) != NULL && fitem->isFuel()) {
		if (n.getContent() == CONTENT_INCINERATOR) {
			n.param1 = n.param2;
			n.setContent(CONTENT_INCINERATOR_ACTIVE);
			env->setPostStepNodeSwap(pos,n);
			return true;
		}
	}else if (n.getContent() == CONTENT_INCINERATOR_ACTIVE) {
		n.param2 = n.param1;
		n.setContent(CONTENT_INCINERATOR);
		env->setPostStepNodeSwap(pos,n);
		return true;
	}
	return false;
}
std::string IncineratorNodeMetadata::getDrawSpecString()
{
	return
		std::string("size[8,7]"
		"label[1,0.5;")+gettext("Add fuel, then punch to incinerate wielded item")+"]"
		"label[3.5,1.5;Fuel]"
		"list[current_name;fuel;4,1;1,1;]"
		"list[current_player;main;0,3;8,4;]";
}

/*
	CraftGuideNodeMetadata
*/

// Prototype
CraftGuideNodeMetadata proto_CraftGuideNodeMetadata;

CraftGuideNodeMetadata::CraftGuideNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_page = 0;
	m_recipe = 0;

	m_inventory = new Inventory();
	m_inventory->addList("list", 300);
	m_inventory->addList("recipe", 9);
	m_inventory->addList("result", 1);
}
CraftGuideNodeMetadata::~CraftGuideNodeMetadata()
{
	delete m_inventory;
}
u16 CraftGuideNodeMetadata::typeId() const
{
	return CONTENT_CRAFT_BOOK_OPEN;
}
NodeMetadata* CraftGuideNodeMetadata::clone()
{
	CraftGuideNodeMetadata *d = new CraftGuideNodeMetadata();
	*d->m_inventory = *m_inventory;
	d->m_page = m_page;
	InventoryList *l = d->m_inventory->getList("list");
	InventoryItem *t;
	content_t *r;
	l->clearItems();
	std::vector<content_t> &list = lists::get("craftguide");
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		r = crafting::getRecipe(t);
		if (!r) {
			delete t;
			continue;
		}
		l->addItem(t);
	}
	return d;
}
NodeMetadata* CraftGuideNodeMetadata::create(std::istream &is)
{
	CraftGuideNodeMetadata *d = new CraftGuideNodeMetadata();

	d->m_inventory->deSerialize(is);
	is>>d->m_page;
	is>>d->m_recipe;

	return d;
}
void CraftGuideNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
	os<<itos(m_page) << " ";
	os<<itos(m_recipe) << " ";
}
bool CraftGuideNodeMetadata::nodeRemovalDisabled()
{
	return false;
}
void CraftGuideNodeMetadata::inventoryModified()
{
	infostream<<"CraftGuide inventory modification callback"<<std::endl;
}
bool CraftGuideNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	InventoryList *l = m_inventory->getList("result");
	InventoryItem *t = l->getItem(0);
	if (!t || t->getContent() == CONTENT_IGNORE)
		return false;
	content_t *r = crafting::getRecipe(t,m_recipe);
	if (!r) {
		if (m_recipe == 0)
			return false;

		m_recipe = 0;
		r = crafting::getRecipe(t,m_recipe);
		if (!r)
			return false;
	}
	l = m_inventory->getList("recipe");
	l->clearItems();
	for (int i=0; i<9; i++) {
		if (r[i] == CONTENT_IGNORE)
			continue;
		t = InventoryItem::create(r[i],1);
		l->addItem(i,t);
	}

	delete[] r;

	return true;
}
bool CraftGuideNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() == CONTENT_BOOK)
		m_page = ((ClosedBookNodeMetadata*)meta)->getPage();

	if (m_page < 0)
		m_page = 0;
	std::vector<content_t> &list = lists::get("craftguide");
	if (m_page > (list.size()/40))
		m_page = list.size()/40;
	InventoryList *l = m_inventory->getList("list");
	InventoryItem *t;
	content_t *r;
	l->clearItems();
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		r = crafting::getRecipe(t);
		if (!r) {
			delete t;
			continue;
		}
		l->addItem(t);
	}
	return true;
}
bool CraftGuideNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	InventoryList *l = m_inventory->getList("list");
	InventoryItem *t;
	if (fields["rprev"] != "" || fields["rnext"] != "") {
		l = m_inventory->getList("result");
		t = l->getItem(0);
		if (fields["rprev"] != "") {
			if (m_recipe > 0)
				m_recipe--;
		}else{
			m_recipe++;
		}
		int m = 1;
		if (t && t->getContent() != CONTENT_IGNORE)
			m = crafting::getRecipeCount(t);
		if (m_recipe >= m)
			m_recipe = m-1;
		step(0,v3s16(0,0,0),NULL);
		return true;
	}
	if (fields["prev"] == "" && fields["next"] == "")
		return false;
	std::vector<content_t> &list = lists::get("craftguide");
	if (fields["prev"] != "") {
		if (m_page > 0) {
			m_page--;
		}else{
			m_page = list.size()/40;
		}
	}
	if (fields["next"] != "")
		m_page++;
	if (m_page > (list.size()/40))
		m_page = 0;
	content_t *r;
	l->clearItems();
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		r = crafting::getRecipe(t);
		if (!r) {
			delete t;
			continue;
		}
		l->addItem(t);
	}
	return true;
}
std::string CraftGuideNodeMetadata::getDrawSpecString()
{
	InventoryList *l = m_inventory->getList("result");
	InventoryItem *q = l->getItem(0);
	int tr = 0;
	int rc = 0;
	std::vector<content_t> &list = lists::get("craftguide");
	if (q && q->getContent() != CONTENT_IGNORE) {
		tr = crafting::getResultCount(q);
		rc = crafting::getRecipeCount(q);
	}

	char buff[256];
	snprintf(buff,256,gettext("Page %d of %d"),(int)(m_page+1),(int)((list.size()/40)+1));

	std::string spec("size[8,10]");
	spec +=	"label[0.5,0.75;";
	spec += gettext("Add item here to see recipe");
	spec += "]";
	spec +=	"list[current_name;result;2,1;1,1;]";
	if (rc > 1) {
		char rbuff[256];
		snprintf(rbuff,256,gettext("Recipe %d of %d"),(int)(m_recipe+1),rc);
		spec += "button[2.5,3.5;1,0.75;rprev;<<]";
		spec += "label[3.5,3.5;";
		spec += rbuff;
		spec += "]";
		spec += "button[5.5,3.5;1,0.75;rnext;>>]";
	}
	if (q && tr) {
		spec += "label[1,1.5;";
		spec += gettext("Gives");
		spec += " ";
		spec += itos(tr);
		// this overflows into the craft grid... but could be cool
		//spec += " ";
		//spec += wide_to_narrow(q->getGuiName());
		spec += "]";
	}
	spec +=	"list[current_name;recipe;4,0;3,3;]";
	spec +=	"button[0.25,4.5;2.5,0.75;prev;";
	spec += gettext("<< Previous Page");
	spec += "]";
	spec +=	"label[3.5,4.5;";
	spec += buff;
	spec += "]";
	spec +=	"button[6,4.5;2.5,0.75;next;";
	spec += gettext("Next Page >>");
	spec += "]";
	spec +=	"list[current_name;list;0,5;8,5;]";
	return spec;
}

/*
	CookBookNodeMetadata
*/

// Prototype
CookBookNodeMetadata proto_CookBookNodeMetadata;

CookBookNodeMetadata::CookBookNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_page = 0;

	m_inventory = new Inventory();
	m_inventory->addList("list", 300);
	m_inventory->addList("recipe", 1);
	m_inventory->addList("result", 1);
}
CookBookNodeMetadata::~CookBookNodeMetadata()
{
	delete m_inventory;
}
u16 CookBookNodeMetadata::typeId() const
{
	return CONTENT_COOK_BOOK_OPEN;
}
NodeMetadata* CookBookNodeMetadata::clone()
{
	CookBookNodeMetadata *d = new CookBookNodeMetadata();
	*d->m_inventory = *m_inventory;
	d->m_page = m_page;
	InventoryList *l = d->m_inventory->getList("list");
	InventoryItem *t;
	l->clearItems();
	std::vector<content_t> &list = lists::get("cooking");
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		InventoryItem *cookresult = t->createCookResult();
		if (!cookresult || cookresult->getContent() == CONTENT_IGNORE) {
			delete t;
			delete cookresult;
			continue;
		}
		delete cookresult;
		l->addItem(t);
	}
	return d;
}
NodeMetadata* CookBookNodeMetadata::create(std::istream &is)
{
	CookBookNodeMetadata *d = new CookBookNodeMetadata();

	d->m_inventory->deSerialize(is);
	is>>d->m_page;

	return d;
}
void CookBookNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
	os<<itos(m_page) << " ";
}
bool CookBookNodeMetadata::nodeRemovalDisabled()
{
	return false;
}
void CookBookNodeMetadata::inventoryModified()
{
	infostream<<"CookBook inventory modification callback"<<std::endl;
}
bool CookBookNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	InventoryList *l = m_inventory->getList("result");
	InventoryItem *t = l->getItem(0);
	if (!t || t->getContent() == CONTENT_IGNORE)
		return false;
	InventoryItem *cookresult = t->createCookResult();
	if (!cookresult || cookresult->getContent() == CONTENT_IGNORE)
		return false;
	l = m_inventory->getList("recipe");
	l->clearItems();
	l->addItem(0,cookresult);

	return true;
}
bool CookBookNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() == CONTENT_BOOK)
		m_page = ((ClosedBookNodeMetadata*)meta)->getPage();

	if (m_page < 0)
		m_page = 0;
	std::vector<content_t> &list = lists::get("cooking");
	if (m_page > (list.size()/40))
		m_page = list.size()/40;
	InventoryList *l = m_inventory->getList("list");
	InventoryItem *t;
	l->clearItems();
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		InventoryItem *cookresult = t->createCookResult();
		if (!cookresult || cookresult->getContent() == CONTENT_IGNORE) {
			delete t;
			delete cookresult;
			continue;
		}
		delete cookresult;
		l->addItem(t);
	}
	return true;
}
bool CookBookNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	if (fields["prev"] == "" && fields["next"] == "")
		return false;
	std::vector<content_t> &list = lists::get("cooking");
	if (fields["prev"] != "") {
		if (m_page > 0) {
			m_page--;
		}else{
			m_page = list.size()/40;
		}
	}
	if (fields["next"] != "")
		m_page++;
	if (m_page > (list.size()/40))
		m_page = 0;
	InventoryList *l = m_inventory->getList("list");
	InventoryItem *t;
	l->clearItems();
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		t = InventoryItem::create(list[i],1);
		InventoryItem *cookresult = t->createCookResult();
		if (!cookresult || cookresult->getContent() == CONTENT_IGNORE) {
			delete t;
			delete cookresult;
			continue;
		}
		delete cookresult;
		l->addItem(t);
	}
	return true;
}
std::string CookBookNodeMetadata::getDrawSpecString()
{
	std::vector<content_t> &list = lists::get("cooking");

	char buff[256];
	snprintf(buff,256,gettext("Page %d of %d"),(int)(m_page+1),(int)((list.size()/40)+1));

	std::string spec("size[8,9]");
	spec +=	"label[0.5,0.75;";
	spec += gettext("Add item here to see cook result");
	spec += "]";
	spec +=	"list[current_name;result;2,1;1,1;]";
	spec +=	"list[current_name;recipe;4,1;1,1;]";
	spec +=	"button[0.25,3.5;2.5,0.75;prev;";
	spec += gettext("<< Previous Page");
	spec += "]";
	spec +=	"label[3.5,3.5;";
	spec += buff;
	spec += "]";
	spec +=	"button[6,3.5;2.5,0.75;next;";
	spec += gettext("Next Page >>");
	spec += "]";
	spec +=	"list[current_name;list;0,4;8,5;]";
	return spec;
}

/*
	DeCraftNodeMetadata
*/

// Prototype
DeCraftNodeMetadata proto_DeCraftNodeMetadata;

DeCraftNodeMetadata::DeCraftNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_page = 0;

	m_inventory = new Inventory();
	m_inventory->addList("list", 300);
	m_inventory->addList("recipe", 1);
	m_inventory->addList("random", 1);
	m_inventory->addList("result", 1);
}
DeCraftNodeMetadata::~DeCraftNodeMetadata()
{
	delete m_inventory;
}
u16 DeCraftNodeMetadata::typeId() const
{
	return CONTENT_DECRAFT_BOOK_OPEN;
}
NodeMetadata* DeCraftNodeMetadata::clone()
{
	DeCraftNodeMetadata *d = new DeCraftNodeMetadata();
	*d->m_inventory = *m_inventory;
	d->m_page = m_page;
	InventoryList *l = d->m_inventory->getList("list");
	InventoryItem *t;
	l->clearItems();
	std::vector<content_t> &list = lists::get("decrafting");
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		if ((list[i]&CONTENT_CRAFTITEM_MASK) == CONTENT_CRAFTITEM_MASK)
			continue;
		if ((list[i]&CONTENT_TOOLITEM_MASK) == CONTENT_TOOLITEM_MASK)
			continue;
		if ((list[i]&CONTENT_CLOTHESITEM_MASK) == CONTENT_CLOTHESITEM_MASK)
			continue;
		if (content_features(list[i]).dug_item == "" && content_features(list[i]).extra_dug_item == "")
			continue;

		t = new MaterialItem(list[i],1);
		l->addItem(t);
	}
	return d;
}
NodeMetadata* DeCraftNodeMetadata::create(std::istream &is)
{
	DeCraftNodeMetadata *d = new DeCraftNodeMetadata();

	d->m_inventory->deSerialize(is);
	is>>d->m_page;

	return d;
}
void DeCraftNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
	os<<itos(m_page) << " ";
}
bool DeCraftNodeMetadata::nodeRemovalDisabled()
{
	return false;
}
void DeCraftNodeMetadata::inventoryModified()
{
	infostream<<"DeCraft inventory modification callback"<<std::endl;
}
bool DeCraftNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	InventoryList *l = m_inventory->getList("result");
	InventoryItem *t = l->getItem(0);
	if (!t || t->getContent() == CONTENT_IGNORE)
		return false;
	if ((t->getContent()&CONTENT_CRAFTITEM_MASK) == CONTENT_CRAFTITEM_MASK)
		return false;
	if ((t->getContent()&CONTENT_TOOLITEM_MASK) == CONTENT_TOOLITEM_MASK)
		return false;
	if ((t->getContent()&CONTENT_CLOTHESITEM_MASK) == CONTENT_CLOTHESITEM_MASK)
		return false;
	if (content_features(t->getContent()).dug_item == "" && content_features(t->getContent()).extra_dug_item == "")
		return false;
	l = m_inventory->getList("recipe");
	l->clearItems();
	if (content_features(t->getContent()).dug_item != "") {
		std::istringstream is(content_features(t->getContent()).dug_item, std::ios::binary);
		InventoryItem *r = InventoryItem::deSerialize(is);
		if (!r)
			return false;
		if (r->getContent() == CONTENT_IGNORE) {
			delete r;
			return false;
		}
		l->addItem(0,r);
	}
	l = m_inventory->getList("random");
	l->clearItems();
	if (content_features(t->getContent()).extra_dug_item != "") {
		std::istringstream is(content_features(t->getContent()).extra_dug_item, std::ios::binary);
		InventoryItem *r = InventoryItem::deSerialize(is);
		if (!r)
			return true;
		if (r->getContent() == CONTENT_IGNORE) {
			delete r;
			return true;
		}
		l->addItem(0,r);
	}
	return true;
}
bool DeCraftNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() == CONTENT_BOOK)
		m_page = ((ClosedBookNodeMetadata*)meta)->getPage();

	if (m_page < 0)
		m_page = 0;
	std::vector<content_t> &list = lists::get("decrafting");
	if (m_page > (list.size()/40))
		m_page = list.size()/40;
	InventoryList *l = m_inventory->getList("list");
	InventoryItem *t;
	l->clearItems();
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		if ((list[i]&CONTENT_CRAFTITEM_MASK) == CONTENT_CRAFTITEM_MASK)
			continue;
		if ((list[i]&CONTENT_TOOLITEM_MASK) == CONTENT_TOOLITEM_MASK)
			continue;
		if ((list[i]&CONTENT_CLOTHESITEM_MASK) == CONTENT_CLOTHESITEM_MASK)
			continue;
		if (content_features(list[i]).dug_item == "" && content_features(list[i]).extra_dug_item == "")
			continue;

		t = new MaterialItem(list[i],1);
		l->addItem(t);
	}
	return true;
}
bool DeCraftNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	if (fields["prev"] == "" && fields["next"] == "")
		return false;
	std::vector<content_t> &list = lists::get("decrafting");
	if (fields["prev"] != "") {
		if (m_page > 0) {
			m_page--;
		}else{
			m_page = list.size()/40;
		}
	}
	if (fields["next"] != "")
		m_page++;
	if (m_page > (list.size()/40))
		m_page = 0;
	InventoryList *l = m_inventory->getList("list");
	InventoryItem *t;
	l->clearItems();
	u16 start = m_page*40;
	u16 end = start+40;
	if (end > list.size())
		end = list.size();
	for (int i=start; i<end; i++) {
		if ((list[i]&CONTENT_CRAFTITEM_MASK) == CONTENT_CRAFTITEM_MASK)
			continue;
		if ((list[i]&CONTENT_TOOLITEM_MASK) == CONTENT_TOOLITEM_MASK)
			continue;
		if ((list[i]&CONTENT_CLOTHESITEM_MASK) == CONTENT_CLOTHESITEM_MASK)
			continue;
		if (content_features(list[i]).dug_item == "" && content_features(list[i]).extra_dug_item == "")
			continue;

		t = new MaterialItem(list[i],1);
		l->addItem(t);
	}
	return true;
}
std::string DeCraftNodeMetadata::getDrawSpecString()
{
	std::vector<content_t> &list = lists::get("decrafting");
	char buff[256];
	snprintf(buff,256,gettext("Page %d of %d"),(int)(m_page+1),(int)((list.size()/40)+1));

	std::string spec("size[8,9]");
	spec +=	"label[0.5,0.75;";
	spec += gettext("Add item here to see dig result");
	spec += "]";
	spec +=	"list[current_name;result;2,1;1,1;]";
	spec +=	"label[5,1;";
	spec += gettext("Dig Result");
	spec += "]";
	spec +=	"list[current_name;recipe;6.5,0.5;1,1;]";
	spec +=	"label[5,2;";
	spec += gettext("Random Drop");
	spec += "]";
	spec +=	"list[current_name;random;6.5,1.5;1,1;]";
	spec +=	"button[0.25,3.5;2.5,0.75;prev;";
	spec += gettext("<< Previous Page");
	spec += "]";
	spec +=	"label[3.5,3.5;";
	spec += buff;
	spec += "]";
	spec +=	"button[6,3.5;2.5,0.75;next;";
	spec += gettext("Next Page >>");
	spec += "]";
	spec +=	"list[current_name;list;0,4;8,5;]";
	return spec;
}

/*
	BookNodeMetadata
*/

// Prototype
BookNodeMetadata proto_BookNodeMetadata;

BookNodeMetadata::BookNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_title = "Book";
	m_content = "";
}

u16 BookNodeMetadata::typeId() const
{
	return CONTENT_BOOK_OPEN;
}
NodeMetadata* BookNodeMetadata::clone()
{
	BookNodeMetadata *d = new BookNodeMetadata();
	d->m_title = m_title;
	d->m_content = m_content;
	return d;
}
NodeMetadata* BookNodeMetadata::create(std::istream &is)
{
	BookNodeMetadata *d = new BookNodeMetadata();
	d->m_title = deSerializeString(is);
	d->m_content = deSerializeString(is);
	return d;
}
void BookNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_title);
	os<<serializeString(m_content);
}
bool BookNodeMetadata::nodeRemovalDisabled()
{
	if (m_content != "")
		return true;
	return false;
}
bool BookNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_BOOK)
		return false;

	ClosedBookNodeMetadata *m = (ClosedBookNodeMetadata*)meta;
	m_title = wide_to_narrow(m->infoText());
	m_content = m->getContent();
	return true;
}
bool BookNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	m_title = fields["title"];
	m_content = fields["content"];

	std::string::size_type pos = 0;
	while ((pos = m_content.find("]",pos)) != std::string::npos) {
		m_content.replace(pos,1,")");
		pos++;
	}
	pos = 0;
	while ((pos = m_content.find("[",pos)) != std::string::npos) {
		m_content.replace(pos,1,"(");
		pos++;
	}
	return true;
}

std::string BookNodeMetadata::getDrawSpecString()
{
	std::string spec("size[6,6]");
	spec += "field[1,1;5,1;title;";
	spec += gettext("Title");
	spec += ";";
	spec += m_title;
	spec += "]";
	spec += "field[1,2;5,2;content;";
	spec += gettext("Content");
	spec += ";";
	spec += m_content;
	spec += "]";
	spec += "button_exit[2,5;3,1;submit;";
	spec += gettext("Save");
	spec += "]";
	return spec;
}

/*
	DiaryNodeMetadata
*/

// Prototype
DiaryNodeMetadata proto_DiaryNodeMetadata;

DiaryNodeMetadata::DiaryNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_title = gettext("Diary");
	m_content = "";
}

u16 DiaryNodeMetadata::typeId() const
{
	return CONTENT_DIARY_BOOK_OPEN;
}
NodeMetadata* DiaryNodeMetadata::clone()
{
	DiaryNodeMetadata *d = new DiaryNodeMetadata();
	d->m_owner = m_owner;
	d->m_title = m_title;
	d->m_content = m_content;
	return d;
}
NodeMetadata* DiaryNodeMetadata::create(std::istream &is)
{
	DiaryNodeMetadata *d = new DiaryNodeMetadata();
	d->m_owner = deSerializeString(is);
	d->m_title = deSerializeString(is);
	d->m_content = deSerializeString(is);
	return d;
}
void DiaryNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_owner);
	os<<serializeString(m_title);
	os<<serializeString(m_content);
}
bool DiaryNodeMetadata::nodeRemovalDisabled()
{
	if (m_content != "")
		return true;
	return false;
}
bool DiaryNodeMetadata::import(NodeMetadata *meta)
{
	if (meta->typeId() != CONTENT_BOOK)
		return false;

	ClosedBookNodeMetadata *m = (ClosedBookNodeMetadata*)meta;
	m_owner = m->getOwner();
	m_title = wide_to_narrow(m->infoText());
	m_content = m->getContent();
	return true;
}
bool DiaryNodeMetadata::receiveFields(std::string formname, std::map<std::string, std::string> fields, Player *player)
{
	if (m_owner == "")
		m_owner = player->getName();
	if (player->getName() != m_owner)
		return false;
	m_title = fields["title"];
	m_content = fields["content"];

	std::string::size_type pos = 0;
	while ((pos = m_content.find("]",pos)) != std::string::npos) {
		m_content.replace(pos,1,")");
		pos++;
	}
	pos = 0;
	while ((pos = m_content.find("[",pos)) != std::string::npos) {
		m_content.replace(pos,1,"(");
		pos++;
	}
	return true;
}

std::string DiaryNodeMetadata::getDrawSpecString()
{
	std::string spec("size[6,6]");
	spec += "field[1,1;5,1;title;";
	spec += gettext("Title");
	spec += ";";
	spec += m_title;
	spec += "]";
	spec += "field[1,2;5,2;content;";
	spec += gettext("Content");
	spec += ";";
	spec += m_content;
	spec += "]";
	spec += "button_exit[2,5;3,1;submit;";
	spec += gettext("Save");
	spec += "]";
	return spec;
}

/*
	ClosedBookNodeMetadata
*/

// Prototype
ClosedBookNodeMetadata proto_ClosedBookNodeMetadata;

ClosedBookNodeMetadata::ClosedBookNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_page = 0;
	m_owner = "";
	m_title = "";
	m_content = "";
}
u16 ClosedBookNodeMetadata::typeId() const
{
	return CONTENT_BOOK;
}
NodeMetadata* ClosedBookNodeMetadata::clone()
{
	ClosedBookNodeMetadata *d = new ClosedBookNodeMetadata();
	d->m_page = m_page;
	d->m_owner = m_owner;
	d->m_title = m_title;
	d->m_content = m_content;
	return d;
}
NodeMetadata* ClosedBookNodeMetadata::create(std::istream &is)
{
	ClosedBookNodeMetadata *d = new ClosedBookNodeMetadata();

	d->m_owner = deSerializeString(is);
	d->m_title = deSerializeString(is);
	d->m_content = deSerializeString(is);
	is>>d->m_page;

	return d;
}
void ClosedBookNodeMetadata::serializeBody(std::ostream &os)
{
	os<<serializeString(m_owner);
	os<<serializeString(m_title);
	os<<serializeString(m_content);
	os<<itos(m_page) << " ";
}
bool ClosedBookNodeMetadata::nodeRemovalDisabled()
{
	return false;
}
bool ClosedBookNodeMetadata::import(NodeMetadata *meta)
{
	switch (meta->typeId()) {
	case CONTENT_CRAFT_BOOK_OPEN:
	{
		CraftGuideNodeMetadata *cm = (CraftGuideNodeMetadata*)meta;
		m_page = cm->getPage();
		m_title = wide_to_narrow(cm->infoText());
		break;
	}
	case CONTENT_DECRAFT_BOOK_OPEN:
	{
		DeCraftNodeMetadata *cm = (DeCraftNodeMetadata*)meta;
		m_page = cm->getPage();
		m_title = wide_to_narrow(cm->infoText());
		break;
	}
	case CONTENT_BOOK_OPEN:
	{
		BookNodeMetadata *bm = (BookNodeMetadata*)meta;
		m_title = wide_to_narrow(bm->infoText());
		m_content = bm->getContent();
		break;
	}
	case CONTENT_DIARY_BOOK_OPEN:
	{
		DiaryNodeMetadata *dm = (DiaryNodeMetadata*)meta;
		m_owner = dm->getOwner();
		m_title = wide_to_narrow(dm->infoText());
		m_content = dm->getContent();
		break;
	}
	default:;
	}
	return false;
}

/*
	BookShelfNodeMetadata
*/

// Prototype
BookShelfNodeMetadata proto_BookShelfNodeMetadata;

BookShelfNodeMetadata::BookShelfNodeMetadata()
{
	NodeMetadata::registerType(typeId(), create);

	m_inventory = new Inventory();
	m_inventory->addList("0", 14);
	InventoryList *l = m_inventory->getList("0");
	l->setStackable(false);
	l->addAllowed(CONTENT_BOOK);
	l->addAllowed(CONTENT_COOK_BOOK);
	l->addAllowed(CONTENT_DECRAFT_BOOK);
	l->addAllowed(CONTENT_DIARY_BOOK);
	l->addAllowed(CONTENT_CRAFT_BOOK);
}
BookShelfNodeMetadata::~BookShelfNodeMetadata()
{
	delete m_inventory;
}
u16 BookShelfNodeMetadata::typeId() const
{
	return CONTENT_BOOKSHELF;
}
NodeMetadata* BookShelfNodeMetadata::create(std::istream &is)
{
	BookShelfNodeMetadata *d = new BookShelfNodeMetadata();
	d->m_inventory->deSerialize(is);
	return d;
}
NodeMetadata* BookShelfNodeMetadata::clone()
{
	BookShelfNodeMetadata *d = new BookShelfNodeMetadata();
	*d->m_inventory = *m_inventory;
	return d;
}
void BookShelfNodeMetadata::serializeBody(std::ostream &os)
{
	m_inventory->serialize(os);
}
bool BookShelfNodeMetadata::nodeRemovalDisabled()
{
	/*
		Disable removal if chest contains something
	*/
	InventoryList *list = m_inventory->getList("0");
	if(list == NULL)
		return false;
	if(list->getUsedSlots() == 0)
		return false;
	return true;
}
std::string BookShelfNodeMetadata::getDrawSpecString()
{
	return
		"size[8,7]"
		"list[current_name;0;0.5,0;7,2;]"
		"list[current_player;main;0,3;8,4;]";
}
std::vector<NodeBox> BookShelfNodeMetadata::getNodeBoxes(MapNode &n) {
	std::vector<NodeBox> boxes;
	boxes.clear();

	InventoryList *list = m_inventory->getList("0");
	if(list == NULL)
		return boxes;
	if(list->getUsedSlots() == 0)
		return boxes;

	f32 x = 0;
	f32 y = 0;
	f32 h = 0;

	for (s16 i=0; i<14; i++) {
		if (list->getItem(i) == NULL)
			continue;
		x = (i%7)*0.125;
		y = (i/7)*-0.5;
		h = ((i%7)%2)*0.0625;

		boxes.push_back(NodeBox(
			(-0.4375+x)*BS,(0.0625+y)*BS,-0.4375*BS,(-0.3125+x)*BS,(0.375+y+h)*BS,-0.0625*BS
		));
		boxes.push_back(NodeBox(
			(0.3125-x)*BS,(0.0625+y)*BS,0.0625*BS,(0.4375-x)*BS,(0.375+y+h)*BS,0.4375*BS
		));
	}

	return transformNodeBox(n,boxes);
}

/*
	ClockNodeMetadata
*/

// Prototype
ClockNodeMetadata proto_ClockNodeMetadata;

ClockNodeMetadata::ClockNodeMetadata():
	m_time(0)
{
	NodeMetadata::registerType(typeId(), create);
}
u16 ClockNodeMetadata::typeId() const
{
	return CONTENT_CLOCK;
}
NodeMetadata* ClockNodeMetadata::create(std::istream &is)
{
	ClockNodeMetadata *d = new ClockNodeMetadata();
	int temp;
	is>>temp;
	d->m_time = temp;
	return d;
}
NodeMetadata* ClockNodeMetadata::clone()
{
	ClockNodeMetadata *d = new ClockNodeMetadata();
	return d;
}
void ClockNodeMetadata::serializeBody(std::ostream &os)
{
	os<<itos(m_time) << " ";
}
std::vector<NodeBox> ClockNodeMetadata::getNodeBoxes(MapNode &n) {
	std::vector<NodeBox> boxes;
	boxes.clear();

	u16 h = m_time/100;
	u16 m = (u16)((float)(m_time%100)/1.6667);

	u16 v[4];
	v[0] = h/10;
	v[1] = h%10;
	v[2] = m/10;
	v[3] = m%10;

	f32 x[4] = {-0.125,0.0625,0.3125,0.5};

	u8 b[10] = {0xFC,0x0C,0xB6,0x9E,0x4E,0xDA,0xFA,0x8C,0xFE,0xDE};

	for (int i=0; i<4; i++) {
		if ((b[v[i]]&0x80))
			boxes.push_back(NodeBox((-0.25+x[i])*BS,0.0625*BS,-0.125*BS,(-0.0625+x[i])*BS,0.125*BS,-0.0625*BS));
		if ((b[v[i]]&0x04))
			boxes.push_back(NodeBox((-0.125+x[i])*BS,-0.0625*BS,-0.125*BS,(-0.0625+x[i])*BS,0.0625*BS,-0.0625*BS));
		if ((b[v[i]]&0x08))
			boxes.push_back(NodeBox((-0.125+x[i])*BS,-0.25*BS,-0.125*BS,(-0.0625+x[i])*BS,-0.125*BS,-0.0625*BS));
		if ((b[v[i]]&0x10))
			boxes.push_back(NodeBox((-0.25+x[i])*BS,-0.3125*BS,-0.125*BS,(-0.0625+x[i])*BS,-0.25*BS,-0.0625*BS));
		if ((b[v[i]]&0x20))
			boxes.push_back(NodeBox((-0.25+x[i])*BS,-0.25*BS,-0.125*BS,(-0.1875+x[i])*BS,-0.125*BS,-0.0625*BS));
		if ((b[v[i]]&0x40))
			boxes.push_back(NodeBox((-0.25+x[i])*BS,-0.0625*BS,-0.125*BS,(-0.1875+x[i])*BS,0.0625*BS,-0.0625*BS));
		if ((b[v[i]]&0x02))
			boxes.push_back(NodeBox((-0.1875+x[i])*BS,-0.125*BS,-0.125*BS,(-0.125+x[i])*BS,-0.0625*BS,-0.0625*BS));
	}

	return transformNodeBox(n,boxes);
}
bool ClockNodeMetadata::step(float dtime, v3s16 pos, ServerEnvironment *env)
{
	u32 t = env->getTimeOfDay();
	t /= 10;
	if (t == m_time)
		return false;
	m_time = t;
	return true;
}

/*
	CircuitNodeMetadata
*/

// Prototype
CircuitNodeMetadata proto_CircuitNodeMetadata;

CircuitNodeMetadata::CircuitNodeMetadata():
	m_energy(0),
	m_ptime(0)
{
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
CircuitNodeMetadata::~CircuitNodeMetadata()
{
}
u16 CircuitNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_MESEWIRE;
}
NodeMetadata* CircuitNodeMetadata::create(std::istream &is)
{
	CircuitNodeMetadata *d = new CircuitNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* CircuitNodeMetadata::clone()
{
	CircuitNodeMetadata *d = new CircuitNodeMetadata();
	return d;
}
void CircuitNodeMetadata::serializeBody(std::ostream &os)
{
	os<<itos(m_energy) << " ";
	os<<itos(m_ptime*10)<<" ";
	os<<itos(m_sources.size()) << " ";
	for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
		os<<itos(i->first.X) << " ";
		os<<itos(i->first.Y) << " ";
		os<<itos(i->first.Z) << " ";
		os<<itos(i->second) << " ";
	}
}
bool CircuitNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	m_ptime += dtime;
	if (!m_energy || m_ptime < 0.6)
		return false;
	m_energy = 0;
	MapNode n = env->getMap().getNodeNoEx(pos);
	if (content_features(n).unpowered_node != CONTENT_IGNORE) {
		n.setContent(content_features(n).unpowered_node);
		env->getMap().addNodeWithEvent(pos, n);
	}
	return true;
}
bool CircuitNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	m_ptime = 0;
	if (!m_energy)
		m_sources.clear();
	if (m_sources[powersrc] == level)
		return true;
	if (level && m_sources[powersrc] > level)
		return false;
	m_sources[powersrc] = level;
	if (!level || m_energy < level) {
		m_energy = level;
		if (!level) {
			m_sources.erase(powersrc);
			for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
				u8 v = i->second;
				if (v > m_energy)
					m_energy = v;
			}
		}
	}
	return true;
}

/*
	SwitchNodeMetadata
*/

// Prototype
SwitchNodeMetadata proto_SwitchNodeMetadata;

SwitchNodeMetadata::SwitchNodeMetadata()
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 SwitchNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_SWITCH;
}
NodeMetadata* SwitchNodeMetadata::create(std::istream &is)
{
	SwitchNodeMetadata *d = new SwitchNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* SwitchNodeMetadata::clone()
{
	SwitchNodeMetadata *d = new SwitchNodeMetadata();
	return d;
}
bool SwitchNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	if (!m_energy)
		return false;

	core::map<v3s16,MapBlock*> modified_blocks;
	env->propogateEnergy(ENERGY_MAX,pos,pos,pos,modified_blocks);
	MapEditEvent event;
	event.type = MEET_OTHER;
	for (core::map<v3s16, MapBlock*>::Iterator i = modified_blocks.getIterator(); i.atEnd() == false; i++) {
		v3s16 p = i->getKey();
		event.modified_blocks.insert(p, true);
	}
	env->getMap().dispatchEvent(&event);

	return true;
}
bool SwitchNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	m_ptime = 0;
	if (m_energy == level)
		return true;
	if (powersrc != pos)
		return false;
	m_energy = level;
	return true;
}
std::vector<NodeBox>  SwitchNodeMetadata::getNodeBoxes(MapNode &n)
{
	std::vector<NodeBox> boxes;
	boxes.clear();

	if (m_energy) {
		boxes.push_back(NodeBox(
			-0.125*BS,0.*BS,0.375*BS,-0.0625*BS,0.0625*BS,0.4375*BS
		));
	}else{
		boxes.push_back(NodeBox(
			0.0625*BS,0.*BS,0.375*BS,0.125*BS,0.0625*BS,0.4375*BS
		));
	}

	return transformNodeBox(n,boxes);
}


/*
	ButtonNodeMetadata
*/

// Prototype
ButtonNodeMetadata proto_ButtonNodeMetadata;

ButtonNodeMetadata::ButtonNodeMetadata()
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 ButtonNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_BUTTON;
}
NodeMetadata* ButtonNodeMetadata::create(std::istream &is)
{
	ButtonNodeMetadata *d = new ButtonNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* ButtonNodeMetadata::clone()
{
	ButtonNodeMetadata *d = new ButtonNodeMetadata();
	return d;
}
bool ButtonNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	if (!m_energy)
		return false;

	core::map<v3s16,MapBlock*> modified_blocks;
	env->propogateEnergy(ENERGY_MAX,pos,pos,pos,modified_blocks);
	MapEditEvent event;
	event.type = MEET_OTHER;
	for (core::map<v3s16, MapBlock*>::Iterator i = modified_blocks.getIterator(); i.atEnd() == false; i++) {
		v3s16 p = i->getKey();
		event.modified_blocks.insert(p, true);
	}
	env->getMap().dispatchEvent(&event);

	m_energy = 0;
	return true;
}
bool ButtonNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	m_ptime = 0;
	if (m_energy == level)
		return true;
	if (powersrc != pos)
		return false;
	m_energy = level;
	return true;
}

/*
	SolarPanelNodeMetadata
*/

// Prototype
SolarPanelNodeMetadata proto_SolarPanelNodeMetadata;

SolarPanelNodeMetadata::SolarPanelNodeMetadata()
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 SolarPanelNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_SOLARPANEL;
}
NodeMetadata* SolarPanelNodeMetadata::create(std::istream &is)
{
	SolarPanelNodeMetadata *d = new SolarPanelNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* SolarPanelNodeMetadata::clone()
{
	SolarPanelNodeMetadata *d = new SolarPanelNodeMetadata();
	return d;
}
bool SolarPanelNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	MapNode n = env->getMap().getNodeNoEx(pos);
	if (n.getLightBlend(env->getDayNightRatio()) < 10) {
		m_energy = 0;
		return false;
	}

	m_energy = ENERGY_MAX;
	core::map<v3s16,MapBlock*> modified_blocks;
	env->propogateEnergy(ENERGY_MAX,pos,pos,pos,modified_blocks);
	MapEditEvent event;
	event.type = MEET_OTHER;
	for (core::map<v3s16, MapBlock*>::Iterator i = modified_blocks.getIterator(); i.atEnd() == false; i++) {
		v3s16 p = i->getKey();
		event.modified_blocks.insert(p, true);
	}
	env->getMap().dispatchEvent(&event);

	return true;
}
bool SolarPanelNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	if (!m_energy)
		return false;
	return true;
}

/*
	WaterWheelNodeMetadata
*/

// Prototype
WaterWheelNodeMetadata proto_WaterWheelNodeMetadata;

WaterWheelNodeMetadata::WaterWheelNodeMetadata()
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 WaterWheelNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_WATERWHEEL;
}
NodeMetadata* WaterWheelNodeMetadata::create(std::istream &is)
{
	WaterWheelNodeMetadata *d = new WaterWheelNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* WaterWheelNodeMetadata::clone()
{
	WaterWheelNodeMetadata *d = new WaterWheelNodeMetadata();
	return d;
}
bool WaterWheelNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	MapNode n = env->getMap().getNodeNoEx(pos);
	v3s16 dir = n.getRotation();
	if (dir == v3s16(1,1,1)) {
		dir = v3s16(0,0,-1);
	}else if (dir == v3s16(-1,1,1)) {
		dir = v3s16(-1,0,0);
	}else if (dir == v3s16(-1,1,-1)) {
		dir = v3s16(0,0,1);
	}else if (dir == v3s16(1,1,-1)) {
		dir = v3s16(1,0,0);
	}
	MapNode inlet = env->getMap().getNodeNoEx(pos-dir);
	MapNode outlet = env->getMap().getNodeNoEx(pos+dir);
	if (inlet.getContent() != CONTENT_WATERSOURCE) {
		if (outlet.getContent() == CONTENT_WATERSOURCE)
			env->getMap().removeNodeWithEvent(pos+dir);
		if (m_energy) {
			m_energy = 0;
			return true;
		}
		return false;
	}
	if (outlet.getContent() != CONTENT_WATERSOURCE) {
		if (outlet.getContent() != CONTENT_AIR) {
			if (m_energy) {
				m_energy = 0;
				return true;
			}
			return false;
		}
		outlet.setContent(CONTENT_WATERSOURCE);
		env->getMap().addNodeWithEvent(pos+dir,outlet);
	}

	m_energy = ENERGY_MAX;
	core::map<v3s16,MapBlock*> modified_blocks;
	env->propogateEnergy(ENERGY_MAX,pos,pos,pos,modified_blocks);
	MapEditEvent event;
	event.type = MEET_OTHER;
	for (core::map<v3s16, MapBlock*>::Iterator i = modified_blocks.getIterator(); i.atEnd() == false; i++) {
		v3s16 p = i->getKey();
		event.modified_blocks.insert(p, true);
	}
	env->getMap().dispatchEvent(&event);

	return true;
}
bool WaterWheelNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	if (!m_energy)
		return false;
	return true;
}

/*
	SourceNodeMetadata
*/

// Prototype
SourceNodeMetadata proto_SourceNodeMetadata;

SourceNodeMetadata::SourceNodeMetadata()
{
	m_energy = ENERGY_MAX;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 SourceNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_REACTOR;
}
NodeMetadata* SourceNodeMetadata::create(std::istream &is)
{
	SourceNodeMetadata *d = new SourceNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* SourceNodeMetadata::clone()
{
	SourceNodeMetadata *d = new SourceNodeMetadata();
	return d;
}

/*
	NotGateNodeMetadata
*/

// Prototype
NotGateNodeMetadata proto_NotGateNodeMetadata;

NotGateNodeMetadata::NotGateNodeMetadata()
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 NotGateNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_NOTGATE;
}
NodeMetadata* NotGateNodeMetadata::create(std::istream &is)
{
	NotGateNodeMetadata *d = new NotGateNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* NotGateNodeMetadata::clone()
{
	NotGateNodeMetadata *d = new NotGateNodeMetadata();
	return d;
}
bool NotGateNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	m_ptime += dtime;
	if (m_ptime < 0.6)
		return false;

	m_energy = 0;

	core::map<v3s16,MapBlock*> modified_blocks;
	env->propogateEnergy(ENERGY_MAX,pos,pos,pos,modified_blocks);
	MapEditEvent event;
	event.type = MEET_OTHER;
	for (core::map<v3s16, MapBlock*>::Iterator i = modified_blocks.getIterator(); i.atEnd() == false; i++) {
		v3s16 p = i->getKey();
		event.modified_blocks.insert(p, true);
	}
	env->getMap().dispatchEvent(&event);

	return true;
}
bool NotGateNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	if (powersrc == pos)
		return true;
	m_ptime = 0;
	if (level && m_sources[powersrc] > level)
		return false;
	if (!level || m_energy < level) {
		m_energy = level;
		if (!level) {
			m_sources.erase(powersrc);
			for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
				u8 v = i->second;
				if (v > m_energy)
					m_energy = v;
			}
		}
	}
	return true;
}

/*
	RepeaterNodeMetadata
*/

// Prototype
RepeaterNodeMetadata proto_RepeaterNodeMetadata;

RepeaterNodeMetadata::RepeaterNodeMetadata():
	m_ticks(0)
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 RepeaterNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_REPEATER;
}
NodeMetadata* RepeaterNodeMetadata::create(std::istream &is)
{
	RepeaterNodeMetadata *d = new RepeaterNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ticks = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* RepeaterNodeMetadata::clone()
{
	RepeaterNodeMetadata *d = new RepeaterNodeMetadata();
	return d;
}
void RepeaterNodeMetadata::serializeBody(std::ostream &os)
{
	os<<itos(m_energy) << " ";
	os<<itos(m_ticks) << " ";
	os<<itos(m_ptime*10)<<" ";
	os<<itos(m_sources.size()) << " ";
	for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
		os<<itos(i->first.X) << " ";
		os<<itos(i->first.Y) << " ";
		os<<itos(i->first.Z) << " ";
		os<<itos(i->second) << " ";
	}
}
bool RepeaterNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	m_ptime += dtime;
	if (m_ptime > 1.0 && m_ticks > 0) {
		m_energy = 0;
		m_ticks--;
	}
	if (!m_energy && !m_ticks) {
		return false;
	}else if (m_energy && m_ticks < 6) {
		m_ticks++;
		return true;
	}

	core::map<v3s16,MapBlock*> modified_blocks;
	env->propogateEnergy(ENERGY_MAX,pos,pos,pos,modified_blocks);
	MapEditEvent event;
	event.type = MEET_OTHER;
	for (core::map<v3s16, MapBlock*>::Iterator i = modified_blocks.getIterator(); i.atEnd() == false; i++) {
		v3s16 p = i->getKey();
		event.modified_blocks.insert(p, true);
	}
	env->getMap().dispatchEvent(&event);

	return true;
}
bool RepeaterNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	if (powersrc == pos)
		return true;
	m_ptime = 0;
	if (level && m_sources[powersrc] > level)
		return false;
	if (!level || m_energy < level) {
		m_energy = level;
		if (!level) {
			m_sources.erase(powersrc);
			for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
				u8 v = i->second;
				if (v > m_energy)
					m_energy = v;
			}
		}
	}
	return true;
}


/*
	DoorNodeMetadata
*/

// Prototype
DoorNodeMetadata proto_DoorNodeMetadata;

DoorNodeMetadata::DoorNodeMetadata():
	m_otime(0)
{
	m_energy = 0;
	m_ptime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 DoorNodeMetadata::typeId() const
{
	return CONTENT_STEEL_DOOR_LB;
}
NodeMetadata* DoorNodeMetadata::create(std::istream &is)
{
	DoorNodeMetadata *d = new DoorNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	is>>temp;
	d->m_otime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* DoorNodeMetadata::clone()
{
	DoorNodeMetadata *d = new DoorNodeMetadata();
	return d;
}
void DoorNodeMetadata::serializeBody(std::ostream &os)
{
	os<<itos(m_energy) << " ";
	os<<itos(m_ptime*10)<<" ";
	os<<itos(m_otime*10)<<" ";
	os<<itos(m_sources.size()) << " ";
	for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
		os<<itos(i->first.X) << " ";
		os<<itos(i->first.Y) << " ";
		os<<itos(i->first.Z) << " ";
		os<<itos(i->second) << " ";
	}
}
bool DoorNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	if (m_ptime == 0.0 && m_otime == 0.0 && m_energy == 0) {
		MapNode n = env->getMap().getNodeNoEx(pos);
		if ((n.getContent()&CONTENT_DOOR_OPEN_MASK) == CONTENT_DOOR_OPEN_MASK)
			m_otime = 5.0;
	}
	m_ptime += dtime;
	if (!m_energy) {
		if (m_otime <= 0.0) {
			MapNode n = env->getMap().getNodeNoEx(pos);
			content_t c = n.getContent();
			if ((c&CONTENT_DOOR_OPEN_MASK) == CONTENT_DOOR_OPEN_MASK) {
				n.setContent(n.getContent()&~CONTENT_DOOR_OPEN_MASK);
				env->setPostStepNodeSwap(pos,n);
				return true;
			}
			return false;
		}
		m_otime -= dtime;
		return false;
	}else{
		MapNode n = env->getMap().getNodeNoEx(pos);
		content_t c = n.getContent();
		if ((c&CONTENT_DOOR_OPEN_MASK) != CONTENT_DOOR_OPEN_MASK) {
			n.setContent(n.getContent()|CONTENT_DOOR_OPEN_MASK);
			env->setPostStepNodeSwap(pos,n);
		}
	}
	if (m_ptime < 0.3)
		return false;
	m_energy = 0;
	m_otime = 5.0;
	return true;
}
bool DoorNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	m_ptime = 0;
	if (!m_energy)
		m_sources.clear();
	if (powersrc == pos) {
		m_otime = 5.0;
		return true;
	}
	if (m_sources[powersrc] == level)
		return true;
	if (level && m_sources[powersrc] > level)
		return false;
	m_sources[powersrc] = level;
	if (!level || m_energy < level) {
		m_energy = level;
		if (!level) {
			m_sources.erase(powersrc);
			for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
				u8 v = i->second;
				if (v > m_energy)
					m_energy = v;
			}
		}
	}
	return true;
}


/*
	PistonNodeMetadata
*/

// Prototype
PistonNodeMetadata proto_PistonNodeMetadata;

PistonNodeMetadata::PistonNodeMetadata()
{
	m_energy = 0;
	m_ptime = 0;
	m_otime = 0;
	m_sources.clear();
	NodeMetadata::registerType(typeId(), create);
}
u16 PistonNodeMetadata::typeId() const
{
	return CONTENT_CIRCUIT_PISTON;
}
NodeMetadata* PistonNodeMetadata::create(std::istream &is)
{
	PistonNodeMetadata *d = new PistonNodeMetadata();
	int temp;
	is>>temp;
	d->m_energy = temp;
	is>>temp;
	d->m_ptime = (float)temp/10;
	int i;
	is>>i;
	v3s16 p;
	for (; i > 0; i--) {
		is>>temp;
		p.X = temp;
		is>>temp;
		p.Y = temp;
		is>>temp;
		p.Z = temp;
		is>>temp;
		d->m_sources[p] = temp;
	}
	return d;
}
NodeMetadata* PistonNodeMetadata::clone()
{
	PistonNodeMetadata *d = new PistonNodeMetadata();
	d->m_energy = m_energy;
	d->m_ptime = m_ptime;
	d->m_sources = m_sources;
	return d;
}
void PistonNodeMetadata::serializeBody(std::ostream &os)
{
	os<<itos(m_energy) << " ";
	os<<itos(m_ptime*10)<<" ";
	os<<itos(m_sources.size()) << " ";
	for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
		os<<itos(i->first.X) << " ";
		os<<itos(i->first.Y) << " ";
		os<<itos(i->first.Z) << " ";
		os<<itos(i->second) << " ";
	}
}
bool PistonNodeMetadata::stepCircuit(float dtime, v3s16 pos, ServerEnvironment *env)
{
	m_ptime += dtime;
	if (!m_energy) {
		MapNode n = env->getMap().getNodeNoEx(pos);
		v3s16 dir = n.getRotation();
		switch (n.getContent()) {
		case CONTENT_CIRCUIT_PISTON:
		{
			if (dir == v3s16(1,1,1)) {
				dir = v3s16(0,0,-1);
			}else if (dir == v3s16(-1,1,1)) {
				dir = v3s16(-1,0,0);
			}else if (dir == v3s16(-1,1,-1)) {
				dir = v3s16(0,0,1);
			}else if (dir == v3s16(1,1,-1)) {
				dir = v3s16(1,0,0);
			}
			n.setContent(CONTENT_CIRCUIT_PISTON_OFF);
			contract(pos,dir,false,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_PISTON_UP:
		{
			dir = v3s16(0,1,0);
			n.setContent(CONTENT_CIRCUIT_PISTON_UP_OFF);
			contract(pos,dir,false,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_PISTON_DOWN:
		{
			dir = v3s16(0,-1,0);
			n.setContent(CONTENT_CIRCUIT_PISTON_DOWN_OFF);
			contract(pos,dir,false,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_STICKYPISTON:
		{
			if (dir == v3s16(1,1,1)) {
				dir = v3s16(0,0,-1);
			}else if (dir == v3s16(-1,1,1)) {
				dir = v3s16(-1,0,0);
			}else if (dir == v3s16(-1,1,-1)) {
				dir = v3s16(0,0,1);
			}else if (dir == v3s16(1,1,-1)) {
				dir = v3s16(1,0,0);
			}
			n.setContent(CONTENT_CIRCUIT_STICKYPISTON_OFF);
			contract(pos,dir,true,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_UP:
		{
			dir = v3s16(0,1,0);
			n.setContent(CONTENT_CIRCUIT_STICKYPISTON_UP_OFF);
			contract(pos,dir,true,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_DOWN:
		{
			dir = v3s16(0,-1,0);
			n.setContent(CONTENT_CIRCUIT_STICKYPISTON_DOWN_OFF);
			contract(pos,dir,true,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_PISTON_OFF:
		{
			if (dir == v3s16(1,1,1)) {
				dir = v3s16(0,0,-1);
			}else if (dir == v3s16(-1,1,1)) {
				dir = v3s16(-1,0,0);
			}else if (dir == v3s16(-1,1,-1)) {
				dir = v3s16(0,0,1);
			}else if (dir == v3s16(1,1,-1)) {
				dir = v3s16(1,0,0);
			}
			if (env->getMap().getNodeNoEx(pos+dir).getContent() == CONTENT_CIRCUIT_PISTON_ARM)
				contract(pos,dir,false,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_PISTON_UP_OFF:
		{
			dir = v3s16(0,1,0);
			if (env->getMap().getNodeNoEx(pos+dir).getContent() == CONTENT_CIRCUIT_PISTON_UP_ARM)
				contract(pos,dir,false,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_PISTON_DOWN_OFF:
		{
			dir = v3s16(0,-1,0);
			if (env->getMap().getNodeNoEx(pos+dir).getContent() == CONTENT_CIRCUIT_PISTON_DOWN_ARM)
				contract(pos,dir,false,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_OFF:
		{
			if (dir == v3s16(1,1,1)) {
				dir = v3s16(0,0,-1);
			}else if (dir == v3s16(-1,1,1)) {
				dir = v3s16(-1,0,0);
			}else if (dir == v3s16(-1,1,-1)) {
				dir = v3s16(0,0,1);
			}else if (dir == v3s16(1,1,-1)) {
				dir = v3s16(1,0,0);
			}
			if (env->getMap().getNodeNoEx(pos+dir).getContent() == CONTENT_CIRCUIT_STICKYPISTON_ARM)
				contract(pos,dir,true,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_UP_OFF:
		{
			dir = v3s16(0,1,0);
			if (env->getMap().getNodeNoEx(pos+dir).getContent() == CONTENT_CIRCUIT_STICKYPISTON_UP_ARM)
				contract(pos,dir,true,n,env);
			return true;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_DOWN_OFF:
		{
			dir = v3s16(0,-1,0);
			if (env->getMap().getNodeNoEx(pos+dir).getContent() == CONTENT_CIRCUIT_STICKYPISTON_DOWN_ARM)
				contract(pos,dir,true,n,env);
			return true;
		}
		default:;
		}
		return false;
	}else{
		m_otime += dtime;
		if (m_otime < 3)
			return false;
		MapNode n = env->getMap().getNodeNoEx(pos);
		v3s16 dir = n.getRotation();
		switch (n.getContent()) {
		case CONTENT_CIRCUIT_PISTON_OFF:
		{
			if (dir == v3s16(1,1,1)) {
				dir = v3s16(0,0,-1);
			}else if (dir == v3s16(-1,1,1)) {
				dir = v3s16(-1,0,0);
			}else if (dir == v3s16(-1,1,-1)) {
				dir = v3s16(0,0,1);
			}else if (dir == v3s16(1,1,-1)) {
				dir = v3s16(1,0,0);
			}
			n.setContent(CONTENT_CIRCUIT_PISTON);
			extend(pos,dir,CONTENT_CIRCUIT_PISTON_ARM,n,env);
			break;
		}
		case CONTENT_CIRCUIT_PISTON_UP_OFF:
		{
			dir = v3s16(0,1,0);
			n.setContent(CONTENT_CIRCUIT_PISTON_UP);
			extend(pos,dir,CONTENT_CIRCUIT_PISTON_UP_ARM,n,env);
			break;
		}
		case CONTENT_CIRCUIT_PISTON_DOWN_OFF:
		{
			dir = v3s16(0,-1,0);
			n.setContent(CONTENT_CIRCUIT_PISTON_DOWN);
			extend(pos,dir,CONTENT_CIRCUIT_PISTON_DOWN_ARM,n,env);
			break;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_OFF:
		{
			if (dir == v3s16(1,1,1)) {
				dir = v3s16(0,0,-1);
			}else if (dir == v3s16(-1,1,1)) {
				dir = v3s16(-1,0,0);
			}else if (dir == v3s16(-1,1,-1)) {
				dir = v3s16(0,0,1);
			}else if (dir == v3s16(1,1,-1)) {
				dir = v3s16(1,0,0);
			}
			n.setContent(CONTENT_CIRCUIT_STICKYPISTON);
			extend(pos,dir,CONTENT_CIRCUIT_STICKYPISTON_ARM,n,env);
			break;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_UP_OFF:
		{
			dir = v3s16(0,1,0);
			n.setContent(CONTENT_CIRCUIT_STICKYPISTON_UP);
			extend(pos,dir,CONTENT_CIRCUIT_STICKYPISTON_UP_ARM,n,env);
			break;
		}
		case CONTENT_CIRCUIT_STICKYPISTON_DOWN_OFF:
		{
			dir = v3s16(0,-1,0);
			n.setContent(CONTENT_CIRCUIT_STICKYPISTON_DOWN);
			extend(pos,dir,CONTENT_CIRCUIT_STICKYPISTON_DOWN_ARM,n,env);
			break;
		}
		default:;
		}
	}
	if (m_ptime < 3)
		return false;
	//m_otime += dtime;
	//if (m_otime < 1.0)
		//return false;
	m_energy = 0;
	return true;
}
bool PistonNodeMetadata::energise(u8 level, v3s16 powersrc, v3s16 signalsrc, v3s16 pos)
{
	m_ptime = 0;
	if (!m_energy)
		m_sources.clear();
	if (m_sources[powersrc] == level)
		return true;
	if (level && m_sources[powersrc] > level)
		return false;
	m_sources[powersrc] = level;
	if (!level || m_energy < level) {
		m_energy = level;
		if (!level) {
			m_sources.erase(powersrc);
			for (std::map<v3s16,u8>::iterator i = m_sources.begin(); i != m_sources.end(); i++) {
				u8 v = i->second;
				if (v > m_energy)
					m_energy = v;
			}
		}
	}
	if (level)
		m_otime = 0;
	return true;
}
bool PistonNodeMetadata::extend(v3s16 pos, v3s16 dir, content_t arm, MapNode piston, ServerEnvironment *env)
{
	bool can_extend = false;
	v3s16 epos = pos;
	s16 max_d = g_settings->getS16("borderstone_radius");
	for (int i=0; i<17; i++) {
		epos += dir;
		v3s16 test_p;
		MapNode testnode;
		for(s16 z=-max_d; z<=max_d; z++) {
		for(s16 y=-max_d; y<=max_d; y++) {
		for(s16 x=-max_d; x<=max_d; x++) {
			test_p = epos + v3s16(x,y,z);
			testnode = env->getMap().getNodeNoEx(test_p);
			if (testnode.getContent() == CONTENT_IGNORE || testnode.getContent() == CONTENT_BORDERSTONE)
				return false;
		}
		}
		}
		MapNode n = env->getMap().getNodeNoEx(epos);
		if (n.getContent() == CONTENT_IGNORE)
			return false;
		ContentFeatures &f = content_features(n);
		if (f.pressure_type == CST_SOLID)
			return false;
		if (f.pressure_type == CST_CRUSHED || f.pressure_type == CST_CRUSHABLE) {
			can_extend = true;
			break;
		}
	}
	if (!can_extend)
		return false;
	env->addEnvEvent(ENV_EVENT_SOUND,intToFloat(pos,BS),"env-piston");
	MapNode n_prev;
	MapNode n_cur;
	MapNode n_next;
	v3s16 p_prev = pos;
	v3s16 p_cur = p_prev+dir;
	v3s16 p_next = p_cur+dir;
	env->getMap().addNodeWithEvent(pos,piston);

	if (arm == CONTENT_CIRCUIT_PISTON_ARM || arm == CONTENT_CIRCUIT_STICKYPISTON_ARM)
		n_prev = env->getMap().getNodeNoEx(p_prev);
	n_prev.setContent(arm);
	n_cur = env->getMap().getNodeNoEx(p_cur);
	for (int i=0; i<17; i++) {
		ContentFeatures &f = content_features(n_cur);
		n_next = env->getMap().getNodeNoEx(p_next);
		env->getMap().addNodeWithEvent(p_cur,n_prev);
		if (f.pressure_type == CST_CRUSHED)
			break;
		if (f.pressure_type == CST_CRUSHABLE && n_next.getContent() != CONTENT_AIR)
			break;
		n_prev = n_cur;
		n_cur = n_next;
		p_prev = p_cur;
		p_cur = p_next;
		p_next += dir;
	}

	return true;
}
bool PistonNodeMetadata::contract(v3s16 pos, v3s16 dir, bool sticky, MapNode piston, ServerEnvironment *env)
{
	bool dropping = false;
	bool contract = true;
	bool remove_arm = true;
	if (dir.Y == 1)
		dropping = true;
	if (sticky || dropping) {
		s16 max_d = g_settings->getS16("borderstone_radius");
		v3s16 p_cur = pos+dir;
		v3s16 p_next = p_cur+dir;
		bool walk = true;
		for (int i=0; walk && i<16; i++) {
			MapNode n = env->getMap().getNodeNoEx(p_next);
			if (n.getContent() == CONTENT_IGNORE) {
				contract = false;
				walk = false;
				break;
			}
			ContentFeatures &f = content_features(n);
			if (f.pressure_type == CST_SOLID)
				break;
			if (f.liquid_type != LIQUID_NONE)
				break;
			if ((!sticky || i) && f.pressure_type != CST_DROPABLE)
				break;
			v3s16 test_p;
			MapNode testnode;
			for(s16 z=-max_d; walk && z<=max_d; z++) {
			for(s16 y=-max_d; walk && y<=max_d; y++) {
			for(s16 x=-max_d; walk && x<=max_d; x++) {
				test_p = p_cur + v3s16(x,y,z);
				testnode = env->getMap().getNodeNoEx(test_p);
				if (testnode.getContent() == CONTENT_IGNORE || testnode.getContent() == CONTENT_BORDERSTONE) {
					walk = false;
					contract = false;
					break;
				}
			}
			}
			}
			if (!dropping)
				break;
			p_cur = p_next;
			p_next += dir;
		}

		if (walk) {
			p_cur = pos+dir;
			p_next = p_cur+dir;

			for (int i=0; i<16; i++) {
				MapNode n = env->getMap().getNodeNoEx(p_next);
				if (n.getContent() == CONTENT_IGNORE)
					break;
				ContentFeatures &f = content_features(n);
				if (f.pressure_type == CST_SOLID)
					break;
				if (f.liquid_type != LIQUID_NONE)
					break;
				if ((!sticky || i) && f.pressure_type != CST_DROPABLE)
					break;
				env->getMap().removeNodeWithEvent(p_next);
				env->getMap().addNodeWithEvent(p_cur,n);
				remove_arm = false;
				if (!dropping)
					break;
				p_cur = p_next;
				p_next += dir;
			}
		}
	}
	if (contract) {
		env->addEnvEvent(ENV_EVENT_SOUND,intToFloat(pos,BS),"env-piston");
		env->getMap().addNodeWithEvent(pos,piston);
		if (remove_arm)
			env->getMap().removeNodeWithEvent(pos+dir);
	}

	return true;
}
