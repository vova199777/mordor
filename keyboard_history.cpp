#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "keyboard_history.h"
#include "chatwindow.h"
#include "keyboard.h"

#include "net/netgame.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CKeyBoard *pKeyBoard;

CKeyboardHistory::CKeyboardHistory()
{	
	m_bEnabled = false;
	get_now_pos = -1;
	get_time_use = -1;
}

void CKeyboardHistory::Render()
{
	if(!m_bEnabled) return;
	
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::GetOverlayDrawList()->AddRectFilled(	ImVec2(io.DisplaySize.x/1.02,io.DisplaySize.y/4.96), ImVec2(io.DisplaySize.x/1.085,io.DisplaySize.y/3.27), 0xB0000000);
	
	if((io.MousePos.x > io.DisplaySize.x/1.085 && io.MousePos.x < io.DisplaySize.x/1.02) && (io.MousePos.y > io.DisplaySize.y/4.96 && io.MousePos.y < io.DisplaySize.y/3.27))
	{
		if (ImGui::IsMouseClicked(0))
		{
			ImGui::GetOverlayDrawList()->AddRectFilled(	ImVec2(io.DisplaySize.x/1.02,io.DisplaySize.y/4.96), ImVec2(io.DisplaySize.x/1.085,io.DisplaySize.y/3.27), 0xFFEAD811);
			if(get_now_pos > 0)
			{
				get_now_pos--;
				pKeyBoard->InsertText(OrigHistory[get_now_pos]);
				get_time_use = GetTickCount();
			}
		}
	}
	
	
	//вверх
	ImGui::GetOverlayDrawList()->AddTriangleFilled(
        ImVec2(io.DisplaySize.x/1.0525, io.DisplaySize.y/4.5), //верхняя вершина
        ImVec2(io.DisplaySize.x/1.03, io.DisplaySize.y/3.4), //левая нижняя
        ImVec2(io.DisplaySize.x/1.075, io.DisplaySize.y/3.4), //правая нижняя
        0xFF8A8886);
		

	ImGui::GetOverlayDrawList()->AddRectFilled(	ImVec2(io.DisplaySize.x/1.02,io.DisplaySize.y/2.32), ImVec2(io.DisplaySize.x/1.085,io.DisplaySize.y/3.06), 0xB0000000);
	
	if((io.MousePos.x > io.DisplaySize.x/1.085 && io.MousePos.x < io.DisplaySize.x/1.02) && (io.MousePos.y > io.DisplaySize.y/3.06 && io.MousePos.y < io.DisplaySize.y/2.32))
	{
		if (ImGui::IsMouseClicked(0))
		{
			ImGui::GetOverlayDrawList()->AddRectFilled(	ImVec2(io.DisplaySize.x/1.02,io.DisplaySize.y/2.32), ImVec2(io.DisplaySize.x/1.085,io.DisplaySize.y/3.06), 0xFFEAD811);
			if(get_now_pos >= -1 && get_now_pos <= 8)
			{
				get_now_pos++;
				pKeyBoard->InsertText(OrigHistory[get_now_pos]);
				get_time_use = GetTickCount();
			}
		}
	}
	
	
	//вниз
    ImGui::GetOverlayDrawList()->AddTriangleFilled(
        ImVec2(io.DisplaySize.x/1.0525, io.DisplaySize.y/2.4), //нижняя вершина
        ImVec2(io.DisplaySize.x/1.03, io.DisplaySize.y/2.88), //левая верхняя
        ImVec2(io.DisplaySize.x/1.075, io.DisplaySize.y/2.88), //правая верхняя
        0xFF8A8886);
		
	//ImGui::Begin("test",nullptr,ImVec2(),-1.0f,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	
	//ImGui::SetWindowPos( ImVec2(io.DisplaySize.x/1.085, io.DisplaySize.y/2.32));
	
	//ImGui::End();
		
}