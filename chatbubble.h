#pragma once

#include "game/common.h"
#include "vendor/imgui/imgui.h"

#define MAX_BUBBLES 4096

class CChatBubble
{
public:
	CChatBubble();
	~CChatBubble();

	void ResetBubble(PLAYERID playerId);
	void SetBubbleInfo(PLAYERID playerId, char* szInfo, uint32_t color, uint32_t duration, uint8_t length, float distance);

	char* GetBubbleMessage(PLAYERID playerId) { return fMessage[playerId]; }
	uint32_t GetBubbleColor(PLAYERID playerId) { return fColor[playerId]; }
	uint32_t GetBubbleDuration(PLAYERID playerId) { return fDuration[playerId]; }

	void DrawAll(ImVec2 vecPos, PLAYERID playerId);

public:
	bool 		fState[MAX_PLAYERS*4];
	char* 		fMessage[MAX_PLAYERS*4];
	uint32_t 	fColor[MAX_PLAYERS*4];
	uint32_t 	fDuration[MAX_PLAYERS*4];
	uint32_t 	dwLastBubbleTick[MAX_PLAYERS*4];
	float		fDistance[MAX_PLAYERS*4];
};