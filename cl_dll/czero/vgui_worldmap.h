
#pragma once

#include <VGUI_Panel.h>
#include <VGUI_Label.h>
#include <VGUI_Button.h>
#include <VGUI_BitmapTGA.h>

class CMenuPanel;

//==============================================================================
class UIWorldMap : public CMenuPanel
{
private:
	vgui::BitmapTGA *_worldmap1;
	vgui::BitmapTGA *_worldmap2;

public:
	UIWorldMap(int iTrans, bool iRemoveMe, int x, int y, int wide, int tall);
	void Initialize();

	virtual void paintBackground();
};
