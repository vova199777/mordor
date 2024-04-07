#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "showmain.h"
#include "chatwindow.h"
#include "net/netgame.h"

#include "vendor/imgui/imgui_internal.h"

#include "settings.h"

#define RGBA_TO_FLOAT(r,g,b,a) (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

extern CSettings *pSettings;

CShowMain::CShowMain()
{	
	m_bEnabled = false;
	
	settings_main = false;
    show_main = true;

    show_keyboard = false;
    show_dialogs = false;
    show_hud = false;
    show_g = false;
}

void CShowMain::Render()
{
	if(!m_bEnabled) return;
}