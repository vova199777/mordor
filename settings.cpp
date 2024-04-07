#include "main.h"
#include "settings.h"
#include "vendor/inih/cpp/INIReader.h"

CSettings::CSettings()
{
	Log("Loading settings..");	

	char buff[0x7F];
	sprintf(buff, BOEV("%sSAMP/settings.ini"), g_pszStorage);

	INIReader reader(buff);

	if(reader.ParseError() < 0)
	{
		Log("Error: can't load %s", buff);
		std::terminate();
		return;
	}

	// Client
	
	const char* client = BOEV("client");
	const char* ldebug = BOEV("debug");
	const char* gui = BOEV("gui");
	const char* Font = BOEV("Font");
	
	const char* lname = BOEV("name");
	const char* lpass = BOEV("password");
	const char* fpslimit = BOEV("fpslimit");
	const char* debug_fps = BOEV("debug_fps");
	const char* setfont = BOEV("Arial.ttf");
	const char* fontsize = BOEV("FontSize");
	const char* fontoutline = BOEV("FontOutline");
	const char* serverid = BOEV("serverid");
	
	const char* b_iCutout = BOEV("cutout");
	const char* b_iAndroidKeyboard = BOEV("androidKeyboard");
	const char* b_iAndroidDialog = BOEV("androidDialog");
	const char* ChatPosX = BOEV("ChatPosX");
	const char* ChatPosY = BOEV("ChatPosY");
	const char* ChatSizeX = BOEV("ChatSizeX");
	const char* ChatSizeY = BOEV("ChatSizeY");
	
	const char* ChatMaxMessages = BOEV("ChatMaxMessages");
	
	const char* HealthBarWidth = BOEV("HealthBarWidth");
	const char* HealthBarHeight = BOEV("HealthBarHeight");
	
	const char* Speedo = BOEV("speedo");
	const char* Ch = BOEV("ch");
	const char* Objdebug = BOEV("objdebug");
	
	size_t length = 0;
	sprintf(buff, BOEV("__android_%d%d"), rand() % 1000, rand() % 1000);
	length = reader.Get(client, lname, buff).copy(m_Settings.szNickName, MAX_PLAYER_NAME);
	m_Settings.szNickName[length] = '\0';
	length = reader.Get(client, lpass, "").copy(m_Settings.szPassword, MAX_SETTINGS_STRING);
	m_Settings.szPassword[length] = '\0';
	m_Settings.szserverid = reader.GetInteger(client, serverid, 0);
	
	//limit fps
	m_Settings.fps = reader.GetInteger(client, fpslimit, 30);
	m_Settings.ch = reader.GetBoolean(client, Ch, false);
	m_Settings.objdebug = reader.GetBoolean(client, Objdebug, false);
	//toggle debug fps
	m_Settings.DEBUGFPS = reader.GetBoolean(ldebug, debug_fps, false);

	// gui
	length = reader.Get(gui, Font, setfont).copy(m_Settings.szFont, 35);
	m_Settings.szFont[length] = '\0';
	m_Settings.fFontSize = reader.GetReal(gui, fontsize, 37.0f);
	m_Settings.iFontOutline = reader.GetInteger(gui, fontoutline, 1);
	m_Settings.speedo = reader.GetBoolean(gui, Speedo, false);
	
	// chat
	m_Settings.iCutout = reader.GetBoolean(gui, b_iCutout, false);
	m_Settings.iAndroidKeyboard = reader.GetBoolean(gui, b_iAndroidKeyboard, true);
	m_Settings.iAndroidDialog = reader.GetBoolean(gui, b_iAndroidDialog, true);
	m_Settings.fChatPosX = reader.GetReal(gui, ChatPosX, 325.0f);
	m_Settings.fChatPosY = reader.GetReal(gui, ChatPosY, 25.0f);
	m_Settings.fChatSizeX = reader.GetReal(gui, ChatSizeX, 1150.0f);
	m_Settings.fChatSizeY = reader.GetReal(gui, ChatSizeY, 220.0f);
	m_Settings.iChatMaxMessages = reader.GetInteger(gui, ChatMaxMessages, 6);
	
	// nametags
	m_Settings.fHealthBarWidth = reader.GetReal(gui, HealthBarWidth, 100.0f);
	m_Settings.fHealthBarHeight = reader.GetReal(gui, HealthBarHeight, 10.0f);
}