#pragma once

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// client
	char szNickName[MAX_PLAYER_NAME+1];
	char szPassword[MAX_SETTINGS_STRING+1];
	int szserverid;
	
	int fps;
	bool DEBUGFPS;
	bool ch;
	bool objdebug;

	// gui
	char szFont[40];
	float fFontSize;
	int iFontOutline;
	float fChatPosX;
	float fChatPosY;
	float fChatSizeX;
	float fChatSizeY;
	int iChatMaxMessages;
	float fHealthBarWidth;
	float fHealthBarHeight;
	bool iCutout;
	bool iAndroidKeyboard;
	bool iAndroidDialog;
	bool speedo;
};

class CSettings
{
public:
	CSettings();
	~CSettings();

	stSettings& Get() { return m_Settings; }
	
private:
	struct stSettings m_Settings;
};