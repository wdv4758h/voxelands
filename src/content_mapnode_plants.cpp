/************************************************************************
* Minetest-c55
* Copyright (C) 2010 celeron55, Perttu Ahola <celeron55@gmail.com>
*
* content_mapnode_special.cpp
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
************************************************************************/

// For g_settings
#include "main.h"

#include "content_mapnode.h"
#include "content_nodebox.h"
#include "content_list.h"
#include "content_craft.h"
#include "content_nodemeta.h"
#include "settings.h"

void content_mapnode_plants(bool repeat)
{
	bool new_style_leaves = g_settings->getBool("new_style_leaves");
	content_t i;
	ContentFeatures *f = NULL;


	i = CONTENT_TREE;
	f = &content_features(i);
	f->description = std::string("Tree");
	f->setAllTextures("tree.png");
	f->setTexture(0, "tree_top.png");
	f->setTexture(1, "tree_top.png");
	f->setInventoryTextureCube("tree_top.png", "tree.png", "tree.png");
	f->draw_type = CDT_CUBELIKE;
	f->is_ground_content = true;
	f->flammable = 2; // can be set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->cook_result = std::string("CraftItem lump_of_charcoal 1");
	f->fuel_time = 30;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_APPLE_TREE;
	f = &content_features(i);
	f->description = std::string("Apple Tree");
	f->setAllTextures("apple_tree.png");
	f->setTexture(0, "apple_tree_top.png");
	f->setTexture(1, "apple_tree_top.png");
	f->setInventoryTextureCube("apple_tree_top.png", "apple_tree.png", "apple_tree.png");
	f->draw_type = CDT_CUBELIKE;
	f->is_ground_content = true;
	f->flammable = 2; // can be set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->cook_result = std::string("CraftItem lump_of_charcoal 1");
	f->fuel_time = 30;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_JUNGLETREE;
	f = &content_features(i);
	f->description = std::string("Jungle Tree");
	f->setAllTextures("jungletree.png");
	f->setTexture(0, "jungletree_top.png");
	f->setTexture(1, "jungletree_top.png");
	f->setInventoryTextureCube("jungletree_top.png", "jungletree.png", "jungletree.png");
	f->draw_type = CDT_CUBELIKE;
	//f->is_ground_content = true;
	f->flammable = 2; // can be set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->fuel_time = 30;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);

	i = CONTENT_CONIFER_TREE;
	f = &content_features(i);
	f->description = std::string("Conifer Tree");
	f->setAllTextures("conifer_tree.png");
	f->setTexture(0, "conifer_tree_top.png");
	f->setTexture(1, "conifer_tree_top.png");
	f->is_ground_content = true;
	f->flammable = 2; // can be set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->cook_result = std::string("CraftItem lump_of_charcoal 1");
	f->fuel_time = 30;
	f->draw_type = CDT_NODEBOX;
	f->param_type = CPT_LIGHT;
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->is_ground_content = true;
	f->solidness = 0; // drawn separately, makes no faces
	f->visual_solidness = 0;
	f->fuel_time = 25;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->setNodeBox(core::aabbox3d<f32>(
		-0.4375*BS,
		-0.5*BS,
		-0.4375*BS,
		0.4375*BS,
		0.5*BS,
		0.4375*BS
	));
	f->setInventoryTextureNodeBox(i,"conifer_tree_top.png", "conifer_tree.png", "conifer_tree.png");
	f->type = CMT_WOOD;
	f->hardness = 0.8;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_YOUNG_TREE;
	f = &content_features(i);
	f->description = std::string("Young Tree");
	f->setAllTextures("tree.png");
	f->setTexture(0, "tree_top.png");
	f->setTexture(1, "tree_top.png");
	f->draw_type = CDT_NODEBOX;
	f->solidness = 0; // drawn separately, makes no faces
	f->param_type = CPT_LIGHT;
	content_nodebox_youngtree(f);
	f->setInventoryTextureNodeBox(i,"tree_top.png", "tree.png", "tree.png");
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->special_alternate_node = CONTENT_LEAVES;
	f->fuel_time = 20;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);

	i = CONTENT_YOUNG_JUNGLETREE;
	f = &content_features(i);
	f->description = std::string("Young Jungle Tree");
	f->setAllTextures("jungletree.png");
	f->setTexture(0, "jungletree_top.png");
	f->setTexture(1, "jungletree_top.png");
	f->draw_type = CDT_NODEBOX;
	f->solidness = 0; // drawn separately, makes no faces
	f->param_type = CPT_LIGHT;
	content_nodebox_youngtree(f);
	f->setInventoryTextureNodeBox(i,"jungletree_top.png", "jungletree.png", "jungletree.png");
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->special_alternate_node = CONTENT_JUNGLELEAVES;
	f->fuel_time = 20;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);

	i = CONTENT_YOUNG_APPLE_TREE;
	f = &content_features(i);
	f->description = std::string("Young Apple Tree");
	f->setAllTextures("apple_tree.png");
	f->setTexture(0, "apple_tree_top.png");
	f->setTexture(1, "apple_tree_top.png");
	f->draw_type = CDT_NODEBOX;
	f->solidness = 0; // drawn separately, makes no faces
	f->param_type = CPT_LIGHT;
	content_nodebox_youngtree(f);
	f->setInventoryTextureNodeBox(i,"apple_tree_top.png", "apple_tree.png", "apple_tree.png");
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->special_alternate_node = CONTENT_APPLE_LEAVES;
	f->fuel_time = 20;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);

	i = CONTENT_YOUNG_CONIFER_TREE;
	f = &content_features(i);
	f->description = std::string("Young Conifer Tree");
	f->setAllTextures("conifer_tree.png");
	f->setTexture(0, "conifer_tree_top.png");
	f->setTexture(1, "conifer_tree_top.png");
	f->draw_type = CDT_NODEBOX;
	f->solidness = 0; // drawn separately, makes no faces
	f->param_type = CPT_LIGHT;
	content_nodebox_youngtree(f);
	f->setInventoryTextureNodeBox(i,"conifer_tree_top.png", "conifer_tree.png", "conifer_tree.png");
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->special_alternate_node = CONTENT_CONIFER_LEAVES;
	f->fuel_time = 20;
	f->type = CMT_WOOD;
	f->hardness = 1.0;
	lists::add("creative",i);

	i = CONTENT_JUNGLEGRASS;
	f = &content_features(i);
	f->description = std::string("Jungle Grass");
	f->setInventoryTexture("junglegrass.png");
	f->setAllTextures("junglegrass.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE_LGE;
	//f->is_ground_content = true;
	f->air_equivalent = false; // grass grows underneath
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_LEAVES;
	f = &content_features(i);
	f->description = std::string("Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	//f->is_ground_content = true;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("leaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("leaves.png", "leaves.png", "leaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:leaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->extra_dug_item = std::string("MaterialItem2 ")+itos(CONTENT_SAPLING)+" 1";
	f->extra_dug_item_rarity = 20;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("decrafting",i);
	lists::add("cooking",i);

	i = CONTENT_APPLE_LEAVES;
	f = &content_features(i);
	f->description = std::string("Apple Tree Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	//f->is_ground_content = true;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("apple_leaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("apple_leaves.png", "apple_leaves.png", "apple_leaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:apple_leaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->extra_dug_item = std::string("MaterialItem2 ")+itos(CONTENT_APPLE_SAPLING)+" 1";
	f->extra_dug_item_rarity = 20;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("decrafting",i);
	lists::add("cooking",i);

	i = CONTENT_JUNGLELEAVES;
	f = &content_features(i);
	f->description = std::string("Jungle Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	//f->is_ground_content = true;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("jungleleaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("jungleleaves.png", "jungleleaves.png", "jungleleaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:jungleleaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->extra_dug_item = std::string("MaterialItem2 ")+itos(CONTENT_JUNGLESAPLING)+" 1";
	f->extra_dug_item_rarity = 20;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("decrafting",i);
	lists::add("cooking",i);

	i = CONTENT_CONIFER_LEAVES;
	f = &content_features(i);
	f->description = std::string("Conifer Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	//f->is_ground_content = true;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("conifer_leaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("conifer_leaves.png", "conifer_leaves.png", "conifer_leaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:conifer_leaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->extra_dug_item = std::string("MaterialItem2 ")+itos(CONTENT_CONIFER_SAPLING)+" 1";
	f->extra_dug_item_rarity = 20;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("decrafting",i);
	lists::add("cooking",i);

	i = CONTENT_TRIMMED_LEAVES;
	f = &content_features(i);
	f->description = std::string("Trimmed Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("leaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("leaves.png", "leaves.png", "leaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:leaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_TRIMMED_APPLE_LEAVES;
	f = &content_features(i);
	f->description = std::string("Trimmed Apple Tree Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("apple_leaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("apple_leaves.png", "apple_leaves.png", "apple_leaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:apple_leaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_TRIMMED_JUNGLE_LEAVES;
	f = &content_features(i);
	f->description = std::string("Trimmed Jungle Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("jungleleaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("jungleleaves.png", "jungleleaves.png", "jungleleaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:jungleleaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_TRIMMED_CONIFER_LEAVES;
	f = &content_features(i);
	f->description = std::string("Trimmed Conifer Leaves");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("conifer_leaves.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("conifer_leaves.png", "conifer_leaves.png", "conifer_leaves.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:conifer_leaves.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/16;
	f->cook_result = std::string("CraftItem lump_of_resin 1");
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);
	lists::add("cooking",i);

	i = CONTENT_APPLE_BLOSSOM;
	f = &content_features(i);
	f->description = std::string("Apple Tree Blossom");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	//f->is_ground_content = true;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("apple_leaves.png^apple_blossom.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("apple_leaves.png^apple_blossom.png", "apple_leaves.png^apple_blossom.png", "apple_leaves.png^apple_blossom.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:apple_leaves.png^apple_blossom.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->extra_dug_item = std::string("CraftItem apple_blossom 1");
	f->extra_dug_item_rarity = 1;
	f->dug_item = std::string("MaterialItem2 ")+itos(CONTENT_APPLE_LEAVES)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("decrafting",i);

	i = CONTENT_TRIMMED_APPLE_BLOSSOM;
	f = &content_features(i);
	f->description = std::string("Trimmed Apple Tree Blossom");
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	if (new_style_leaves) {
		f->draw_type = CDT_GLASSLIKE;
		f->solidness = 0; // drawn separately, makes no faces
		f->visual_solidness = 1;
		f->setAllTextures("apple_leaves.png^apple_blossom.png");
#ifndef SERVER
		f->setAllTextureTypes(MATERIAL_ALPHA_SIMPLE);
#endif
		f->setInventoryTextureCube("apple_leaves.png^apple_blossom.png", "apple_leaves.png^apple_blossom.png", "apple_leaves.png^apple_blossom.png");
	}else{
		f->draw_type = CDT_CUBELIKE;
		f->setAllTextures("[noalpha:apple_leaves.png^apple_blossom.png");
	}
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->type = CMT_PLANT;
	f->hardness = 0.15;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_CACTUS_BLOSSOM;
	f = &content_features(i);
	f->description = std::string("Cactus Blossom");
	f->setInventoryTexture("cactus_blossom.png");
	f->setAllTextures("cactus_blossom.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->air_equivalent = false; // grass grows underneath
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_CACTUS_FLOWER;
	f = &content_features(i);
	f->description = std::string("Cactus Flower");
	f->setInventoryTexture("cactus_flower.png");
	f->setAllTextures("cactus_flower.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->air_equivalent = false; // grass grows underneath
	f->dug_item = std::string("MaterialItem2 ")+itos(CONTENT_CACTUS_BLOSSOM)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);
	lists::add("decrafting",i);

	i = CONTENT_CACTUS_FRUIT;
	f = &content_features(i);
	f->description = std::string("Cactus Berry");
	f->setInventoryTexture("cactus_fruit.png");
	f->setAllTextures("cactus_fruit.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->air_equivalent = false; // grass grows underneath
	f->dug_item = std::string("CraftItem cactus_fruit 1");
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_CACTUS;
	f = &content_features(i);
	f->description = std::string("Cactus");
	f->setAllTextures("cactus_side.png");
	f->setTexture(0, "cactus_top.png");
	f->setTexture(1, "cactus_top.png");
	f->draw_type = CDT_NODEBOX;
	f->param_type = CPT_LIGHT;
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->is_ground_content = true;
	f->damage_per_second = 2;
	f->solidness = 0; // drawn separately, makes no faces
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/4;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->setNodeBox(core::aabbox3d<f32>(
		-0.4375*BS,
		-0.5*BS,
		-0.4375*BS,
		0.4375*BS,
		0.5*BS,
		0.4375*BS
	));
	f->addNodeBox(core::aabbox3d<f32>(
		-0.5*BS,
		0.3125*BS,
		-0.5*BS,
		-0.4375*BS,
		0.375*BS,
		-0.4375*BS
	));
	f->addNodeBox(core::aabbox3d<f32>(
		0.4375*BS,
		0.125*BS,
		0.4375*BS,
		0.5*BS,
		0.1875*BS,
		0.5*BS
	));
	f->addNodeBox(core::aabbox3d<f32>(
		0.4375*BS,
		-0.1875*BS,
		-0.5*BS,
		0.5*BS,
		-0.125*BS,
		-0.4375*BS
	));
	f->addNodeBox(core::aabbox3d<f32>(
		-0.5*BS,
		-0.375*BS,
		0.4375*BS,
		-0.4375*BS,
		-0.3125*BS,
		0.5*BS
	));
	f->setInventoryTextureCube("cactus_top.png", "cactus_side.png", "cactus_side.png");
	f->type = CMT_WOOD;
	f->hardness = 0.75;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_PAPYRUS;
	f = &content_features(i);
	f->description = std::string("Papyrus");
	f->setInventoryTexture("papyrus.png");
	f->setAllTextures("papyrus.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->is_ground_content = true;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->type = CMT_WOOD;
	f->hardness = 0.25;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_SAPLING;
	f = &content_features(i);
	f->description = std::string("Sapling");
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->setAllTextures("sapling.png");
	f->setInventoryTexture("sapling.png");
	f->setAllTextureFlags(0);
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->light_propagates = true;
	f->air_equivalent = false;
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 1;
	f->type = CMT_WOOD;
	f->hardness = 0.0;
	lists::add("creative",i);

	i = CONTENT_APPLE_SAPLING;
	f = &content_features(i);
	f->description = std::string("Apple Tree Sapling");
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->setAllTextures("apple_sapling.png");
	f->setInventoryTexture("apple_sapling.png");
	f->setAllTextureFlags(0);
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->light_propagates = true;
	f->air_equivalent = false;
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 1;
	f->type = CMT_WOOD;
	f->hardness = 0.0;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_JUNGLESAPLING;
	f = &content_features(i);
	f->description = std::string("Jungle Sapling");
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->setAllTextures("junglesapling.png");
	f->setInventoryTexture("junglesapling.png");
	f->setAllTextureFlags(0);
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->light_propagates = true;
	f->air_equivalent = false;
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 1;
	f->type = CMT_WOOD;
	f->hardness = 0.0;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_CONIFER_SAPLING;
	f = &content_features(i);
	f->description = std::string("Conifer Sapling");
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->setAllTextures("conifer_sapling.png");
	f->setInventoryTexture("conifer_sapling.png");
	f->setAllTextureFlags(0);
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->light_propagates = true;
	f->air_equivalent = false;
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 1;
	f->type = CMT_WOOD;
	f->hardness = 0.0;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_APPLE;
	f = &content_features(i);
	f->description = std::string("Apple");
	f->setInventoryTexture("apple.png");
	f->setAllTextures("apple.png");
	f->setAllTextureFlags(0);
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE_SML;
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->air_equivalent = true;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 1;
	f->dug_item = std::string("CraftItem apple 1");
	f->type = CMT_WOOD;
	f->hardness = 0.0;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	// plants
	i = CONTENT_WILDGRASS_SHORT;
	f = &content_features(i);
	f->description = std::string("Wild Grass");
	f->setInventoryTexture("wildgrass_short.png");
	f->setAllTextures("wildgrass_short.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->air_equivalent = false; // grass grows underneath
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	//f->pointable = false;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;

	i = CONTENT_WILDGRASS_LONG;
	f = &content_features(i);
	f->description = std::string("Wild Grass");
	f->setInventoryTexture("wildgrass_long.png");
	f->setAllTextures("wildgrass_long.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->air_equivalent = false; // grass grows underneath
	f->dug_item = std::string("MaterialItem2 ")+itos(CONTENT_WILDGRASS_SHORT)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;

	i = CONTENT_DEADGRASS;
	f = &content_features(i);
	f->description = std::string("Dead Grass");
	f->setInventoryTexture("deadgrass.png");
	f->setAllTextures("deadgrass.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;

	i = CONTENT_FLOWER_STEM;
	f = &content_features(i);
	f->description = std::string("Flower Stem");
	f->setInventoryTexture("flower_stem.png");
	f->setAllTextures("flower_stem.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;

	i = CONTENT_FLOWER_ROSE;
	f = &content_features(i);
	f->description = std::string("Rose");
	f->setInventoryTexture("flower_rose.png");
	f->setAllTextures("flower_rose.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_FLOWER_DAFFODIL;
	f = &content_features(i);
	f->description = std::string("Daffodil");
	f->setInventoryTexture("flower_daffodil.png");
	f->setAllTextures("flower_daffodil.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);

	i = CONTENT_FLOWER_TULIP;
	f = &content_features(i);
	f->description = std::string("Tulip");
	f->setInventoryTexture("flower_tulip.png");
	f->setAllTextures("flower_tulip.png");
	f->setAllTextureFlags(0);
	f->light_propagates = true;
	f->sunlight_propagates = true;
	f->param_type = CPT_LIGHT;
	f->draw_type = CDT_PLANTLIKE;
	f->dug_item = std::string("MaterialItem2 ")+itos(i)+" 1";
	f->solidness = 0; // drawn separately, makes no faces
	f->walkable = false;
	f->material_pointable = false;
	f->flammable = 1; // can be replaced by fire if the node under it is set on fire
	f->fuel_time = 30/32;
	f->buildable_to = true;
	f->type = CMT_WOOD;
	f->hardness = 0.10;
	f->pressure_type = CST_CRUSHABLE;
	lists::add("creative",i);
}