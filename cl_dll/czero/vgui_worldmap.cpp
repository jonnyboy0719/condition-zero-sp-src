
#include "VGUI_Font.h"
#include "VGUI_TextImage.h"

#include <VGUI_StackLayout.h>

#include "hud.h"
#include "cl_util.h"
#include "camera.h"
#include "kbutton.h"
#include "const.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_int.h"
#include "vgui_worldmap.h"
#include "vgui_loadtga.h"

using namespace vgui;

#define MOTD_TITLE_X XRES(16)
#define MOTD_TITLE_Y YRES(16)

#define MOTD_WINDOW_X XRES(112)
#define MOTD_WINDOW_Y YRES(80)
#define MOTD_WINDOW_SIZE_X XRES(424)
#define MOTD_WINDOW_SIZE_Y YRES(312)

static CImageLabel *_background = NULL;
extern bool g_iVisibleMouse;

//==============================================================================
char* GetTGAWorldMap( const char* pszName )
{
	static char gd[256];
	sprintf( gd, "%s/gfx/vgui/worldmap/%s.tga", gEngfuncs.pfnGetGameDirectory(), pszName );
	gEngfuncs.Con_Printf("----: GetTGAWorldMap :----\n");
	gEngfuncs.Con_Printf("TGA: %s\n", gd);
	return gd;
}

//==============================================================================
BitmapTGAWorldMap* LoadWorldMapForRes( const char* pImageName, int wide, int tall )
{
	BitmapTGAWorldMap *pWorldTGA = vgui_LoadWorldMapTGA( GetTGAWorldMap( pImageName ) );
	if ( pWorldTGA )
		pWorldTGA->SetMapSzie( wide, tall );
	return pWorldTGA;
}

//==============================================================================
UIWorldMap::UIWorldMap(int iTrans, bool iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	_worldmap1 = LoadWorldMapForRes( "worldmap", wide, tall );
	_worldmap2 = LoadWorldMapForRes( "worldmap2", wide, tall );

	// Create the Health Label
	_background = new CImageLabel( _worldmap1, 0, 0, ScreenWidth, ScreenHeight );
	_background->setParent( this );
	_background->setSize( ScreenWidth, ScreenHeight );

	// BELOW THIS IS JUST DEBUG TEST
	// Get the scheme used for the Titles
	CSchemeManager* pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );

	// color schemes
	int r, g, b, a;

	// Create the title
	Label* pLabel = new Label( "", MOTD_TITLE_X, MOTD_TITLE_Y );
	pLabel->setParent(this);
	pLabel->setFont(pSchemes->getFont(hTitleScheme));
	pLabel->setFont(Scheme::sf_primary1);

	pSchemes->getFgColor(hTitleScheme, r, g, b, a);
	pLabel->setFgColor(r, g, b, a);
	pLabel->setFgColor(Scheme::sc_primary1);
	pSchemes->getBgColor(hTitleScheme, r, g, b, a);
	pLabel->setBgColor(r, g, b, a);
	pLabel->setContentAlignment(vgui::Label::a_west);
	pLabel->setText("WorldMap: POTATO LAND");

	CommandButton* pButton = new CommandButton(CHudTextMessage::BufferedLocaliseTextString("#Menu_OK"), XRES(16), tall - YRES(16) - BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y);
	pButton->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	pButton->setParent(this);
}


//==============================================================================
void UIWorldMap::Initialize()
{
	gEngfuncs.Con_Printf("----: UIWorldMap >> Initialize :----\n");
}


//==============================================================================
void UIWorldMap::paintBackground()
{
	CMenuPanel::paintBackground();

	int wide, tall;
	getParent()->getSize( wide, tall );
	setSize( wide, tall );

	_background->setSize( wide, tall );
}
