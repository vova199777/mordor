#include "main.h"
#include "chatbubble.h"
//#include "timer.hpp"
#include "gui/gui.h"

extern CGUI *pGUI;

CChatBubble::CChatBubble()
{
	for(int i = 0; i < 1004; i++)
	{
		fMessage[i] = nullptr; 
		fState[i] = false;
		fColor[i] = 0xFF;
		fDuration[i] = 0xFF;
		dwLastBubbleTick[i] = 0xFF;
		fDistance[i] = 0;
	}
}

CChatBubble::~CChatBubble()
{

}

void CChatBubble::ResetBubble(PLAYERID playerId)
{
	lastfunc("bb1");
	if(fState[playerId])
	{
		fState[playerId] = false;
		fMessage[playerId] = nullptr;
		fDuration[playerId] = 0;
		fColor[playerId] = 0;
	}
}

void CChatBubble::SetBubbleInfo(PLAYERID playerId, char* szInfo, uint32_t color, uint32_t duration, uint8_t length, float distance) 
{ 
	lastfunc("bb2");
	if(!szInfo) return;
	
	if(fMessage[playerId]) 
	{ 
		free(fMessage[playerId]);
		fMessage[playerId] = nullptr; 
	} 
	
	fMessage[playerId] = (char*)malloc((length * 3) + 1); 
	if(!fMessage[playerId])
	{
		Log("BubbleChat: can't allocate memory");
		return;
	}
	
	cp1251_to_utf8(fMessage[playerId], szInfo); 

	fColor[playerId] = color;
	fDuration[playerId] = duration;
	fState[playerId] = true;
	fDistance[playerId] = distance;

	dwLastBubbleTick[playerId] = GetTickCount();
}

ImVec2 aCalcTextSizeWithoutTags(char* szStr)
{
	if(szStr == nullptr) return ImVec2(0,0);
	if(!szStr) return ImVec2(0, 0);

	char szNonColored[2048+1];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;

	return ImGui::CalcTextSize(szNonColored);
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
void aTextWithColors(ImVec2 pos, ImColor col, const char* szStr)
{
	char tempStr[4096];

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				pGUI->RenderText(vecPos, col, true, textStart, textCur);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
				pushedColorStyle = false;

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			pGUI->RenderText(vecPos, col, true, textStart, textCur);
			vecPos.x = pos.x;
			vecPos.y += pGUI->GetFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
	{
		pGUI->RenderText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
		vecPos.y += pGUI->GetFontSize();
}

void CChatBubble::DrawAll(ImVec2 vecPos, PLAYERID playerId)
{
	lastfunc("bb3");
	if(	fMessage[playerId] == nullptr or !strlen(fMessage[playerId]))
	{
		return;
	}	
	if(!fState[playerId]) 
	{
		return;
	}
	
	// размещаем по центру
	vecPos.x -= aCalcTextSizeWithoutTags(fMessage[playerId]).x / 2;
	
	// Цветокоррекция
	if(GetBubbleColor(playerId) == 0xFFFFFFFF or GetBubbleColor(playerId) == -1) 
	{
		fColor[playerId] = 0xA0C2DEFF; // Устанавливам стандартный цвет (0.3.7)
	}
	
	// Рендер
	aTextWithColors(vecPos, __builtin_bswap32(GetBubbleColor(playerId) | (0x000000FF)), fMessage[playerId]);

	// Задержка
	if(GetTickCount() - dwLastBubbleTick[playerId] > GetBubbleDuration(playerId))
	{ 
		// Время прошло
		ResetBubble(playerId); // Убираем ChatBubble игрока
	}
}
