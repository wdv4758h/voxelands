/*
Minetest-c55
Copyright (C) 2010 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "common_irrlicht.h"
#include "inventory.h"
#include "utility.h"
#include "modalMenu.h"
#include "keycode.h"
#include "guiFormSpecMenu.h"
#include <IGUICheckBox.h>
#include <IGUIEditBox.h>
#include <IGUIButton.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>
#include "constants.h"
#include "log.h"
#include "tile.h" // ITextureSource
#include "path.h"

#include "gettext.h"

void drawInventoryItem(video::IVideoDriver *driver,
		gui::IGUIFont *font,
		InventoryItem *item,
		core::rect<s32> rect,
		const core::rect<s32> *clip)
{
	if(item == NULL)
		return;

	video::ITexture *texture = NULL;
	texture = item->getImage();

	if(texture != NULL)
	{
		const video::SColor color(255,255,255,255);
		const video::SColor colors[] = {color,color,color,color};
		driver->draw2DImage(texture, rect,
			core::rect<s32>(core::position2d<s32>(0,0),
			core::dimension2di(texture->getOriginalSize())),
			clip, colors, true);
	}
	else
	{
		video::SColor bgcolor(255,50,50,128);
		driver->draw2DRectangle(bgcolor, rect, clip);
	}

	if(font != NULL)
	{
		std::string text = item->getText();
		if(font && text != "")
		{
			v2u32 dim = font->getDimension(narrow_to_wide(text).c_str());
			v2s32 sdim(dim.X,dim.Y);

			core::rect<s32> rect2(
				/*rect.UpperLeftCorner,
				core::dimension2d<u32>(rect.getWidth(), 15)*/
				rect.LowerRightCorner - sdim,
				sdim
			);

			video::SColor bgcolor(128,0,0,0);
			driver->draw2DRectangle(bgcolor, rect2, clip);

			font->draw(text.c_str(), rect2,
					video::SColor(255,255,255,255), false, false,
					clip);
		}
	}
}

/*
	GUIFormSpecMenu
*/

GUIFormSpecMenu::GUIFormSpecMenu(gui::IGUIEnvironment* env,
		gui::IGUIElement* parent, s32 id,
		IMenuManager *menumgr,
		InventoryManager *invmgr
):
	GUIModalMenu(env, parent, id, menumgr),
	m_invmgr(invmgr),
	m_form_src(NULL),
	m_text_dst(NULL),
	m_selected_item(NULL),
	m_selected_amount(0),
	m_selected_dragging(false),
	m_tooltip_element(NULL)
{
}

GUIFormSpecMenu::~GUIFormSpecMenu()
{
	removeChildren();

	delete m_selected_item;
	delete m_form_src;
	delete m_text_dst;
}

void GUIFormSpecMenu::removeChildren()
{
	const core::list<gui::IGUIElement*> &children = getChildren();
	core::list<gui::IGUIElement*> children_copy;
	for(core::list<gui::IGUIElement*>::ConstIterator
			i = children.begin(); i != children.end(); i++)
	{
		children_copy.push_back(*i);
	}
	for(core::list<gui::IGUIElement*>::Iterator
			i = children_copy.begin();
			i != children_copy.end(); i++)
	{
		(*i)->remove();
	}
	/*{
		gui::IGUIElement *e = getElementFromId(256);
		if(e != NULL)
			e->remove();
	}*/
	if(m_tooltip_element)
	{
		m_tooltip_element->remove();
		m_tooltip_element = NULL;
	}
}

void GUIFormSpecMenu::regenerateGui(v2u32 screensize)
{
	// Remove children
	removeChildren();

	v2s32 size(100,100);
	s32 helptext_h = 15;
	core::rect<s32> rect;

	// Base position of contents of form
	v2s32 basepos = getBasePos();
	// State of basepos, 0 = not set, 1= set by formspec, 2 = set by size[] element
	// Used to adjust form size automatically if needed
	// A proceed button is added if there is no size[] element
	int bp_set = 0;

	/* Convert m_init_draw_spec to m_inventorylists */

	m_inventorylists.clear();
	m_images.clear();
	m_fields.clear();

	Strfnd f(m_formspec_string);
	while(f.atend() == false)
	{
		std::string type = trim(f.next("["));
		if(type == "invsize" || type == "size")
		{
			v2f invsize;
			invsize.X = stof(f.next(","));
			if(type == "size")
			{
				invsize.Y = stof(f.next("]"));
			}
			else{
				invsize.Y = stof(f.next(";"));
				errorstream<<"WARNING: invsize is deprecated, use size"<<std::endl;
				f.next("]");
			}
			infostream<<"size ("<<invsize.X<<","<<invsize.Y<<")"<<std::endl;

			padding = v2s32(screensize.Y/40, screensize.Y/40);
			spacing = v2s32(screensize.Y/12, screensize.Y/13);
			imgsize = v2s32(screensize.Y/15, screensize.Y/15);
			size = v2s32(
				padding.X*2+spacing.X*(invsize.X-1.0)+imgsize.X,
				padding.Y*2+spacing.Y*(invsize.Y-1.0)+imgsize.Y + (helptext_h-5)
			);
			rect = core::rect<s32>(
				screensize.X/2 - size.X/2,
				screensize.Y/2 - size.Y/2,
				screensize.X/2 + size.X/2,
				screensize.Y/2 + size.Y/2
			);
			DesiredRect = rect;
			recalculateAbsolutePosition(false);
			basepos = getBasePos();
			bp_set = 2;
		}
		else if(type == "list")
		{
			std::string name = f.next(";");
			InventoryLocation loc;
			if(name == "context" || name == "current_name")
				loc = m_current_inventory_location;
			else
				loc.deSerialize(name);
			std::string listname = f.next(";");
			v2s32 pos = basepos;
			pos.X += stof(f.next(",")) * (float)spacing.X;
			pos.Y += stof(f.next(";")) * (float)spacing.Y;
			v2s32 geom;
			geom.X = stoi(f.next(","));
			geom.Y = stoi(f.next(";"));
			infostream<<"list inv="<<name<<", listname="<<listname
					<<", pos=("<<pos.X<<","<<pos.Y<<")"
					<<", geom=("<<geom.X<<","<<geom.Y<<")"
					<<std::endl;
			f.next("]");
			if(bp_set != 2)
				errorstream<<"WARNING: invalid use of list without a size[] element"<<std::endl;
			m_inventorylists.push_back(ListDrawSpec(loc, listname, pos, geom));
		}
		else if(type == "image")
		{
			v2s32 pos = basepos;
			pos.X += stof(f.next(",")) * (float)spacing.X;
			pos.Y += stof(f.next(";")) * (float)spacing.Y;
			v2s32 geom;
			geom.X = stof(f.next(",")) * (float)imgsize.X;
			geom.Y = stof(f.next(";")) * (float)imgsize.Y;
			std::string name = f.next("]");
			infostream<<"image name="<<name
					<<", pos=("<<pos.X<<","<<pos.Y<<")"
					<<", geom=("<<geom.X<<","<<geom.Y<<")"
					<<std::endl;
			if(bp_set != 2)
				errorstream<<"WARNING: invalid use of image without a size[] element"<<std::endl;
			m_images.push_back(ImageDrawSpec(name, pos, geom));
		}
		else if(type == "field")
		{
			std::string fname = f.next(";");
			std::string flabel = f.next(";");

			if(fname.find(",") == std::string::npos && flabel.find(",") == std::string::npos)
			{
				if(!bp_set)
				{
					rect = core::rect<s32>(
						screensize.X/2 - 160,
						screensize.Y/2 - 60,
						screensize.X/2 + 160,
						screensize.Y/2 + 60
					);
					DesiredRect = rect;
					recalculateAbsolutePosition(false);
					basepos = getBasePos();
					bp_set = 1;
				}
				else if(bp_set == 2)
					errorstream<<"WARNING: invalid use of unpositioned field in inventory"<<std::endl;

				v2s32 pos = basepos;
				pos.Y = (m_fields.size()+1*50);
				printf("%d\n",pos.Y);
				v2s32 size = DesiredRect.getSize();
				rect = core::rect<s32>(size.X/2-150, pos.Y, (size.X/2-150)+300, pos.Y+30);
			}
			else
			{
				v2s32 pos;
				pos.X = stof(fname.substr(0,fname.find(","))) * (float)spacing.X;
				pos.Y = stof(fname.substr(fname.find(",")+1)) * (float)spacing.Y;
				v2s32 geom;
				geom.X = (stof(flabel.substr(0,flabel.find(","))) * (float)spacing.X)-(spacing.X-imgsize.X);
				pos.Y += (stof(flabel.substr(flabel.find(",")+1)) * (float)imgsize.Y)/2;

				rect = core::rect<s32>(pos.X, pos.Y-15, pos.X+geom.X, pos.Y+15);

				fname = f.next(";");
				flabel = f.next(";");
				if(bp_set != 2)
					errorstream<<"WARNING: invalid use of positioned field without a size[] element"<<std::endl;

			}

			std::string odefault = f.next("]");
			std::string fdefault;

			// fdefault may contain a variable reference, which
			// needs to be resolved from the node metadata
			if(m_form_src)
				fdefault = m_form_src->resolveText(odefault);
			else
				fdefault = odefault;

			FieldSpec spec = FieldSpec(
				narrow_to_wide(fname.c_str()),
				narrow_to_wide(flabel.c_str()),
				narrow_to_wide(fdefault.c_str()),
				258+m_fields.size()
			);

			// three cases: field and no label, label and no field, label and field
			if (flabel == "")
			{
				spec.send = true;
				gui::IGUIElement *e = Environment->addEditBox(spec.fdefault.c_str(), rect, false, this, spec.fid);
				Environment->setFocus(e);

				irr::SEvent evt;
				evt.EventType = EET_KEY_INPUT_EVENT;
				evt.KeyInput.Key = KEY_END;
				evt.KeyInput.PressedDown = true;
				e->OnEvent(evt);
			}
			else if (fname == "")
			{
				// set spec field id to 0, this stops submit searching for a value that isn't there
				Environment->addStaticText(spec.flabel.c_str(), rect, false, true, this, spec.fid);
			}
			else
			{
				spec.send = true;
				gui::IGUIElement *e = Environment->addEditBox(spec.fdefault.c_str(), rect, false, this, spec.fid);
				Environment->setFocus(e);
				rect.UpperLeftCorner.Y -= 15;
				rect.LowerRightCorner.Y -= 15;
				Environment->addStaticText(spec.flabel.c_str(), rect, false, true, this, 0);

				irr::SEvent evt;
				evt.EventType = EET_KEY_INPUT_EVENT;
				evt.KeyInput.Key = KEY_END;
				evt.KeyInput.PressedDown = true;
				e->OnEvent(evt);
			}

			m_fields.push_back(spec);
		}
		else if(type == "label")
		{
			v2s32 pos;
			pos.X = stof(f.next(",")) * (float)spacing.X;
			pos.Y = stof(f.next(";")) * (float)spacing.Y;

			rect = core::rect<s32>(pos.X, pos.Y+((imgsize.Y/2)-15), pos.X+300, pos.Y+((imgsize.Y/2)+15));

			std::string flabel = f.next("]");
			if(bp_set != 2)
				errorstream<<"WARNING: invalid use of label without a size[] element"<<std::endl;

			FieldSpec spec = FieldSpec(
				narrow_to_wide(""),
				narrow_to_wide(flabel.c_str()),
				narrow_to_wide(""),
				258+m_fields.size()
			);
			Environment->addStaticText(spec.flabel.c_str(), rect, false, true, this, spec.fid);
			m_fields.push_back(spec);
		}
		else if(type == "button" || type == "button_exit")
		{
			v2s32 pos;
			pos.X = stof(f.next(",")) * (float)spacing.X;
			pos.Y = stof(f.next(";")) * (float)spacing.Y;
			v2s32 geom;
			geom.X = (stof(f.next(",")) * (float)spacing.X)-(spacing.X-imgsize.X);
			pos.Y += (stof(f.next(";")) * (float)imgsize.Y)/2;

			rect = core::rect<s32>(pos.X, pos.Y-15, pos.X+geom.X, pos.Y+15);

			std::string fname = f.next(";");
			std::string flabel = f.next("]");
			if(bp_set != 2)
				errorstream<<"WARNING: invalid use of button without a size[] element"<<std::endl;

			FieldSpec spec = FieldSpec(
				narrow_to_wide(fname.c_str()),
				narrow_to_wide(flabel.c_str()),
				narrow_to_wide(""),
				258+m_fields.size()
			);
			spec.is_button = true;
			if (type == "button_exit")
				spec.is_exit = true;
			Environment->addButton(rect, this, spec.fid, spec.flabel.c_str());
			m_fields.push_back(spec);
		}
		else if(type == "image_button" || type == "image_button_exit")
		{
			v2s32 pos;
			pos.X = stof(f.next(",")) * (float)spacing.X;
			pos.Y = stof(f.next(";")) * (float)spacing.Y;
			v2s32 geom;
			geom.X = (stof(f.next(",")) * (float)spacing.X)-(spacing.X-imgsize.X);
			geom.Y = (stof(f.next(";")) * (float)spacing.Y)-(spacing.Y-imgsize.Y);

			rect = core::rect<s32>(pos.X, pos.Y, pos.X+geom.X, pos.Y+geom.Y);

			std::string fimage = f.next(";");
			std::string fname = f.next(";");
			std::string flabel = f.next("]");
			if(bp_set != 2)
				errorstream<<"WARNING: invalid use of image_button without a size[] element"<<std::endl;

			FieldSpec spec = FieldSpec(
				narrow_to_wide(fname.c_str()),
				narrow_to_wide(flabel.c_str()),
				narrow_to_wide(fimage.c_str()),
				258+m_fields.size()
			);
			spec.is_button = true;
			if (type == "image_button_exit")
				spec.is_exit = true;

			video::ITexture *texture = Environment->getVideoDriver()->getTexture(getTexturePath(fimage).c_str());
			gui::IGUIButton *e = Environment->addButton(rect, this, spec.fid, spec.flabel.c_str());
			e->setImage(texture);
			e->setPressedImage(texture);
			e->setScaleImage(true);

			m_fields.push_back(spec);
		}
		else
		{
			// Ignore others
			std::string ts = f.next("]");
			infostream<<"Unknown DrawSpec: type="<<type<<", data=\""<<ts<<"\""
					<<std::endl;
		}
	}

	// If there's inventory, put the usage string at the bottom
	if (m_inventorylists.size())
	{
		changeCtype("");
		core::rect<s32> rect(0, 0, size.X-padding.X*2, helptext_h);
		rect = rect + v2s32(size.X/2 - rect.getWidth()/2,
				size.Y-rect.getHeight()-5);
		const wchar_t *text = wgettext("Left click: Move all items, Right click: Move single item");
		Environment->addStaticText(text, rect, false, true, this, 256);
		changeCtype("C");
	}
	// If there's fields, add a Proceed button
	if (m_fields.size() && bp_set != 2)
	{
		// if the size wasn't set by an invsize[] or size[] adjust it now to fit all the fields
		rect = core::rect<s32>(
			screensize.X/2 - 160,
			screensize.Y/2 - 60,
			screensize.X/2 + 160,
			screensize.Y/2 + 50+(m_fields.size()*50)
		);
		DesiredRect = rect;
		recalculateAbsolutePosition(false);
		basepos = getBasePos();

		changeCtype("");
		{
			v2s32 pos = basepos;
			pos.Y = ((m_fields.size()+1)*50);

			v2s32 size = DesiredRect.getSize();
			rect = core::rect<s32>(size.X/2-70, pos.Y, (size.X/2-70)+140, pos.Y+25);
			Environment->addButton(rect, this, 257, wgettext("Write It"));
		}
		changeCtype("C");
	}
	// Add tooltip
	{
		// Note: parent != this so that the tooltip isn't clipped by the menu rectangle
		m_tooltip_element = Environment->addStaticText(L"",core::rect<s32>(0,0,110,18));
		m_tooltip_element->enableOverrideColor(true);
		m_tooltip_element->setBackgroundColor(video::SColor(140,30,30,50));
		m_tooltip_element->setDrawBackground(true);
		m_tooltip_element->setDrawBorder(true);
		m_tooltip_element->setOverrideColor(video::SColor(255,255,255,255));
		m_tooltip_element->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER);
		m_tooltip_element->setWordWrap(false);
	}
}

GUIFormSpecMenu::ItemSpec GUIFormSpecMenu::getItemAtPos(v2s32 p) const
{
	core::rect<s32> imgrect(0,0,imgsize.X,imgsize.Y);

	for(u32 i=0; i<m_inventorylists.size(); i++)
	{
		const ListDrawSpec &s = m_inventorylists[i];

		for(s32 i=0; i<s.geom.X*s.geom.Y; i++)
		{
			s32 x = (i%s.geom.X) * spacing.X;
			s32 y = (i/s.geom.X) * spacing.Y;
			v2s32 p0(x,y);
			core::rect<s32> rect = imgrect + s.pos + p0;
			if(rect.isPointInside(p))
			{
				return ItemSpec(s.inventoryloc, s.listname, i);
			}
		}
	}

	return ItemSpec(InventoryLocation(), "", -1);
}

void GUIFormSpecMenu::drawList(const ListDrawSpec &s, int phase)
{
	video::IVideoDriver* driver = Environment->getVideoDriver();

	// Get font
	gui::IGUIFont *font = NULL;
	gui::IGUISkin* skin = Environment->getSkin();
	if (skin)
		font = skin->getFont();

	Inventory *inv = m_invmgr->getInventory(&s.inventoryloc);
	assert(inv);
	InventoryList *ilist = inv->getList(s.listname);

	core::rect<s32> imgrect(0,0,imgsize.X,imgsize.Y);
	video::SColor bdcolor(245,60,60,80);
	video::SColor hbcolor(240,255,0,0);
	video::SColor bgcolor(240,30,30,50);

	for (s32 i=0; i<s.geom.X*s.geom.Y; i++) {
		s32 x = (i%s.geom.X) * spacing.X;
		s32 y = (i/s.geom.X) * spacing.Y;
		v2s32 p(x,y);
		core::rect<s32> rect = imgrect + s.pos + p;
		InventoryItem *item = NULL;
		if (ilist)
			item = ilist->getItem(i);

		driver->draw2DRectangle(bgcolor, rect, &AbsoluteClippingRect);

		if (m_selected_item != NULL && m_selected_item->listname == s.listname && m_selected_item->i == i) {
			driver->draw2DRectangleOutline(rect, hbcolor);

		}else{
			driver->draw2DRectangleOutline(rect, bdcolor);
		}

		if (item) {
			drawInventoryItem(driver, font, item, rect, &AbsoluteClippingRect);
			if (rect.isPointInside(m_pointer)) {
				std::string name = item->getGuiName();
				if (name != "") {
					m_tooltip_element->setVisible(true);
					this->bringToFront(m_tooltip_element);
					m_tooltip_element->setText(narrow_to_wide(name).c_str());
					s32 tooltip_x = m_pointer.X + 15;
					s32 tooltip_y = m_pointer.Y + 15;
					s32 tooltip_width = m_tooltip_element->getTextWidth() + 14;
					s32 tooltip_height = m_tooltip_element->getTextHeight() + 4;
					m_tooltip_element->setRelativePosition(
						core::rect<s32>(
							core::position2d<s32>(tooltip_x, tooltip_y),
							core::dimension2d<s32>(tooltip_width, tooltip_height)
						)
					);
				}
			}
		}
	}
}

void GUIFormSpecMenu::drawMenu()
{
	if (m_form_src) {
		std::string newform = m_form_src->getForm();
		if (newform != m_formspec_string) {
			m_formspec_string = newform;
			regenerateGui(m_screensize_old);
		}
	}

	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();

	video::SColor bgtcolor(240,50,50,70);
	video::SColor bgbcolor(240,30,30,50);
	driver->draw2DRectangle(AbsoluteRect,bgtcolor, bgtcolor, bgbcolor, bgbcolor, &AbsoluteClippingRect);
	video::SColor bdcolor(245,60,60,80);
	driver->draw2DRectangleOutline(AbsoluteRect, bdcolor);

	m_tooltip_element->setVisible(false);

	/*
		Draw items
		Phase 0: Item slot rectangles
		Phase 1: Item images; prepare tooltip
	*/

	for(u32 i=0; i<m_inventorylists.size(); i++)
	{
		drawList(m_inventorylists[i], 1);
	}

	for(u32 i=0; i<m_images.size(); i++)
	{
		const ImageDrawSpec &spec = m_images[i];
		video::ITexture *texture = Environment->getVideoDriver()->getTexture(getTexturePath(spec.name).c_str());
		// Image size on screen
		core::rect<s32> imgrect(0, 0, spec.geom.X, spec.geom.Y);
		// Image rectangle on screen
		core::rect<s32> rect = imgrect + spec.pos;
		const video::SColor color(255,255,255,255);
		const video::SColor colors[] = {color,color,color,color};
		driver->draw2DImage(texture, rect,
			core::rect<s32>(core::position2d<s32>(0,0),
					core::dimension2di(texture->getOriginalSize())),
			NULL/*&AbsoluteClippingRect*/, colors, true);
	}

	/*
		Call base class
	*/
	gui::IGUIElement::draw();
}

void GUIFormSpecMenu::acceptInput()
{
	if(m_text_dst)
	{
		std::map<std::string, std::string> fields;
		gui::IGUIElement *e;
		for(u32 i=0; i<m_fields.size(); i++)
		{
			const FieldSpec &s = m_fields[i];
			if(s.send)
			{
				if(s.is_button)
				{
					fields[wide_to_narrow(s.fname.c_str())] = wide_to_narrow(s.flabel.c_str());
				}
				else
				{
					e = getElementFromId(s.fid);
					if(e != NULL)
					{
						fields[wide_to_narrow(s.fname.c_str())] = wide_to_narrow(e->getText());
					}
				}
			}
		}
		m_text_dst->gotText(fields);
	}
}

bool GUIFormSpecMenu::OnEvent(const SEvent& event)
{
	if(event.EventType==EET_KEY_INPUT_EVENT)
	{
		KeyPress kp(event.KeyInput);
		if (event.KeyInput.PressedDown && (kp == EscapeKey ||
			kp == getKeySetting("keymap_inventory")))
		{
			m_tooltip_element->setVisible(false);
			quitMenu();
			return true;
		}
		if(event.KeyInput.Key==KEY_RETURN && event.KeyInput.PressedDown)
		{
			m_tooltip_element->setVisible(false);
			acceptInput();
			quitMenu();
			return true;
		}
	}
	if(event.EventType==EET_MOUSE_INPUT_EVENT) {
		char amount = -1;

		if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			amount = 0;
		else if(event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
			amount = 1;
		else if(event.MouseInput.Event == EMIE_MMOUSE_PRESSED_DOWN)
			amount = 10;

		m_pointer.X = event.MouseInput.X;
		m_pointer.Y = event.MouseInput.Y;

		if(amount >= 0)
		{
			v2s32 p(event.MouseInput.X, event.MouseInput.Y);
			//infostream<<"Mouse down at p=("<<p.X<<","<<p.Y<<")"<<std::endl;
			ItemSpec s = getItemAtPos(p);
			if(s.isValid())
			{
				if(m_selected_item)
				{
					Inventory *inv_from = m_invmgr->getInventory(&m_selected_item->inventoryloc);
					Inventory *inv_to = m_invmgr->getInventory(&s.inventoryloc);
					assert(inv_from);
					assert(inv_to);
					InventoryList *list_from =
							inv_from->getList(m_selected_item->listname);
					InventoryList *list_to =
							inv_to->getList(s.listname);
					if(list_from == NULL)
						infostream<<"from list doesn't exist"<<std::endl;
					if(list_to == NULL)
						infostream<<"to list doesn't exist"<<std::endl;
					// Indicates whether source slot completely empties
					bool source_empties = false;
					if(list_from && list_to
							&& list_from->getItem(m_selected_item->i) != NULL)
					{
						infostream<<"Handing IACTION_MOVE to manager"<<std::endl;
						IMoveAction *a = new IMoveAction();
						a->count = amount;
						a->from_inv = m_selected_item->inventoryloc.getName();
						a->from_list = m_selected_item->listname;
						a->from_i = m_selected_item->i;
						a->to_inv = s.inventoryloc.getName();
						a->to_list = s.listname;
						a->to_i = s.i;
						//ispec.actions->push_back(a);
						m_invmgr->inventoryAction(a);

						if(list_from->getItem(m_selected_item->i)->getCount()==1)
							source_empties = true;
					}
					// Remove selection if target was left-clicked or source
					// slot was emptied
					if(amount == 0 || source_empties)
					{
						delete m_selected_item;
						m_selected_item = NULL;
					}
				}
				else
				{
					/*
						Select if non-NULL
					*/
					Inventory *inv = m_invmgr->getInventory(&s.inventoryloc);
					assert(inv);
					InventoryList *list = inv->getList(s.listname);
					if(list->getItem(s.i) != NULL)
					{
						m_selected_item = new ItemSpec(s);
					}
				}
			}
			else
			{
				if(m_selected_item)
				{
					delete m_selected_item;
					m_selected_item = NULL;
				}
			}
		}
	}
	if(event.EventType==EET_GUI_EVENT)
	{
		if(event.GUIEvent.EventType==gui::EGET_ELEMENT_FOCUS_LOST
				&& isVisible())
		{
			if(!canTakeFocus(event.GUIEvent.Element))
			{
				infostream<<"GUIFormSpecMenu: Not allowing focus change."
						<<std::endl;
				// Returning true disables focus change
				return true;
			}
		}
		if(event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED)
		{
			switch(event.GUIEvent.Caller->getID())
			{
			case 257:
				acceptInput();
				quitMenu();
				// quitMenu deallocates menu
				return true;
			}
			// find the element that was clicked
			for(u32 i=0; i<m_fields.size(); i++)
			{
				FieldSpec &s = m_fields[i];
				// if its a button, set the send field so
				// receiveFields knows which button was pressed
				if (s.is_button && s.fid == event.GUIEvent.Caller->getID())
				{
					s.send = true;
					acceptInput();
					if (s.is_exit) {
						quitMenu();
					}else{
						s.send = false;
					}
					return true;
				}
			}
		}
		if(event.GUIEvent.EventType==gui::EGET_EDITBOX_ENTER)
		{
			if(event.GUIEvent.Caller->getID() > 257)
			{
				acceptInput();
				quitMenu();
				// quitMenu deallocates menu
				return true;
			}
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}
