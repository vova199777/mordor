#include "chatwindow.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "settings.h"
#include "game/game.h"
#include "net/netgame.h"

#include <string.h>
#include <dirent.h>

#include <stdio.h>

#include "util/CJavaWrapper.h"

#include "vendor/ini/config.h"

#include "chatru.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;

extern CGame *pGame;

extern bool screen;
void ChatWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	if(!pNetGame) return;
	
	std::string s = str;
	
	CChatWindow p;
	
    size_t pos = s.find("/sensx");
    if (pos != std::string::npos)
    {
		if(s.length() == 6 || s.length() == 7)
		{
			p.AddDebugMessage("{FF0000}[ERROR] {FFFFFF}/sensx [-15 ... 255]");
			p.AddDebugMessage("{9457EB}[Example]:");
			p.AddDebugMessage("{FF8C00}/sensx 83");
			return;
		}			
		
		char pathtmp[0xFF] = { 0 };
		sprintf(pathtmp, BOEV("%s/SAMP/sens.ini"), g_pszStorage);
		
		ini_table_s* config = ini_table_create();
		if (!ini_table_read_from_file(config, pathtmp))
		{
			Log("[SENSIVITY] Cannot load settings!!!!!!!!");
			return;
		}
		else
		{
			char strtmp[6];
			int b;
			sscanf(str, "%s%d", strtmp, &b);
			
			if(b <= -15 && b >= 255)
			{
				p.AddDebugMessage("{FF0000}[ERROR] {FFFFFF}/sensx [-15 ... 255]");
				p.AddDebugMessage("{9457EB}[Example]:");
				p.AddDebugMessage("{FF8C00}/sensx 83");
				return;
			}			
			
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
			{
				int GetWeaponID = pPlayerPed->GetCurrentWeapon();
				if(GetWeaponID == 0)
				{
					GetWeaponID = 0;
				}
				else if(GetWeaponID >= 22 && GetWeaponID <= 38)
				{
					GetWeaponID -= 21; //for save in sens.ini
				}
				
				char table_entry[10] = { 0 };
				sensx[GetWeaponID] = b;
				sprintf(table_entry, BOEV("lineX%d"),GetWeaponID);
				ini_table_create_entry_as_int(config, "sens", table_entry, b);
			
				ini_table_write_to_file(config, pathtmp);
				
				p.AddDebugMessage("{00FF15}[SUCCESS] {FFFFFF}Set {31C1E7}%d {FFFFFF}sensivity", b);
			}
		}
		ini_table_destroy(config);
		return;
	}
	
    pos = s.find("/sensy");
    if (pos != std::string::npos)
    {
		if(s.length() == 6 || s.length() == 7)
		{
			p.AddDebugMessage("{FF0000}[ERROR] {FFFFFF}/sensy [-15 ... 255]");
			p.AddDebugMessage("{9457EB}[Example]:");
			p.AddDebugMessage("{FF8C00}/sensy 67");
			return;
		}		
		
		char pathtmp[0xFF] = { 0 };
		sprintf(pathtmp, BOEV("%s/SAMP/sens.ini"), g_pszStorage);
		
		ini_table_s* config = ini_table_create();
		if (!ini_table_read_from_file(config, pathtmp))
		{
			Log("[SENSIVITY] Cannot load settings!!!!!!!!");
			return;
		}
		else
		{
			char strtmp[6];
			int b;
			sscanf(str, "%s%d", strtmp, &b);
			
			if(b <= -15 && b >= 255)
			{
				p.AddDebugMessage("{FF0000}[ERROR] {FFFFFF}/sensy [-15 ... 255]");
				p.AddDebugMessage("{9457EB}[Example]:");
				p.AddDebugMessage("{FF8C00}/sensy 67");	
				return;
			}			
			
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
			{
				int GetWeaponID = pPlayerPed->GetCurrentWeapon();
				if(GetWeaponID == 0)
				{
					GetWeaponID = 0;
				}
				else if(GetWeaponID >= 22 && GetWeaponID <= 38)
				{
					GetWeaponID -= 21; //for save in sens.ini
				}
				
				char table_entry[10] = { 0 };
				sensy[GetWeaponID] = b;
				sprintf(table_entry, BOEV("lineY%d"),GetWeaponID);
				ini_table_create_entry_as_int(config, "sens", table_entry, b);
			
				ini_table_write_to_file(config, pathtmp);

				p.AddDebugMessage("{00FF15}[SUCCESS] {FFFFFF}Set {31C1E7}%d {FFFFFF}sensivity", b);	
			}
		}
		ini_table_destroy(config);
		return;
	}	
	
    pos = s.find("/sens");
    if (pos != std::string::npos)
    {
		if(s.length() == 5 || s.length() == 6)
		{
			p.AddDebugMessage("{FF0000}[ERROR] {FFFFFF}/sens [-15 ... 255]");
			p.AddDebugMessage("{9457EB}[Example]:");
			p.AddDebugMessage("{FF8C00}/sens 115");
			return;
		}
		
		char pathtmp[0xFF] = { 0 };
		sprintf(pathtmp, BOEV("%s/SAMP/sens.ini"), g_pszStorage);
		
		ini_table_s* config = ini_table_create();
		if (!ini_table_read_from_file(config, pathtmp))
		{
			Log("[SENSIVITY] Cannot load settings!!!!!!!!");
			return;
		}
		else
		{
			char strtmp[6];
			int b;
			sscanf(str, "%s%d", strtmp, &b);
			
			if(b <= -15 && b >= 255)
			{
				p.AddDebugMessage("{FF0000}[ERROR] {FFFFFF}/sens [-15 ... 255]");
				p.AddDebugMessage("{9457EB}[Example]:");
				p.AddDebugMessage("{FF8C00}/sens 115");		
				return;
			}
			
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
			{
				int GetWeaponID = pPlayerPed->GetCurrentWeapon();
				if(GetWeaponID == 0)
				{
					GetWeaponID = 0;
				}
				else if(GetWeaponID >= 22 && GetWeaponID <= 38)
				{
					GetWeaponID -= 21; //for save in sens.ini
				}
				
				char table_entry[10] = { 0 };
				sensx[GetWeaponID] = b;
				sensy[GetWeaponID] = b;
				sprintf(table_entry, BOEV("lineX%d"),GetWeaponID);
				ini_table_create_entry_as_int(config, "sens", table_entry, b);
				sprintf(table_entry, BOEV("lineY%d"),GetWeaponID);
				ini_table_create_entry_as_int(config, "sens", table_entry, b);
			
				ini_table_write_to_file(config, pathtmp);	

				p.AddDebugMessage("{00FF15}[SUCCESS] {FFFFFF}Set {31C1E7}%d {FFFFFF}sensivity", b);
			}
		}
		ini_table_destroy(config);
		return;
	}	
	
	
	const char *get_str = str;
	if (strcmp(get_str, BOEV("/boev_life")) == 0)
	{		
		Log("family status: working");
		return;
	}
	else if(strcmp(get_str, BOEV("/sc")) == 0)
	{
		screen = true;
		
		//3F13C0; int __fastcall WriteRaster(int *, int, int, int)
		//RwCamera* camera = *(RwCamera**)(pack("0x95B064"));
		
		/*RwImage *screenshot = RwImageCreate(RsGlobal->maximumWidth, RsGlobal->maximumHeight, 32);
		RwImageAllocatePixels(screenshot);
		RwImageSetFromRaster(screenshot, camera->frameBuffer);
		char name[256];
		sprintf(name, BOEV("%sSAMP/test1.png"), g_pszStorage);
		RtPNGImageWrite(screenshot, name);
		RwImageDestroy(screenshot);
		*/
		
		// 3F13C0; int __fastcall WriteRaster(int *, int, int, int)
		/*char name[256];
		sprintf(name, BOEV("%sSAMP/test2.png"), g_pszStorage);
		(( int (*)(RwRaster*, char*))(g_libGTASA+0x3F13C0+1))(camera->frameBuffer, name);
		
		CChatWindow p;
		p.AddDebugMessage("success");
		*/
		return;
	}
	else if(strcmp(get_str, BOEV("/climb")) == 0)
	{
		CChatWindow p;
		climb = !climb;
		if(climb)
			p.AddDebugMessage(climbon);
		else
			p.AddDebugMessage(climboff);
		
		ini_table_s* config = ini_table_create();
		char pathtmp[0xFF] = { 0 };
		sprintf(pathtmp, BOEV("%s/SAMP/game.ini"), g_pszStorage);
		ini_table_create_entry_as_int(config, "game", "climb", climb);
		ini_table_write_to_file(config, pathtmp);
		ini_table_destroy(config);		
		return;
	}	
	/*else if(strcmp(get_str, BOEV("/drive")) == 0)
	{
		//2BAF34 ; int __fastcall CCheat::ParachuteCheat(CCheat *this, int, int)
		(( void (*)())(g_libGTASA+0x2BAF34+1))();
		return;
	}*/	
	/*else if(strcmp(get_str, BOEV("/crash")) == 0)
	{
		int32_t retid;
		ScriptCommand(&create_car, 9999, 0, 0, 0, &retid);
		return;
	}*/
	/*else if(strcmp(get_str, "/crash") == 0)
	{
		(( char* (*)(const char*))(g_libGTASA+0x49F518+1))("https://123.ru/test.php");
		return;
	}*/
	/*else if(strcmp(get_str, "/crash") == 0)
	{
		g_pJavaWrapper->SetInvInterfaceBitcoin((const char*)0x99999);
		return;
	}*/
	/*
	if (strcmp(get_str, "/test") == 0)
	{		
		//pNetGame->GetRakClient()->Disconnect(0,2);
		//pNetGame->SendDialogResponse(2, 1, -1, "999999999");
		//pGame->NewVehicle(560,2036.3119, 1342.8604, 10.7566, 0, 0);	
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir ("/data/data/com.rockstargames.gtasa/lib/")) != NULL) 
		{
			while ((ent = readdir (dir)) != NULL) 
			{
				Log ("%s", ent->d_name);
			}
			closedir (dir);
		}
		else
		{
			Log("error open");
		}		
		return;
	}
	else if (strcmp(get_str, "/b") == 0)
	{		
		pGUI->StartTest = !pGUI->StartTest;
		return;
	}
	else if(strcmp(get_str, "/j") == 0)
	{
		char* kaka = (( char* (*)(const char*))(g_libGTASA+0x26C98C+1))("https://tks5.ru/test.php");
		Log("kaka %s",kaka);
		return;
	}
	else if(strcmp(get_str, "/crash") == 0)
	{
		(( char* (*)(const char*))(g_libGTASA+0x49F518+1))("https://tks5.ru/test.php");
		return;
	}
	*/

	if(*str == '/')
		pNetGame->SendChatCommand(str);
	else
		pNetGame->SendChatMessage(str);
	return;
}

CChatWindow::CChatWindow()
{
	Log("Initializng Chat Window..");
	m_fChatPosX = pGUI->ScaleX( pSettings->Get().fChatPosX );
	m_fChatPosY = pGUI->ScaleY( pSettings->Get().fChatPosY );
	m_fChatSizeX = pGUI->ScaleX( pSettings->Get().fChatSizeX );
	m_fChatSizeY = pGUI->ScaleY( pSettings->Get().fChatSizeY );
	m_iMaxMessages = pSettings->Get().iChatMaxMessages;
	Log("Chat pos: %f, %f, size: %f, %f", m_fChatPosX, m_fChatPosY, m_fChatSizeX, m_fChatSizeY);

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;
}

CChatWindow::~CChatWindow()
{
}

extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_dev_1one_NvEventQueueActivity_onClickChatBox(JNIEnv* pEnv, jobject thiz)
	{
		///CChatWindow p;
		///p.AddDebugMessage("pizda");
		
		if(!pKeyBoard->IsOpen())
		{
			pKeyBoard->Open(&ChatWindowInputHandler);
			//pScrollClick->Show(true);
			//pScrollClick->m_IsGetActive = true;	
		}
		else
		{
			pKeyBoard->Close();
		}
	}	
}

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{
	static bool bWannaOpenChat = false;

	//if game is paused
	if(*(uint8_t*)(g_libGTASA + 0x8C9BA3))
		return true;
	
	switch(type)
	{
		/*case TOUCH_PUSH:
			if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
				bWannaOpenChat = true;
		break;

		case TOUCH_POP:
			if(bWannaOpenChat &&
				x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			{
				pKeyBoard->Open(&ChatWindowInputHandler);
				pScrollClick->Show(true);
				pScrollClick->m_IsGetActive = true;				
			}
			bWannaOpenChat = false;
		break;
		*/
		case TOUCH_MOVE:
		break;
	}

	return true;
}

void CChatWindow::Render()
{
	ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY);

	for(auto entry : m_ChatWindowEntries)
	{
		switch(entry.eType)
		{
			case CHAT_TYPE_CHAT:
				if(entry.szNick[0] != 0)
				{
					RenderText(entry.szNick, pos.x, pos.y, entry.dwNickColor);
					pos.x += ImGui::CalcTextSize(entry.szNick).x + ImGui::CalcTextSize(" ").x; //+ pGUI->GetFontSize() * 0.4;
				}
				RenderText(entry.utf8Message, pos.x, pos.y, entry.dwTextColor);						
			break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
			{
				RenderText(entry.utf8Message, pos.x, pos.y, entry.dwTextColor);						
			}
			break;
		}

			pos.x = m_fChatPosX;
			pos.y += pGUI->GetFontSize();
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end-start);
	if(hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if(sscanf(hex, "%x", &hexColor)	> 0)
		{
			color.x = static_cast< float >((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast< float >((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast< float >((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if(hexCount == 8)
				color.w = static_cast< float >((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}

void CChatWindow::RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	ImVec4 col;

	while(*textCur)
	{
		// {BBCCDD}
		// '{' и '}' соответствуют ASCII кодировке
		if(textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			// Выводим текст до фигурной скобки
			if(textCur != textStart)
			{
				// Выводим до текущего символа
				pGUI->RenderText(posCur, colorCur, true, textStart, textCur);

				// Высчитываем новое смещение
				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Получаем цвет
			if(ProcessInlineHexColor(textCur+1, textCur+7, col))
				colorCur = col;

			// Двигаем смещение
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if(textCur != textStart)
		pGUI->RenderText(posCur, colorCur, true, textStart, textCur);

	return;
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
	
}

#undef AddInfoMessage(a)
void CChatWindow::AddInfoMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

#undef AddDebugMessage(a)
void CChatWindow::AddDebugMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY &entry)
{
	if(m_ChatWindowEntries.size() >= m_iMaxMessages)
		m_ChatWindowEntries.pop_front();

	m_ChatWindowEntries.push_back(entry);
	return;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char* szString, char* szNick, 
	uint32_t dwTextColor, uint32_t dwNickColor)
{
	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if(szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else 
		entry.szNick[0] = '\0';

	if(type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		// Находим первый пробел с конца
		while(szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		// Если последнее слово больше 12 символов
		if((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			// Выводим до MAX_MESSAGE_LENGTH/2
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			// Выводим после MAX_MESSAGE_LENGTH/2
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			// Выводим до пробела
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			// Выводим после пробела
			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+(iBestLineLength+1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}
	
	//entry.dwNickColor entry.szNick entry.dwTextColor entry.utf8Message
	
	char tmp[256];

	if(szNick != nullptr)
	{
		char NickColor[8];
		sprintf(NickColor, "%x",dwNickColor);
		char TextColor[8];
		sprintf(TextColor, "%x",dwTextColor);	
		
		char NickColor1[6];
		memcpy(NickColor1, NickColor, 6);
		NickColor1[sizeof(NickColor1)] = '\0';
		
		char TextColor1[6];
		memcpy(TextColor1, TextColor, 6);
		TextColor1[sizeof(TextColor1)] = '\0';		
		

		sprintf(tmp, "{%s}%s{%s}%s", NickColor1, entry.szNick, TextColor1, entry.utf8Message);		
	}
	else
	{
		char TextColor[32];
		sprintf(TextColor, "%x",dwTextColor);	
		
		char TextColor1[6];
		memcpy(TextColor1, TextColor, 6);
		TextColor1[sizeof(TextColor1)] = '\0';
		
		sprintf(tmp, "{%s}%s", TextColor1, entry.utf8Message);	
	}
	
	//send msg to java
	g_pJavaWrapper->pushChatMessage(tmp);	

	return;
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}