/*
Minetest-c55
Copyright (C) 2010 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MINERAL_HEADER
#define MINERAL_HEADER

#include "inventory.h"

/*
	Minerals

	Value is stored in the lowest 5 bits of a MapNode's CPT_MINERAL
	type param.
*/

// Caches textures
void init_mineral();

#define MINERAL_NONE 0
#define MINERAL_COAL 1
#define MINERAL_IRON 2
#define MINERAL_TIN 3
#define MINERAL_COPPER 4
#define MINERAL_SILVER 5
#define MINERAL_GOLD 6
#define MINERAL_QUARTZ 7

#define MINERAL_COUNT 8

std::string mineral_block_texture(u8 mineral);

inline CraftItem * getDiggedMineralItem(u8 mineral)
{
	if (mineral == MINERAL_COAL) {
		return new CraftItem("lump_of_coal", 2);
	}else if(mineral == MINERAL_IRON) {
		return new CraftItem("lump_of_iron", 2);
	}else if(mineral == MINERAL_TIN) {
		return new CraftItem("lump_of_tin", 2);
	}else if(mineral == MINERAL_COPPER) {
		return new CraftItem("lump_of_copper", 2);
	}else if(mineral == MINERAL_SILVER) {
		return new CraftItem("lump_of_silver", 1);
	}else if(mineral == MINERAL_GOLD) {
		return new CraftItem("lump_of_gold", 1);
	}else if(mineral == MINERAL_QUARTZ) {
		return new CraftItem("lump_of_quartz", 1);
	}

	return NULL;
}

#endif

