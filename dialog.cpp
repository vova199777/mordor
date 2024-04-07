#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "dialog.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"
#include <stdlib.h>

#include <vector>
#include <sstream>

#include "chatwindow.h"

/*
 _______                   _           ______     _          __                 
|_   __ \                 (_)         |_   _ `.  (_)        [  |                
  | |__) |.---.  _ .--.   __   .--.     | | `. \ __   ,--.   | |  .--.   .--./) 
  |  ___// /__\\[ `.-. | [  | ( (`\]    | |  | |[  | `'_\ :  | |/ .'`\ \/ /'`\; 
 _| |_   | \__., | | | |  | |  `'.'.   _| |_.' / | | // | |, | || \__. |\ \._// 
|_____|   '.__.'[___||__][___][\__) ) |______.' [___]\'-;__/[___]'.__.' .',__`  
                                                                       ( ( __))                                                                                                                                    
*/

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CChatWindow *pChatWindow;

char szDialogInputBuffer[100];
char utf8DialogInputBuffer[100*3];

CDialogWindow::CDialogWindow()
{
	m_bIsActive = false;
	m_putf8Info = nullptr;
	m_putf8Info_orig = nullptr;
	m_pszInfo = nullptr;
	
	m_DetectKeyBoard = false;
	
	time_dbl_test = 500;
	
	save_click_time_dialog = 0;
	save_click_time_dialog_now = 0;
	check_dbl_click_dialog_now = false;
	
	m_StylePassword = false;
	
}

CDialogWindow::~CDialogWindow()
{

}

void CDialogWindow::Show(bool bShow)
{
	//if(pGame) 
	//	pGame->FindPlayerPed()->TogglePlayerControllable(!bShow);

	if(pGame)
	{
		pGame->DisplayHUD(!bShow);
	}	

	if(bShow)
	{
		char* get_pad = (( char* (*)(int))(pack("0x39C8D4")+1))(0);
		*((uint16_t*)get_pad + 0x86) |= 0x20u;
	}
	else
	{
		char* get_pad = (( char* (*)(int))(pack("0x39C8D4")+1))(0);
		*((uint16_t*)get_pad + 0x86) &= 0xFFDF;	
	}

	m_bIsActive = bShow;
}

void CDialogWindow::Clear()
{
	if(m_putf8Info)
	{
		free(m_putf8Info);
		m_putf8Info = nullptr;
	}
	
	if(m_putf8Info_orig)
	{
		free(m_putf8Info_orig);
		m_putf8Info_orig = nullptr;
	}

	if(m_pszInfo)
	{
		free(m_pszInfo);
		m_pszInfo = nullptr;
	}

	m_bIsActive = false;
	m_byteDialogStyle = 0;
	m_wDialogID = -1;
	m_utf8Title[0] = 0;
	m_utf8Button1[0] = 0;
	m_utf8Button2[0] = 0;

	m_fSizeX = -1.0f;
	m_fSizeY = -1.0f;

	memset(szDialogInputBuffer, 0, 100);
	memset(utf8DialogInputBuffer, 0, 100*3);
	
	//from VS adaptations..
	
	anim_b_left = 0;
	anim_b_center = 0;
	anim_b_right = 0;

	now_id = 0;
	
	m_DetectKeyBoard = false;
	
	save_click_time_dialog = 0;
	save_click_time_dialog_now = 0;
	check_dbl_click_dialog_now = false;		
}

void CDialogWindow::SetInfo(char* szInfo, int length)
{
	if(!szInfo || !length) return;

	if(m_putf8Info)
	{
		free(m_putf8Info);
		m_putf8Info = nullptr;
	}

	if(m_pszInfo)
	{
		free(m_pszInfo);
		m_pszInfo = nullptr;
	}

	m_putf8Info = (char*)malloc((length * 3) + 1);
	if(!m_putf8Info)
	{
		Log("CDialog::SetInfo: can't allocate memory");
		return;
	}
	
	m_putf8Info_orig = (char*)malloc((length * 3) + 1);
	if(!m_putf8Info_orig)
	{
		Log("CDialog::SetInfo: can't allocate memory(2)");
		return;
	}	

	m_pszInfo = (char*)malloc((length * 3) + 1);
	if(!m_pszInfo)
	{
		Log("CDialog::SetInfo: can't allocate memory");
		return;
	}

	cp1251_to_utf8(m_putf8Info, szInfo);
	
	strcpy(m_putf8Info_orig, (const char*)szInfo);

	// =========
	char szNonColorEmbeddedMsg[4200];
	int iNonColorEmbeddedMsgLen = 0;

	for (size_t pos = 0; pos < strlen(szInfo) && szInfo[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szInfo))
		{
			if (szInfo[pos] == '{' && szInfo[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColorEmbeddedMsg[iNonColorEmbeddedMsgLen] = szInfo[pos];
		iNonColorEmbeddedMsgLen++;
	}

	szNonColorEmbeddedMsg[iNonColorEmbeddedMsgLen] = 0;
	// ========

	cp1251_to_utf8(m_pszInfo, szNonColorEmbeddedMsg);
}

bool ProcessInlineHexColorDialog(const char* start, const char* end, ImVec4& color)
{
    const int hexCount = (int)(end - start);
    if (hexCount == 6 || hexCount == 8)
    {
        char hex[9];
        strncpy(hex, start, hexCount);
        hex[hexCount] = 0;

        unsigned int hexColor = 0;
        if (sscanf(hex, "%x", &hexColor) > 0)
        {
            color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
            color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
            color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
            color.w = 1.0f;

            if (hexCount == 8)
            {
                color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
            }

            return true;
        }
    }

    return false;
}

void TextWithBoev(ImVec2 pos, ImColor col, const char* szStr)
{
    char tempStr[4096];

    ImGuiIO& io = ImGui::GetIO();

    ImVec2 vecPos = pos;

    strcpy(tempStr, szStr);
    tempStr[sizeof(tempStr) - 1] = '\0';

    bool pushedColorStyle = false;
    const char* textStart = tempStr;
    const char* textCur = tempStr;
    while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
    {
        if (*textCur == '{')
        {
            // Print accumulated text
            if (textCur != textStart)
            {
                //outline
                vecPos.x -= 1;
                ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
                vecPos.x += 1;
                // right 
                vecPos.x += 1;
                ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
                vecPos.x -= 1;
                // above
                vecPos.y -= 1;
                ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
                vecPos.y += 1;
                // below
                vecPos.y += 1;
                ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
                vecPos.y -= 1;

                ImGui::GetOverlayDrawList()->AddText(vecPos, col, textStart, textCur);
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
            {
                pushedColorStyle = false;
            }

            ImVec4 textColor;
            if (ProcessInlineHexColorDialog(colorStart, textCur, textColor))
            {
                col = textColor;
                pushedColorStyle = true;
            }

            textStart = textCur + 1;
        }
        else if (*textCur == '\n')
        {
            //outline
            vecPos.x -= 1;
            ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
            vecPos.x += 1;
            // right 
            vecPos.x += 1;
            ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
            vecPos.x -= 1;
            // above
            vecPos.y -= 1;
            ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
            vecPos.y += 1;
            // below
            vecPos.y += 1;
            ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
            vecPos.y -= 1;

            ImGui::GetOverlayDrawList()->AddText(vecPos, col, textStart, textCur);
            vecPos.x = pos.x;
            vecPos.y += ImGui::GetFontSize();
            textStart = textCur + 1;
        }

        ++textCur;
    }

    if (textCur != textStart)
    {
        //outline
        vecPos.x -= 1;
        ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
        vecPos.x += 1;
        // right 
        vecPos.x += 1;
        ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
        vecPos.x -= 1;
        // above
        vecPos.y -= 1;
        ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
        vecPos.y += 1;
        // below
        vecPos.y += 1;
        ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
        vecPos.y -= 1;

        ImGui::GetOverlayDrawList()->AddText(vecPos, col, textStart, textCur);
        vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
    }
    else
    {
        vecPos.y += ImGui::GetFontSize();
    }
}

void TextWithBoevRect(ImVec2 pos, ImColor col, const char* szStr, ImVec4 clip_rect)
{
    char tempStr[4096];

    ImVec2 vecPos = pos;

    strcpy(tempStr, szStr);
    tempStr[sizeof(tempStr) - 1] = '\0';

    bool pushedColorStyle = false;
    const char* textStart = tempStr;
    const char* textCur = tempStr;
    while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
    {
        if (*textCur == '{')
        {
            // Print accumulated text
            if (textCur != textStart)
            {
				//outline
				vecPos.x -= 1;
				ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
				vecPos.x += 1;
				// right 
				vecPos.x += 1;
				ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
				vecPos.x -= 1;
				// above
				vecPos.y -= 1;
				ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
				vecPos.y += 1;
				// below
				vecPos.y += 1;
				ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
				vecPos.y -= 1;				
				
                ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), vecPos, col, textStart, NULL, 0.0f, &clip_rect);
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
            {
                pushedColorStyle = false;
            }

            ImVec4 textColor;
            if (ProcessInlineHexColorDialog(colorStart, textCur, textColor))
            {
                col = textColor;
                pushedColorStyle = true;
            }

            textStart = textCur + 1;
        }
        else if (*textCur == '\n')
        {
			//outline
			vecPos.x -= 1;
			ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
			vecPos.x += 1;
			// right 
			vecPos.x += 1;
			ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
			vecPos.x -= 1;
			// above
			vecPos.y -= 1;
			ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
			vecPos.y += 1;
			// below
			vecPos.y += 1;
			ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
			vecPos.y -= 1;				
			
            // Print accumulated text an go to next line
            ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), vecPos, col, textStart, NULL, 0.0f, &clip_rect);
            vecPos.x = pos.x;
            vecPos.y += ImGui::GetFontSize();
            textStart = textCur + 1;
        }

        ++textCur;
    }

    if (textCur != textStart)
    {
		//outline
		vecPos.x -= 1;
		ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
		vecPos.x += 1;
		// right 
		vecPos.x += 1;
		ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
		vecPos.x -= 1;
		// above
		vecPos.y -= 1;
		ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
		vecPos.y += 1;
		// below
		vecPos.y += 1;
		ImGui::GetOverlayDrawList()->AddText(vecPos, ImColor(IM_COL32_BLACK), textStart, textCur);
		vecPos.y -= 1;			
		
        ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), vecPos, col, textStart, NULL, 0.0f, &clip_rect);
        vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
    }
    else
        vecPos.y += ImGui::GetFontSize();
}

//remove color from string (for center text)
std::string clear_text(std::string gang)
{
    for (int i = 0; i < gang.length(); i++)
    {
        if (gang[i] == '{')
        {
            gang.erase(i, 8);
        }
    }
    return gang;
}

bool IsMouseClickInArea(float x1, float y1, float x2, float y2)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseDownDuration[0] >= 0.0f)
    {
        if ((io.MousePos.x > x1 && x2 > io.MousePos.x))
        {
            if(io.MousePos.y > y1 && io.MousePos.y < y2)
                return true;
        }
        return false;
    }
    return false;
}

bool IsMouseClickInAreaClicked(float x1, float y1, float x2, float y2)
{
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsMouseClicked(0))
    {
        if ((io.MousePos.x > x1 && x2 > io.MousePos.x))
        {
            if(io.MousePos.y > y1 && io.MousePos.y < y2)
                return true;
        }
        return false;
    }
    return false;
}

void DialogWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	strcpy(szDialogInputBuffer, str);
	cp1251_to_utf8(utf8DialogInputBuffer, str);
}

void CDialogWindow::Render()
{
	if(!m_bIsActive || !m_putf8Info) return;

	ImGuiIO &io = ImGui::GetIO();

	/*ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(pGUI->GetFontSize(), pGUI->GetFontSize()));

	ImGui::Begin(m_utf8Title, nullptr, 
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

	switch(m_byteDialogStyle)
	{
		case DIALOG_STYLE_MSGBOX:
		TextWithColors(m_putf8Info);
		ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2) );
		break;

		case DIALOG_STYLE_INPUT:
		TextWithColors(m_putf8Info);
		ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2) );

		if( ImGui::Button(utf8DialogInputBuffer, ImVec2(ImGui::CalcTextSize(m_pszInfo).x, pGUI->GetFontSize() * 1.5f)) )
		{
			if(!pKeyBoard->IsOpen())
				pKeyBoard->Open(&DialogWindowInputHandler);
		}

		ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2) );
		break;
	}

	if(m_utf8Button1[0] != 0) 
	{
		if(ImGui::Button(m_utf8Button1, ImVec2(ImGui::CalcTextSize(m_utf8Button1).x * 1.5f, pGUI->GetFontSize() * 2)))
		{
			Show(false);
			if(pNetGame) 
				pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);
		}
	}
	ImGui::SameLine(0, pGUI->GetFontSize());
	if(m_utf8Button2[0] != 0) 
	{
		if(ImGui::Button(m_utf8Button2, ImVec2(ImGui::CalcTextSize(m_utf8Button2).x * 1.5f, pGUI->GetFontSize() * 2)))
		{
			Show(false);
			if(pNetGame) 
				pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);
		}
	}

	// Размешаем по центру
	ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y - size.y)/2)) );
	ImGui::End();

	ImGui::PopStyleVar();*/
	
	if(m_byteDialogStyle == DIALOG_STYLE_MSGBOX)
	{
        //left up angle
        ImVec2 left_up_angle = ImVec2(io.DisplaySize.x / 2.56, io.DisplaySize.y / 2.44);
        //right down angle
        ImVec2 right_down_angle = ImVec2(io.DisplaySize.x / 1.641, io.DisplaySize.y / 1.87);

        float center_position = io.DisplaySize.x / 2;

        float space_center = io.DisplaySize.x / 128;

        float size_button = io.DisplaySize.x / 8.5;

        float height_buttons_up = right_down_angle.y / 1.089;
        float height_button_down = right_down_angle.y / 1.02;

        float header_size = io.DisplaySize.y / 32.72;

        float up_pos_area_buttons = (right_down_angle.y - height_button_down) + (height_button_down - height_buttons_up) + (right_down_angle.y - height_button_down);

        //check true relation
        double get_relation_true_x = io.DisplaySize.y * 1.777777777777778; // 1280 / 720, 1920 / 1080 etc.

        //if now resolution > true relation - set true relation
        if (io.DisplaySize.x > get_relation_true_x)
        {
            float get_difference = io.DisplaySize.x - get_relation_true_x;
            left_up_angle.x = (get_relation_true_x / 2.56) + (get_difference / 2);
            right_down_angle.x = (get_relation_true_x / 1.641) + (get_difference / 2);

            space_center = get_relation_true_x / 128;

            size_button = get_relation_true_x / 8.5;
        }


        //main text
        std::string small_text = m_putf8Info;

        std::string new_small_text = clear_text(small_text);

        //======= counting the size text and our window ======

        float get_text_size_x = ImGui::CalcTextSize(new_small_text.c_str()).x;
        float get_text_size_y = ImGui::CalcTextSize(new_small_text.c_str()).y;

        float get_window_size_x = right_down_angle.x - left_up_angle.x;
        float get_window_size_y = right_down_angle.y - left_up_angle.y;

        float left_text_space_size = get_relation_true_x / 85.3333; //size space

        //  X
        if (get_text_size_x + (left_text_space_size * 2) >= get_window_size_x)
        {

            left_up_angle.x = left_up_angle.x - ((get_text_size_x / 2) - (get_window_size_x / 2)) - left_text_space_size;
            right_down_angle.x = right_down_angle.x + (get_text_size_x / 2) - (get_window_size_x / 2) + left_text_space_size;

        }
        //  Y
        float change_height_pos = 0;
        if (get_text_size_y + header_size + up_pos_area_buttons >= get_window_size_y)
        {
            left_up_angle.y -= (get_text_size_y/2) - ImGui::GetFontSize() * 1.5;
            right_down_angle.y += (get_text_size_y/2) + ImGui::GetFontSize() * 0.5;

            //recalculation pos button
            change_height_pos += (get_text_size_y / 2) + ImGui::GetFontSize() * 0.5;

        }

        //recalculation position
        float left_pos_text = left_up_angle.x + left_text_space_size; //start text in left pos
        float up_pos_text = left_up_angle.y + (header_size + ImGui::GetFontSize() / 3);

        //background main
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_up_angle.x, left_up_angle.y), ImVec2(right_down_angle.x, right_down_angle.y), ImColor(0, 0, 0, 190));
        //background header
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_up_angle.x, left_up_angle.y), ImVec2(right_down_angle.x, left_up_angle.y + header_size), ImColor(0, 0, 0, 210));

        //header text draw
        TextWithBoev(ImVec2(left_up_angle.x + 5, left_up_angle.y + ((header_size - ImGui::GetFontSize())/2)), ImColor(255, 255, 255, 255), m_utf8Title);

        //draw main text
        TextWithBoev(ImVec2(left_pos_text, up_pos_text), ImColor(169, 196, 228, 255), small_text.c_str());

        //left button
        const char* test_text_left_button = m_utf8Button1;

        std::string new_small_left_button = clear_text(test_text_left_button);

        //right button
        const char* test_text_right_button = m_utf8Button2;

        std::string new_small_right_button = clear_text(test_text_right_button);


        //find NOT NULL button

        if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) > 0)
        {
            //find long text for button
            if (new_small_left_button.length() > new_small_right_button.length()) //if left button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() < new_small_right_button.length()) //if right button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_right_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() == new_small_right_button.length())
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }

            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - space_center - size_button - (get_relation_true_x / 128), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position - space_center, height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position + space_center, height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + space_center + size_button + (get_relation_true_x / 128), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;


            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }
        else if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) == 0) //if LEFT button not null..
        {
            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);					
            }
            else
            {
                anim_b_left = 0;
            }

        }
        else if (strlen(test_text_left_button) == 0 && strlen(test_text_right_button) > 0) //if RIGHT button not null..
        {
            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);					
            }
            else
            {
                anim_b_right = 0;
            }
        }		
	}
	else if(m_byteDialogStyle == DIALOG_STYLE_INPUT)
	{
        //left up angle
        ImVec2 left_up_angle = ImVec2(io.DisplaySize.x / 2.56, io.DisplaySize.y / 2.44);
        //right down angle
        ImVec2 right_down_angle = ImVec2(io.DisplaySize.x / 1.641, io.DisplaySize.y / 1.87);

        right_down_angle.y += io.DisplaySize.y / 24; // +30

        right_down_angle.y += ImGui::GetFontSize() / 2;

        float center_position = io.DisplaySize.x / 2;

        float space_center = io.DisplaySize.x / 128;

        float size_button = io.DisplaySize.x / 8.5;

        float height_buttons_up = right_down_angle.y / 1.089;
        float height_button_down = right_down_angle.y / 1.02;

        float header_size = io.DisplaySize.y / 32.72;

        float up_pos_area_buttons = (right_down_angle.y - height_button_down) + (height_button_down - height_buttons_up) + (right_down_angle.y - height_button_down);

        //check true relation
        double get_relation_true_x = io.DisplaySize.y * 1.777777777777778; // 1280 / 720, 1920 / 1080 etc.

        //if now resolution > true relation - set true relation
        if (io.DisplaySize.x > get_relation_true_x)
        {
            float get_difference = io.DisplaySize.x - get_relation_true_x;
            left_up_angle.x = (get_relation_true_x / 2.56) + (get_difference / 2);
            right_down_angle.x = (get_relation_true_x / 1.641) + (get_difference / 2);

            space_center = get_relation_true_x / 128;

            size_button = get_relation_true_x / 8.5;
        }

        //main text
        std::string small_text = m_putf8Info;

        std::string new_small_text = clear_text(small_text);

        //======= counting the size text and our window ======

        float get_text_size_x = ImGui::CalcTextSize(new_small_text.c_str()).x;
        float get_text_size_y = ImGui::CalcTextSize(new_small_text.c_str()).y;

        float get_window_size_x = right_down_angle.x - left_up_angle.x;
        float get_window_size_y = right_down_angle.y - left_up_angle.y;

        float left_text_space_size = get_relation_true_x / 85.3333; //size space

        //  X
        if (get_text_size_x + (left_text_space_size * 2) >= get_window_size_x)
        {

            left_up_angle.x = left_up_angle.x - ((get_text_size_x / 2) - (get_window_size_x / 2)) - left_text_space_size;
            right_down_angle.x = right_down_angle.x + (get_text_size_x / 2) - (get_window_size_x / 2) + left_text_space_size;

        }
        //  Y
        float change_height_pos = 0;
        if (get_text_size_y + header_size + up_pos_area_buttons + (io.DisplaySize.y / 24) + (ImGui::GetFontSize() / 2) >= get_window_size_y)
        {
            left_up_angle.y -= (get_text_size_y / 2) - ImGui::GetFontSize() * 1.5;
            right_down_angle.y += (get_text_size_y / 2) + ImGui::GetFontSize() * 0.5;

            //recalculation pos button
            change_height_pos += (get_text_size_y / 2) + ImGui::GetFontSize() * 0.5;

        }

        //recalculation position
        float left_pos_text = left_up_angle.x + left_text_space_size; //start text in left pos
        float up_pos_text = left_up_angle.y + (header_size + ImGui::GetFontSize() / 3);

        //background main
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_up_angle.x, left_up_angle.y), ImVec2(right_down_angle.x, right_down_angle.y), ImColor(0, 0, 0, 190));
        //background header
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_up_angle.x, left_up_angle.y), ImVec2(right_down_angle.x, left_up_angle.y + header_size), ImColor(0, 0, 0, 210));

        //header text draw
        TextWithBoev(ImVec2(left_up_angle.x + 5, left_up_angle.y + ((header_size - ImGui::GetFontSize()) / 2)), ImColor(255, 255, 255, 255), m_utf8Title);

        //draw main text
        TextWithBoev(ImVec2(left_pos_text, up_pos_text), ImColor(169, 196, 228, 255), small_text.c_str());

        //input button
        //background color (white)
        ImVec2 left_input_pos = ImVec2(left_up_angle.x + (left_text_space_size / 3), up_pos_text + ImGui::CalcTextSize(small_text.c_str()).y + (ImGui::GetFontSize() / 2));
        ImVec2 right_input_pos = ImVec2(right_down_angle.x - (left_text_space_size / 3), up_pos_text + ImGui::CalcTextSize(small_text.c_str()).y + (ImGui::GetFontSize() / 2) + (io.DisplaySize.y / 24));

        if(!anim_b_center)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_input_pos.x, left_input_pos.y), ImVec2(right_input_pos.x, right_input_pos.y), ImColor(255, 255, 255, 255), 6.0f);
        //main color (dark)
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_input_pos.x + 1, left_input_pos.y + 1), ImVec2(right_input_pos.x - 1, right_input_pos.y - 1), ImColor(0, 0, 0, 255), 6.0f);

        std::string input_text = utf8DialogInputBuffer;

        float max_long = (right_input_pos.x + (left_text_space_size / 2)) - (left_input_pos.x - (left_text_space_size / 2));


        //if text is long, add rect..
        if (ImGui::CalcTextSize(input_text.c_str()).x > max_long)
        {
            //TextWithBoevRect(ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), input_text.c_str(), ImVec4(left_input_pos.x, left_input_pos.y, left_input_pos.x + (right_input_pos.x - left_input_pos.x) - (left_text_space_size / 2), left_input_pos.y + (io.DisplaySize.y / 24)));
            ImVec4 get_text_rect = ImVec4(ImVec4(left_input_pos.x, left_input_pos.y, left_input_pos.x + (right_input_pos.x - left_input_pos.x) - (left_text_space_size / 2), left_input_pos.y + (io.DisplaySize.y / 24)));
            ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255,255,255,255), input_text.c_str(), NULL, 0.0f, &get_text_rect);
        }
        else
        {
            //text render input
            //TextWithBoev(ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), input_text.c_str());
            ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), input_text.c_str(), NULL, 0.0f);

        }

		/*if (IsMouseClickInAreaClicked(left_input_pos.x, left_input_pos.y, right_input_pos.x, right_input_pos.y))
		{
			ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_input_pos.x, left_input_pos.y), ImVec2(right_input_pos.x, right_input_pos.y), ImColor(95, 0, 0, 255), 6.0f);
			anim_b_center = 1;
				
			//render KeyBoard..
			if(!pKeyBoard->IsOpen())
				pKeyBoard->Open(&DialogWindowInputHandler);
		}*/
		
		if(io.MouseDown[0])
		{
			if ((io.MousePos.x > left_input_pos.x && right_input_pos.x > io.MousePos.x))
			{
				if(io.MousePos.y > left_input_pos.y && io.MousePos.y < right_input_pos.y)
				{
					ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_input_pos.x, left_input_pos.y), ImVec2(right_input_pos.x, right_input_pos.y), ImColor(95, 0, 0, 255), 6.0f);
					anim_b_center = 1;
					
					io.MouseDown[0] = false;
					
					m_DetectKeyBoard = true;
							
					//render KeyBoard..
					if(!pKeyBoard->IsOpen())
						pKeyBoard->Open(&DialogWindowInputHandler);

				}
			}
		}
		
		anim_b_center = 0;

        //left button
        const char* test_text_left_button = m_utf8Button1;

        std::string new_small_left_button = clear_text(test_text_left_button);

        //right button
        const char* test_text_right_button = m_utf8Button2;

        std::string new_small_right_button = clear_text(test_text_right_button);


        //find NOT NULL button

        if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) > 0)
        {
            //find long text for button
            if (new_small_left_button.length() > new_small_right_button.length()) //if left button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() < new_small_right_button.length()) //if right button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_right_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() == new_small_right_button.length())
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }

            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - space_center - size_button - (get_relation_true_x / 128), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position - space_center, height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position + space_center, height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + space_center + size_button + (get_relation_true_x / 128), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;


            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }
        else if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) == 0) //if LEFT button not null..
        {
            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

        }
        else if (strlen(test_text_left_button) == 0 && strlen(test_text_right_button) > 0) //if RIGHT button not null..
        {
            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }		
	}
	else if(m_byteDialogStyle == DIALOG_STYLE_LIST)
	{
        ImVec2 list_left_up = ImVec2(io.DisplaySize.x / 2.9090, io.DisplaySize.y / 3.2);
        ImVec2 list_right_down = ImVec2(io.DisplaySize.x / 1.5238, io.DisplaySize.y / 1.4545);

        float space_offset = io.DisplaySize.y / 72;

        float size_button = io.DisplaySize.x / 8.5;

        float height_button = io.DisplaySize.y / 18;

        float header_size = io.DisplaySize.y / 28.8;

        float space_center = io.DisplaySize.x / 128;

        float center_position = io.DisplaySize.x / 2;

        float height_buttons_up = list_right_down.y / 1.064;
        float height_button_down = list_right_down.y / 1.010;

        float space_offset_start_list_button = io.DisplaySize.y / 144; //5

        float offset_start_y_orig = list_left_up.y + header_size + space_offset + space_offset_start_list_button + 1; //266

        float offset_start_y = list_left_up.y + header_size + space_offset + space_offset_start_list_button + 1; //266

        float step_next_pos_button_list = io.DisplaySize.y / 20.5714; //35

        float down_pos_area_list = list_right_down.y - (height_button * 1.5) + header_size; //455

        //main text
        std::string list_text = m_putf8Info;

        std::string new_list_text = clear_text(list_text);

        std::vector<std::string> elems;

        int elements = 0;

        std::stringstream ss(list_text);
        std::string item;
        while (std::getline(ss, item, '\n'))
        {
            elems.push_back(item);
            elements++;
        }
		
		//for save original line
        std::string list_text_orig = m_putf8Info_orig;

        std::string new_list_text_orig = clear_text(list_text_orig);

        std::vector<std::string> elems_orig;

        std::stringstream ss_orig(list_text_orig);
        std::string item_orig;
        while (std::getline(ss_orig, item_orig, '\n'))
        {
            elems_orig.push_back(item_orig);
        }
		//====================================

        float maybe = offset_start_y + (elements * (step_next_pos_button_list + space_offset_start_list_button)); //elements * 40

        //check true relation
        double get_relation_true_x = io.DisplaySize.y * 1.777777777777778; // 1280 / 720, 1920 / 1080 etc.

        //if now resolution > true relation - set true relation
        if (io.DisplaySize.x > get_relation_true_x)
        {
            float get_difference = io.DisplaySize.x - get_relation_true_x;
            list_left_up.x = (get_relation_true_x / 2.9090) + (get_difference / 2);
            list_right_down.x = (get_relation_true_x / 1.5238) + (get_difference / 2);

            space_center = get_relation_true_x / 128;

            size_button = get_relation_true_x / 8.5;
        }

        float get_text_size_x = ImGui::CalcTextSize(new_list_text.c_str()).x;

        float get_window_size_x = list_right_down.x - list_left_up.x;

        //  X
        if (get_text_size_x + (space_offset * 4) >= get_window_size_x)
        {
            float moved_pos = (get_text_size_x + (space_offset * 4)) - get_window_size_x;

            list_left_up.x -= (moved_pos / 2) + (space_offset / 2);
            list_right_down.x += (moved_pos / 2) + (space_offset / 2);
        }

        //  Y
        float change_height_pos = 0;

        float difference_new_old_pos = (maybe - down_pos_area_list) / 2;

        static bool detect_new_long = false;


        float maybe_save = offset_start_y + (9 * (step_next_pos_button_list + space_offset_start_list_button)); //elements * 40
        float difference_new_old_save_pos = (maybe_save - down_pos_area_list) / 2;

        if (maybe > down_pos_area_list && elements <= 10)
        {
			//added in client
			detect_new_long = false;
			//======================
			
            list_left_up.y -= difference_new_old_pos;

            list_right_down.y += difference_new_old_pos;

            //update pos
            height_buttons_up += difference_new_old_pos;
            height_button_down += difference_new_old_pos;

            down_pos_area_list += (maybe - down_pos_area_list) + space_offset_start_list_button;

            offset_start_y -= difference_new_old_pos;
        }
        else if (maybe > down_pos_area_list && elements >= 11)
        {
            detect_new_long = true;

            list_left_up.y -= difference_new_old_save_pos;

            list_right_down.y += difference_new_old_save_pos;

            //update pos
            height_buttons_up += difference_new_old_save_pos;
            height_button_down += difference_new_old_save_pos;

            //down_pos_area_list = list_right_down.y + header_size;
            //честно хуй знает почему тут надо делить на 4, но +- работает так лол
            down_pos_area_list += (maybe_save / 4) + (space_offset_start_list_button * 2);

            offset_start_y -= difference_new_old_save_pos;
        }

        static float offset = 0;
        if (ImGui::IsMouseDragging())
        {
            if ((io.MousePos.x > list_left_up.x && io.MousePos.x < list_right_down.x) && (io.MousePos.y > list_left_up.y && io.MousePos.y < list_right_down.y))
            {
                offset += ImGui::GetIO().MouseDelta.y;
            }
        }

        //background main
        ImGui::GetOverlayDrawList()->AddRectFilled(list_left_up, list_right_down, ImColor(0, 0, 0, 150));
        //header
        ImGui::GetOverlayDrawList()->AddRectFilled(list_left_up, ImVec2(list_right_down.x, list_left_up.y + header_size), ImColor(0, 0, 0, 210));

        //header text
        TextWithBoev(ImVec2(list_left_up.x + space_offset_start_list_button, list_left_up.y + ((header_size - ImGui::GetFontSize()) / 2)), ImColor(255, 255, 255, 255), m_utf8Title);

        //obvodka
        ImGui::GetOverlayDrawList()->AddRect(ImVec2(list_left_up.x + space_offset, list_left_up.y + header_size + space_offset), ImVec2(list_right_down.x - space_offset, list_right_down.y - (height_button * 1.5) + header_size), IM_COL32(255, 255, 255, 200), 5.0f);
        //background child
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, list_right_down.y - (height_button * 1.5) + header_size - 1), IM_COL32(0, 0, 0, 160), 5.0f);

        int get_id = 0;

        //==================================
        //block offset buttons to area
        if (maybe + offset < down_pos_area_list)
        {
            offset = maybe - down_pos_area_list;
            offset -= offset * 2; //negative 
        }

        //if (266 + offset > 266)
        if (offset_start_y_orig + offset > offset_start_y_orig)
        {
            offset = 0;
        }
        //==================================

        float scroll_space = 0;
        if (maybe > down_pos_area_list) // > 455
        {
            scroll_space = space_offset;
        }

        ImGui::GetOverlayDrawList()->PushClipRect(ImVec2(list_left_up.x + space_offset + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, list_right_down.y - (height_button * 1.5) + header_size - 1), true);

		//double click by Vadim Boev
		//=============================================
		//static int time_dbl_test = 500;
		
		//just click for change id button
		//static uint32_t save_click_time_dialog = 0;
		
		//for get double where id button = id button
		//static uint32_t save_click_time_dialog_now = 0;
		//static bool check_dbl_click_dialog_now = false;	
		//=============================================

        for (auto& it : elems)
        {
            const char* cstr = it.c_str();

			if(now_id == 0)
			{
				DialogWindowInputHandler(elems_orig[0].c_str());
			}

            //if mouse in rect
            if ((io.MousePos.x > list_left_up.x + space_offset && io.MousePos.x < list_right_down.x - space_offset) && (io.MousePos.y > offset_start_y + offset && io.MousePos.y < offset_start_y + step_next_pos_button_list + offset))
            {
                if (now_id == get_id)
                {
					if (ImGui::IsMouseClicked(0) && io.MouseDownDuration[0] >= 0.0f)
					{
						if (io.MousePos.y < list_right_down.y - (height_button * 1.5) + header_size - 1)
						{
							if((GetTickCount() - save_click_time_dialog_now) < time_dbl_test && check_dbl_click_dialog_now)
							{
								//if(pChatWindow) pChatWindow->AddDebugMessage("[two]list test dialog!");
								DialogWindowInputHandler(elems_orig[get_id].c_str());	
								
								if(pNetGame) 
									pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);
								
								check_dbl_click_dialog_now = false;
								
								Show(false);
							}
							else
							{
								save_click_time_dialog_now = GetTickCount();
								check_dbl_click_dialog_now = true;
								DialogWindowInputHandler(elems_orig[get_id].c_str());
							}
						}
					}				
					
                    ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                }

                if (ImGui::IsMouseClicked(0) && io.MouseDownDuration[0] >= 0.0f)
                {
                    if (io.MousePos.y < list_right_down.y - (height_button * 1.5) + header_size - 1)
                    {
						if(now_id == get_id)
						{
							if((GetTickCount() - save_click_time_dialog) < time_dbl_test)
							{
								//if(pChatWindow) pChatWindow->AddDebugMessage("list test dialog!");		
								DialogWindowInputHandler(elems_orig[get_id].c_str());	
								
								if(pNetGame) 
									pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);
								
								Show(false);
							}
						}
						else
						{
							now_id = get_id;
							save_click_time_dialog = GetTickCount();
							DialogWindowInputHandler(elems_orig[get_id].c_str());
                        }
						
						ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                    }
                }

                TextWithBoev(ImVec2(list_left_up.x + (space_offset * 2), offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), cstr);
            }
            else //just render text and if clicked before button id == now id then render background
            {
                if (now_id == get_id)
                {
                    ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                }
                TextWithBoev(ImVec2(list_left_up.x + (space_offset * 2), offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), cstr);
            }

            //add distance from button to button
            offset_start_y += step_next_pos_button_list + space_offset_start_list_button;

            //next step
            get_id++;
        }

        //if size area buttons > rect area, render custom slider
        float changed_calc = list_right_down.y - (height_button * 1.5) + header_size;

        //down_pos_area_list
        if (offset_start_y > changed_calc && detect_new_long) //455 - 1
        {
            //my slider, okey..
            //where rounding on rect
            ImDrawCornerFlags corners_tl_br = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight;
            //draw rect, background
            ImGui::GetOverlayDrawList()->AddRect(ImVec2(list_right_down.x - (space_offset * 2), list_left_up.y + header_size + space_offset), ImVec2(list_right_down.x, down_pos_area_list), ImColor(255, 255, 255, 190), 5.0f, corners_tl_br);
            //draw rect, main (the left side is already)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, changed_calc - 1), ImColor(0, 0, 0, 255), 5.0f, corners_tl_br);
            //draw slider
            if (elements <= 26)
            {
                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 3, list_left_up.y + header_size + space_offset + 3 + (-offset / 2)), ImVec2(list_right_down.x - space_offset - 3, changed_calc - 1 - 3 - ((offset_start_y - changed_calc - 1) / 2) + (-offset / 2)), ImColor(177, 32, 43, 255), 5.0f);
            }
            else
            {
                float razn = elements - 26;

                double test_offset = 2;
                test_offset += (razn * 0.115) + (razn / 500);

                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 3, list_left_up.y + header_size + space_offset + 3 + (-offset / test_offset)), ImVec2(list_right_down.x - space_offset - 3, changed_calc - 1 - 3 - (((offset_start_y - (razn * (step_next_pos_button_list + space_offset_start_list_button))) - changed_calc - 1) / 2) + (-offset / test_offset)), ImColor(177, 32, 43, 255), 5.0f);
            }
        }
        else
        {
            offset = 0;
        }

        ImGui::GetOverlayDrawList()->PopClipRect();

        //left button
        const char* test_text_left_button = m_utf8Button1;

        std::string new_small_left_button = clear_text(test_text_left_button);

        //right button
        const char* test_text_right_button = m_utf8Button2;

        std::string new_small_right_button = clear_text(test_text_right_button);

        static bool get_clicked = false;

        if (ImGui::IsMouseClicked(0))
        {
            if (io.MousePos.y >= list_right_down.y - (height_button * 1.5) + header_size - 1)
            {
                get_clicked = true;
            }
            else
            {
                get_clicked = false;
            }
        }


        //find NOT NULL button

        if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) > 0)
        {
            //find long text for button
            if (new_small_left_button.length() > new_small_right_button.length()) //if left button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() < new_small_right_button.length()) //if right button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_right_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() == new_small_right_button.length())
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }

            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - space_center - size_button - (get_relation_true_x / 128), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position - space_center, height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position + space_center, height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + space_center + size_button + (get_relation_true_x / 128), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;


            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, now_id, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }
        else if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) == 0) //if LEFT button not null..
        {
            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);						
            }
            else
            {
                anim_b_left = 0;
            }

        }
        else if (strlen(test_text_left_button) == 0 && strlen(test_text_right_button) > 0) //if RIGHT button not null..
        {
            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, now_id, szDialogInputBuffer);					
            }
            else
            {
                anim_b_right = 0;
            }
        }		
	}
	else if(m_byteDialogStyle == DIALOG_STYLE_PASSWORD)
	{
		m_StylePassword = true;
		
        //left up angle
        ImVec2 left_up_angle = ImVec2(io.DisplaySize.x / 2.56, io.DisplaySize.y / 2.44);
        //right down angle
        ImVec2 right_down_angle = ImVec2(io.DisplaySize.x / 1.641, io.DisplaySize.y / 1.87);

        right_down_angle.y += io.DisplaySize.y / 24; // +30

        right_down_angle.y += ImGui::GetFontSize() / 2;

        float center_position = io.DisplaySize.x / 2;

        float space_center = io.DisplaySize.x / 128;

        float size_button = io.DisplaySize.x / 8.5;

        float height_buttons_up = right_down_angle.y / 1.089;
        float height_button_down = right_down_angle.y / 1.02;

        float header_size = io.DisplaySize.y / 32.72;

        float up_pos_area_buttons = (right_down_angle.y - height_button_down) + (height_button_down - height_buttons_up) + (right_down_angle.y - height_button_down);

        //check true relation
        double get_relation_true_x = io.DisplaySize.y * 1.777777777777778; // 1280 / 720, 1920 / 1080 etc.

        //if now resolution > true relation - set true relation
        if (io.DisplaySize.x > get_relation_true_x)
        {
            float get_difference = io.DisplaySize.x - get_relation_true_x;
            left_up_angle.x = (get_relation_true_x / 2.56) + (get_difference / 2);
            right_down_angle.x = (get_relation_true_x / 1.641) + (get_difference / 2);

            space_center = get_relation_true_x / 128;

            size_button = get_relation_true_x / 8.5;
        }

        //main text
        std::string small_text = m_putf8Info;

        std::string new_small_text = clear_text(small_text);

        //======= counting the size text and our window ======

        float get_text_size_x = ImGui::CalcTextSize(new_small_text.c_str()).x;
        float get_text_size_y = ImGui::CalcTextSize(new_small_text.c_str()).y;

        float get_window_size_x = right_down_angle.x - left_up_angle.x;
        float get_window_size_y = right_down_angle.y - left_up_angle.y;

        float left_text_space_size = get_relation_true_x / 85.3333; //size space

        //  X
        if (get_text_size_x + (left_text_space_size * 2) >= get_window_size_x)
        {

            left_up_angle.x = left_up_angle.x - ((get_text_size_x / 2) - (get_window_size_x / 2)) - left_text_space_size;
            right_down_angle.x = right_down_angle.x + (get_text_size_x / 2) - (get_window_size_x / 2) + left_text_space_size;

        }
        //  Y
        float change_height_pos = 0;
        if (get_text_size_y + header_size + up_pos_area_buttons + (io.DisplaySize.y / 24) + (ImGui::GetFontSize() / 2) >= get_window_size_y)
        {
            left_up_angle.y -= (get_text_size_y / 2) - ImGui::GetFontSize() * 1.5;
            right_down_angle.y += (get_text_size_y / 2) + ImGui::GetFontSize() * 0.5;

            //recalculation pos button
            change_height_pos += (get_text_size_y / 2) + ImGui::GetFontSize() * 0.5;

        }

        //recalculation position
        float left_pos_text = left_up_angle.x + left_text_space_size; //start text in left pos
        float up_pos_text = left_up_angle.y + (header_size + ImGui::GetFontSize() / 3);

        //background main
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_up_angle.x, left_up_angle.y), ImVec2(right_down_angle.x, right_down_angle.y), ImColor(0, 0, 0, 190));
        //background header
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_up_angle.x, left_up_angle.y), ImVec2(right_down_angle.x, left_up_angle.y + header_size), ImColor(0, 0, 0, 210));

        //header text draw
        TextWithBoev(ImVec2(left_up_angle.x + 5, left_up_angle.y + ((header_size - ImGui::GetFontSize()) / 2)), ImColor(255, 255, 255, 255), m_utf8Title);

        //draw main text
        TextWithBoev(ImVec2(left_pos_text, up_pos_text), ImColor(169, 196, 228, 255), small_text.c_str());

        //input button
        //background color (white)
        ImVec2 left_input_pos = ImVec2(left_up_angle.x + (left_text_space_size / 3), up_pos_text + ImGui::CalcTextSize(small_text.c_str()).y + (ImGui::GetFontSize() / 2));
        ImVec2 right_input_pos = ImVec2(right_down_angle.x - (left_text_space_size / 3), up_pos_text + ImGui::CalcTextSize(small_text.c_str()).y + (ImGui::GetFontSize() / 2) + (io.DisplaySize.y / 24));

        if(!anim_b_center)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_input_pos.x, left_input_pos.y), ImVec2(right_input_pos.x, right_input_pos.y), ImColor(255, 255, 255, 255), 6.0f);
        //main color (dark)
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_input_pos.x + 1, left_input_pos.y + 1), ImVec2(right_input_pos.x - 1, right_input_pos.y - 1), ImColor(0, 0, 0, 255), 6.0f);

        std::string input_text = utf8DialogInputBuffer;

        float max_long = (right_input_pos.x + (left_text_space_size / 2)) - (left_input_pos.x - (left_text_space_size / 2));

		std::string password_input_text = "";
		
		//if Russian KeyBoard
		if(pKeyBoard->NowLang() == 1)
		{
			for(int len_input_text = 0; len_input_text < input_text.length()/2; len_input_text++)
			{
				password_input_text.push_back('*');
			}			
		}
		else
		{
			for(int len_input_text = 0; len_input_text < input_text.length(); len_input_text++)
			{
				password_input_text.push_back('*');
			}
		}

        //if text is long, add rect..
        if (ImGui::CalcTextSize(password_input_text.c_str()).x > max_long)
        {
            //TextWithBoevRect(ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), input_text.c_str(), ImVec4(left_input_pos.x, left_input_pos.y, left_input_pos.x + (right_input_pos.x - left_input_pos.x) - (left_text_space_size / 2), left_input_pos.y + (io.DisplaySize.y / 24)));
            ImVec4 get_text_rect = ImVec4(ImVec4(left_input_pos.x, left_input_pos.y, left_input_pos.x + (right_input_pos.x - left_input_pos.x) - (left_text_space_size / 2), left_input_pos.y + (io.DisplaySize.y / 24)));
            ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255,255,255,255), password_input_text.c_str(), NULL, 0.0f, &get_text_rect);
        }
        else
        {
            //text render input
            //TextWithBoev(ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), input_text.c_str());
            ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(left_input_pos.x + (left_text_space_size / 2), left_input_pos.y + ((right_input_pos.y - left_input_pos.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), password_input_text.c_str(), NULL, 0.0f);

        }
		
		if(io.MouseDown[0])
		{
			if ((io.MousePos.x > left_input_pos.x && right_input_pos.x > io.MousePos.x))
			{
				if(io.MousePos.y > left_input_pos.y && io.MousePos.y < right_input_pos.y)
				{
					ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_input_pos.x, left_input_pos.y), ImVec2(right_input_pos.x, right_input_pos.y), ImColor(95, 0, 0, 255), 6.0f);
					anim_b_center = 1;
					
					io.MouseDown[0] = false;
					
					m_DetectKeyBoard = true;
							
					//render KeyBoard..
					if(!pKeyBoard->IsOpen())
						pKeyBoard->Open(&DialogWindowInputHandler);

				}
			}
		}
		
		anim_b_center = 0;

        //left button
        const char* test_text_left_button = m_utf8Button1;

        std::string new_small_left_button = clear_text(test_text_left_button);

        //right button
        const char* test_text_right_button = m_utf8Button2;

        std::string new_small_right_button = clear_text(test_text_right_button);


        //find NOT NULL button

        if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) > 0)
        {
            //find long text for button
            if (new_small_left_button.length() > new_small_right_button.length()) //if left button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() < new_small_right_button.length()) //if right button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_right_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() == new_small_right_button.length())
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }

            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - space_center - size_button - (get_relation_true_x / 128), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position - space_center, height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position + space_center, height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + space_center + size_button + (get_relation_true_x / 128), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;


            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				m_StylePassword = false;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				m_StylePassword = false;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }
        else if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) == 0) //if LEFT button not null..
        {
            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				m_StylePassword = false;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

        }
        else if (strlen(test_text_left_button) == 0 && strlen(test_text_right_button) > 0) //if RIGHT button not null..
        {
            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10))
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				m_StylePassword = false;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }		
	}
	else if(m_byteDialogStyle == DIALOG_STYLE_TABLIST)
	{
        ImVec2 list_left_up = ImVec2(io.DisplaySize.x / 2.9090, io.DisplaySize.y / 3.2);
        ImVec2 list_right_down = ImVec2(io.DisplaySize.x / 1.5238, io.DisplaySize.y / 1.4545);

        float space_offset = io.DisplaySize.y / 72;

        float size_button = io.DisplaySize.x / 8.5;

        float height_button = io.DisplaySize.y / 18;

        float header_size = io.DisplaySize.y / 28.8;

        float space_center = io.DisplaySize.x / 128;

        float center_position = io.DisplaySize.x / 2;

        float height_buttons_up = list_right_down.y / 1.064;
        float height_button_down = list_right_down.y / 1.010;

        float space_offset_start_list_button = io.DisplaySize.y / 144; //5

        float offset_start_y_orig = list_left_up.y + header_size + space_offset + space_offset_start_list_button + 1; //266

        float offset_start_y = list_left_up.y + header_size + space_offset + space_offset_start_list_button + 1; //266

        float step_next_pos_button_list = io.DisplaySize.y / 20.5714; //35

        float down_pos_area_list = list_right_down.y - (height_button * 1.5) + header_size; //455

        //main text
        std::string list_text = m_putf8Info;

        std::string new_list_text = clear_text(list_text);

        std::vector<std::string> elems;
        std::vector<std::string> clear_elems;

        int elements = 0;

        //counter \t
        int column = 0, p = 0;
        bool get_one_line = false;

        std::stringstream ss(list_text);
        std::string item;
        while (std::getline(ss, item, '\n'))
        {
            elems.push_back(item);
            clear_elems.push_back(clear_text(item));
            elements++;

            //find all '\t' in one line 
            if (!get_one_line)
            {
                while ((p = item.find('\t', p)) != item.npos)
                {
                    p++;
                    column++;
                }
                get_one_line = true; //disable next step find
            }
        }
		
		//for save original line
        std::string list_text_orig = m_putf8Info_orig;

        std::string new_list_text_orig = clear_text(list_text_orig);

        std::vector<std::string> elems_orig;

        std::stringstream ss_orig(new_list_text_orig);
        std::string item_orig;
        while (std::getline(ss_orig, item_orig, '\n'))
        {
            elems_orig.push_back(item_orig);
        }		

        int get_number_line_long = 0; //find line long
        int size_line = 0; //get max length line

        //process find..
        for (int line_length = 0; line_length < clear_elems.size(); line_length++)
        {
            if (clear_elems[line_length].length() > size_line)
            {
                get_number_line_long = line_length;
                size_line = clear_elems[line_length].length();
            }
        }

        float maybe = offset_start_y + (elements * (step_next_pos_button_list + space_offset_start_list_button)); //elements * 40

        //check true relation
        double get_relation_true_x = io.DisplaySize.y * 1.777777777777778; // 1280 / 720, 1920 / 1080 etc.

        //if now resolution > true relation - set true relation
        if (io.DisplaySize.x > get_relation_true_x)
        {
            float get_difference = io.DisplaySize.x - get_relation_true_x;
            list_left_up.x = (get_relation_true_x / 2.9090) + (get_difference / 2);
            list_right_down.x = (get_relation_true_x / 1.5238) + (get_difference / 2);

            space_center = get_relation_true_x / 128;

            size_button = get_relation_true_x / 8.5;
        }

        float get_text_size_x = ImGui::CalcTextSize(new_list_text.c_str()).x;

        float get_window_size_x = list_right_down.x - list_left_up.x;

        //  X
        if (get_text_size_x + (space_offset * 4) >= get_window_size_x)
        {
            //printf("trig\n");
            float moved_pos = (get_text_size_x + (space_offset * 4)) - get_window_size_x;

            list_left_up.x -= (moved_pos / 2) + (space_offset / 2);
            list_right_down.x += (moved_pos / 2) + (space_offset / 2);
        }

        int get_now_id_pos = 0;

        std::vector<std::string> elems_pos_all_line_long;

        std::stringstream pos_all_line_long(clear_elems[get_number_line_long].c_str());
        std::string item_pos_all_line_long;
        while (std::getline(pos_all_line_long, item_pos_all_line_long, '\t'))
        {
            elems_pos_all_line_long.push_back(item_pos_all_line_long);
        }

        float column0 = 0;
        float column1 = 0;
        float column2 = 0;
        float column3 = 0;

        float temp_size_text = 0;

        for (auto& it : elems_pos_all_line_long)
        {
            if (get_now_id_pos == 0)
            {
                column0 = list_left_up.x + (space_offset * 2);
                temp_size_text = ImGui::CalcTextSize(it.c_str()).x;
            }
            else if (get_now_id_pos == 1)
            {
                column1 = (ImGui::CalcTextSize(" ").x * 4) + column0 + temp_size_text;
                temp_size_text = ImGui::CalcTextSize(it.c_str()).x;
            }
            else if (get_now_id_pos == 2)
            {
                column2 = (ImGui::CalcTextSize(" ").x * 4) + column1 + temp_size_text;
                temp_size_text = ImGui::CalcTextSize(it.c_str()).x;
            }
            else if (get_now_id_pos == 3)
            {
                column3 = (ImGui::CalcTextSize(" ").x * 4) + column2 + temp_size_text;
            }
            get_now_id_pos++;
        }

        //  Y
        float change_height_pos = 0;

        float difference_new_old_pos = (maybe - down_pos_area_list) / 2;

        static bool detect_new_long = false;


        float maybe_save = offset_start_y + (9 * (step_next_pos_button_list + space_offset_start_list_button)); //elements * 40
        float difference_new_old_save_pos = (maybe_save - down_pos_area_list) / 2;

        if (maybe > down_pos_area_list && elements <= 10)
        {
			detect_new_long = false;
			
            list_left_up.y -= difference_new_old_pos;

            list_right_down.y += difference_new_old_pos;

            //update pos
            height_buttons_up += difference_new_old_pos;
            height_button_down += difference_new_old_pos;

            down_pos_area_list += (maybe - down_pos_area_list) + space_offset_start_list_button;

            offset_start_y -= difference_new_old_pos;
        }
        else if (maybe > down_pos_area_list && elements >= 11)
        {
            detect_new_long = true;

            list_left_up.y -= difference_new_old_save_pos;

            list_right_down.y += difference_new_old_save_pos;

            //update pos
            height_buttons_up += difference_new_old_save_pos;
            height_button_down += difference_new_old_save_pos;

            //down_pos_area_list = list_right_down.y + header_size;
            //честно хуй знает почему тут надо делить на 4, но +- работает так лол
            down_pos_area_list += (maybe_save / 4) + (space_offset_start_list_button * 2);

            offset_start_y -= difference_new_old_save_pos;
        }

        static float offset = 0;
        if (ImGui::IsMouseDragging())
        {
            if ((io.MousePos.x > list_left_up.x && io.MousePos.x < list_right_down.x) && (io.MousePos.y > list_left_up.y && io.MousePos.y < list_right_down.y))
            {
                offset += ImGui::GetIO().MouseDelta.y;
            }
        }

        //background main
        ImGui::GetOverlayDrawList()->AddRectFilled(list_left_up, list_right_down, ImColor(0, 0, 0, 150));
        //header
        ImGui::GetOverlayDrawList()->AddRectFilled(list_left_up, ImVec2(list_right_down.x, list_left_up.y + header_size), ImColor(0, 0, 0, 210));

        //header text
        TextWithBoev(ImVec2(list_left_up.x + space_offset_start_list_button, list_left_up.y + ((header_size - ImGui::GetFontSize()) / 2)), ImColor(255, 255, 255, 255), m_utf8Title);

        //obvodka
        ImGui::GetOverlayDrawList()->AddRect(ImVec2(list_left_up.x + space_offset, list_left_up.y + header_size + space_offset), ImVec2(list_right_down.x - space_offset, list_right_down.y - (height_button * 1.5) + header_size), IM_COL32(255, 255, 255, 200), 5.0f);
        //background child
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, list_right_down.y - (height_button * 1.5) + header_size - 1), IM_COL32(0, 0, 0, 160), 5.0f);

        int get_id = 0;

        //==================================
        //block offset buttons to area
        if (maybe + offset < down_pos_area_list)
        {
            offset = maybe - down_pos_area_list;
            offset -= offset * 2; //negative
        }

        //if (266 + offset > 266)
        if (offset_start_y_orig + offset > offset_start_y_orig)
        {
            offset = 0;
        }
        //==================================

        float scroll_space = 0;
        if (maybe > down_pos_area_list) // > 455
        {
            scroll_space = space_offset;
        }

        ImGui::GetOverlayDrawList()->PushClipRect(ImVec2(list_left_up.x + space_offset + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, list_right_down.y - (height_button * 1.5) + header_size - 1), true);

        for (auto& it : elems)
        {
            const char* cstr = it.c_str();
			
			std::vector<std::string> elems_get_column;

			std::stringstream pos_get_column(elems_orig[get_id].c_str());
			std::string item_pos_get_column;
			while (std::getline(pos_get_column, item_pos_get_column, '\t'))
			{
				elems_get_column.push_back(item_pos_get_column);
			}
			
			if(now_id == 0 && get_id == 0)
			{
				DialogWindowInputHandler(elems_get_column[0].c_str());
			}			

            //if mouse in rect
            if ((io.MousePos.x > list_left_up.x + space_offset && io.MousePos.x < list_right_down.x - space_offset) && (io.MousePos.y > offset_start_y + offset && io.MousePos.y < offset_start_y + step_next_pos_button_list + offset))
            {
                if (now_id == get_id)
                {
					if (ImGui::IsMouseClicked(0) && io.MouseDownDuration[0] >= 0.0f)
					{
						if (io.MousePos.y < list_right_down.y - (height_button * 1.5) + header_size - 1)
						{
							if((GetTickCount() - save_click_time_dialog_now) < time_dbl_test && check_dbl_click_dialog_now)
							{
								//if(pChatWindow) pChatWindow->AddDebugMessage("[two]list test dialog!");								
								
								DialogWindowInputHandler(elems_get_column[0].c_str());	
								
								if(pNetGame) 
									pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);								
								
								check_dbl_click_dialog_now = false;
								
								Show(false);
							}
							else
							{
								save_click_time_dialog_now = GetTickCount();
								check_dbl_click_dialog_now = true;
								
								DialogWindowInputHandler(elems_get_column[0].c_str());	
							}
						}
					}					
					
                    ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                }

                if (ImGui::IsMouseClicked(0) && io.MouseDownDuration[0] >= 0.0f)
                {
                    if (io.MousePos.y < list_right_down.y - (height_button * 1.5) + header_size - 1)
                    {
						if(now_id == get_id)
						{
							if((GetTickCount() - save_click_time_dialog) < time_dbl_test)
							{
								//if(pChatWindow) pChatWindow->AddDebugMessage("list test dialog!");												
								
								DialogWindowInputHandler(elems_get_column[0].c_str());	
								
								if(pNetGame) 
									pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);
								
								Show(false);
							}
						}
						else
						{
							now_id = get_id;
							save_click_time_dialog = GetTickCount();
							
							DialogWindowInputHandler(elems_get_column[0].c_str());	
                        }						
						
                        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                    }
                }

                if (column == 1) //2 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }

                }
                else if (column == 2) //3 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
                else if (column == 3) //4 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 3) //3 column
                        {
                            TextWithBoev(ImVec2(column3, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
            }
            else //just render text and if clicked before button id == now id then render background
            {
                if (now_id == get_id)
                {
                    ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                }
                //TextWithBoevForeground(ImVec2(470 + 1, offset_start_y + offset + 8), ImColor(255, 255, 255, 255), cstr);
                //TextWithBoev(ImVec2(list_left_up.x + (space_offset * 2), offset_start_y + offset + ((height_button / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), cstr);
                if (column == 1) //2 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }

                }
                else if (column == 2) //3 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
                else if (column == 3) //4 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 3) //3 column
                        {
                            TextWithBoev(ImVec2(column3, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
            }

            //add distance from button to button
            offset_start_y += step_next_pos_button_list + space_offset_start_list_button;

            //next step
            get_id++;
        }
		
        //if size area buttons > rect area, render custom slider
        float changed_calc = list_right_down.y - (height_button * 1.5) + header_size;

        //down_pos_area_list
        if (offset_start_y > changed_calc && detect_new_long) //455 - 1
        {
            //my slider, okey..
            //where rounding on rect
            ImDrawCornerFlags corners_tl_br = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight;
            //draw rect, background
            ImGui::GetOverlayDrawList()->AddRect(ImVec2(list_right_down.x - (space_offset * 2), list_left_up.y + header_size + space_offset), ImVec2(list_right_down.x, down_pos_area_list), ImColor(255, 255, 255, 190), 5.0f, corners_tl_br);
            //draw rect, main (the left side is already)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, changed_calc - 1), ImColor(0, 0, 0, 255), 5.0f, corners_tl_br);
            //draw slider
            if (elements <= 26)
            {
                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 3, list_left_up.y + header_size + space_offset + 3 + (-offset / 2)), ImVec2(list_right_down.x - space_offset - 3, changed_calc - 1 - 3 - ((offset_start_y - changed_calc - 1) / 2) + (-offset / 2)), ImColor(177, 32, 43, 255), 5.0f);
            }
            else
            {
                float razn = elements - 26;

                double test_offset = 2;
                test_offset += (razn * 0.115) + (razn / 500);

                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 3, list_left_up.y + header_size + space_offset + 3 + (-offset / test_offset)), ImVec2(list_right_down.x - space_offset - 3, changed_calc - 1 - 3 - (((offset_start_y - (razn * (step_next_pos_button_list + space_offset_start_list_button))) - changed_calc - 1) / 2) + (-offset / test_offset)), ImColor(177, 32, 43, 255), 5.0f);
            }
        }
        else
        {
            offset = 0;
        }

        ImGui::GetOverlayDrawList()->PopClipRect();

        //left button
        const char* test_text_left_button = m_utf8Button1;

        std::string new_small_left_button = clear_text(test_text_left_button);

        //right button
        const char* test_text_right_button = m_utf8Button2;

        std::string new_small_right_button = clear_text(test_text_right_button);

        static bool get_clicked = false;

        if (ImGui::IsMouseClicked(0))
        {
            if (io.MousePos.y >= list_right_down.y - (height_button * 1.5) + header_size - 1)
            {
                get_clicked = true;
            }
            else
            {
                get_clicked = false;
            }
        }


        //find NOT NULL button

        if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) > 0)
        {
            //find long text for button
            if (new_small_left_button.length() > new_small_right_button.length()) //if left button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() < new_small_right_button.length()) //if right button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_right_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() == new_small_right_button.length())
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }

            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - space_center - size_button - (get_relation_true_x / 128), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position - space_center, height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position + space_center, height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + space_center + size_button + (get_relation_true_x / 128), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;


            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);	
            }
            else
            {
                anim_b_left = 0;
            }

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, now_id, szDialogInputBuffer);		
            }
            else
            {
                anim_b_right = 0;
            }
        }
        else if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) == 0) //if LEFT button not null..
        {
            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, now_id, szDialogInputBuffer);	
            }
            else
            {
                anim_b_left = 0;
            }

        }
        else if (strlen(test_text_left_button) == 0 && strlen(test_text_right_button) > 0) //if RIGHT button not null..
        {
            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, now_id, szDialogInputBuffer);	
            }
            else
            {
                anim_b_right = 0;
            }
        }		
	}
	else if(m_byteDialogStyle == DIALOG_STYLE_TABLIST_HEADERS)
	{
        ImVec2 list_left_up = ImVec2(io.DisplaySize.x / 2.9090, io.DisplaySize.y / 3.2);
        ImVec2 list_right_down = ImVec2(io.DisplaySize.x / 1.5238, io.DisplaySize.y / 1.4545);

        float space_offset = io.DisplaySize.y / 72;

        float size_button = io.DisplaySize.x / 8.5;

        float height_button = io.DisplaySize.y / 18;

        float header_size = io.DisplaySize.y / 28.8;

        float space_center = io.DisplaySize.x / 128;

        float center_position = io.DisplaySize.x / 2;

        float height_buttons_up = list_right_down.y / 1.064;
        float height_button_down = list_right_down.y / 1.010;

        float space_offset_start_list_button = io.DisplaySize.y / 144; //5

        float offset_start_y_orig = list_left_up.y + header_size + space_offset + space_offset_start_list_button + 1; //266

        float offset_start_y = list_left_up.y + header_size + space_offset + space_offset_start_list_button + 1; //266

        float step_next_pos_button_list = io.DisplaySize.y / 20.5714; //35

        float down_pos_area_list = list_right_down.y - (height_button * 1.5) + header_size; //455

        //main text
        std::string list_text = m_putf8Info;

        std::string new_list_text = clear_text(list_text);

        std::vector<std::string> elems;
        std::vector<std::string> clear_elems;

        int elements = 0;

        //counter \t
        int column = 0, p = 0;
        bool get_one_line = false;

        std::stringstream ss(list_text);
        std::string item;
        while (std::getline(ss, item, '\n'))
        {
            elems.push_back(item);
            clear_elems.push_back(clear_text(item));
            elements++;

            //find all '\t' in one line 
            if (!get_one_line)
            {
                while ((p = item.find('\t', p)) != item.npos)
                {
                    p++;
                    column++;
                }
                get_one_line = true; //disable next step find
            }
        }
		
		//for save original line
        std::string list_text_orig = m_putf8Info_orig;

        std::string new_list_text_orig = clear_text(list_text_orig);

        std::vector<std::string> elems_orig;

        std::stringstream ss_orig(new_list_text_orig);
        std::string item_orig;
        while (std::getline(ss_orig, item_orig, '\n'))
        {
            elems_orig.push_back(item_orig);
        }		

        //remove one elements, because its header tablist
        elements = elements - 1;

        int get_number_line_long = 0; //find line long
        int size_line = 0; //get max length line

        //process find..
        for (int line_length = 0; line_length < clear_elems.size(); line_length++)
        {
            if (clear_elems[line_length].length() > size_line)
            {
                get_number_line_long = line_length;
                size_line = clear_elems[line_length].length();
            }
        }

        float maybe = offset_start_y + (elements * (step_next_pos_button_list + space_offset_start_list_button)); //elements * 40

        //check true relation
        double get_relation_true_x = io.DisplaySize.y * 1.777777777777778; // 1280 / 720, 1920 / 1080 etc.

        //if now resolution > true relation - set true relation
        if (io.DisplaySize.x > get_relation_true_x)
        {
            float get_difference = io.DisplaySize.x - get_relation_true_x;
            list_left_up.x = (get_relation_true_x / 2.9090) + (get_difference / 2);
            list_right_down.x = (get_relation_true_x / 1.5238) + (get_difference / 2);

            space_center = get_relation_true_x / 128;

            size_button = get_relation_true_x / 8.5;
        }

        float get_text_size_x = ImGui::CalcTextSize(new_list_text.c_str()).x;

        float get_window_size_x = list_right_down.x - list_left_up.x;

        //  X
        if (get_text_size_x + (space_offset * 4) >= get_window_size_x)
        {
            float moved_pos = (get_text_size_x + (space_offset * 4)) - get_window_size_x;

            list_left_up.x -= (moved_pos / 2) + (space_offset / 2);
            list_right_down.x += (moved_pos / 2) + (space_offset / 2);
        }

        int get_now_id_pos = 0;

        std::vector<std::string> elems_pos_all_line_long;

        std::stringstream pos_all_line_long(clear_elems[get_number_line_long].c_str());
        std::string item_pos_all_line_long;
        while (std::getline(pos_all_line_long, item_pos_all_line_long, '\t'))
        {
            elems_pos_all_line_long.push_back(item_pos_all_line_long);
        }

        float column0 = 0;
        float column1 = 0;
        float column2 = 0;
        float column3 = 0;

        float temp_size_text = 0;

        for (auto& it : elems_pos_all_line_long)
        {
            if (get_now_id_pos == 0)
            {
                column0 = list_left_up.x + (space_offset * 2);
                temp_size_text = ImGui::CalcTextSize(it.c_str()).x;
            }
            else if (get_now_id_pos == 1)
            {
                column1 = (ImGui::CalcTextSize(" ").x * 4) + column0 + temp_size_text;
                temp_size_text = ImGui::CalcTextSize(it.c_str()).x;
            }
            else if (get_now_id_pos == 2)
            {
                column2 = (ImGui::CalcTextSize(" ").x * 4) + column1 + temp_size_text;
                temp_size_text = ImGui::CalcTextSize(it.c_str()).x;
            }
            else if (get_now_id_pos == 3)
            {
                column3 = (ImGui::CalcTextSize(" ").x * 4) + column2 + temp_size_text;
            }
            get_now_id_pos++;
        }

        //  Y
        float change_height_pos = 0;

        float difference_new_old_pos = (maybe - down_pos_area_list) / 2;

        static bool detect_new_long = false;


        float maybe_save = offset_start_y + (9 * (step_next_pos_button_list + space_offset_start_list_button)); //elements * 40
        float difference_new_old_save_pos = (maybe_save - down_pos_area_list) / 2;

        if (maybe > down_pos_area_list && elements <= 10)
        {
			detect_new_long = false;
			
            list_left_up.y -= difference_new_old_pos;

            list_right_down.y += difference_new_old_pos;

            //update pos
            height_buttons_up += difference_new_old_pos;
            height_button_down += difference_new_old_pos;

            down_pos_area_list += (maybe - down_pos_area_list) + space_offset_start_list_button;

            offset_start_y -= difference_new_old_pos;
        }
        else if (maybe > down_pos_area_list && elements >= 11)
        {
            detect_new_long = true;

            list_left_up.y -= difference_new_old_save_pos;

            list_right_down.y += difference_new_old_save_pos;

            //update pos
            height_buttons_up += difference_new_old_save_pos;
            height_button_down += difference_new_old_save_pos;

            //down_pos_area_list = list_right_down.y + header_size;
            //честно хуй знает почему тут надо делить на 4, но +- работает так лол
            down_pos_area_list += (maybe_save / 4) + (space_offset_start_list_button * 2);

            offset_start_y -= difference_new_old_save_pos;
        }

        static float offset = 0;
        if (ImGui::IsMouseDragging())
        {
            if ((io.MousePos.x > list_left_up.x && io.MousePos.x < list_right_down.x) && (io.MousePos.y > list_left_up.y && io.MousePos.y < list_right_down.y))
            {
                offset += ImGui::GetIO().MouseDelta.y;
            }
        }

        //offset for tablist_header text
        list_left_up.y -= ImGui::GetFontSize();

        //background main
        ImGui::GetOverlayDrawList()->AddRectFilled(list_left_up, list_right_down, ImColor(0, 0, 0, 150));
        //header
        ImGui::GetOverlayDrawList()->AddRectFilled(list_left_up, ImVec2(list_right_down.x, list_left_up.y + header_size), ImColor(0, 0, 0, 210));

        //header text
        TextWithBoev(ImVec2(list_left_up.x + space_offset_start_list_button, list_left_up.y + ((header_size - ImGui::GetFontSize()) / 2)), ImColor(255, 255, 255, 255), m_utf8Title);

        //header tablist render
        float render_tablist_header = list_left_up.y + header_size + (space_offset/2) + ((list_left_up.y + header_size + ImGui::GetFontSize()) - (list_left_up.y + header_size)) - ImGui::GetFontSize();

        if (column == 1) //2 column
        {
            std::vector<std::string> elems_column;

            std::stringstream pos_column(clear_elems[0].c_str());
            std::string item_pos_column;
            while (std::getline(pos_column, item_pos_column, '\t'))
            {
                elems_column.push_back(item_pos_column);
            }

            int get_pos_id_line = 0;

            for (auto& get_now_line_tab : elems_column)
            {
                if (get_pos_id_line == 0) //1 column
                {
                    TextWithBoev(ImVec2(column0, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                else if (get_pos_id_line == 1) //2 column
                {
                    TextWithBoev(ImVec2(column1, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                get_pos_id_line++;
            }

        }
        else if (column == 2) //3 column
        {
            std::vector<std::string> elems_column;

            std::stringstream pos_column(clear_elems[0].c_str());
            std::string item_pos_column;
            while (std::getline(pos_column, item_pos_column, '\t'))
            {
                elems_column.push_back(item_pos_column);
            }

            int get_pos_id_line = 0;

            for (auto& get_now_line_tab : elems_column)
            {
                if (get_pos_id_line == 0) //1 column
                {
                    TextWithBoev(ImVec2(column0, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                else if (get_pos_id_line == 1) //2 column
                {
                    TextWithBoev(ImVec2(column1, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                else if (get_pos_id_line == 2) //3 column
                {
                    TextWithBoev(ImVec2(column2, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                get_pos_id_line++;
            }
        }
        else if (column == 3) //4 column
        {
            std::vector<std::string> elems_column;

            std::stringstream pos_column(clear_elems[0].c_str());
            std::string item_pos_column;
            while (std::getline(pos_column, item_pos_column, '\t'))
            {
                elems_column.push_back(item_pos_column);
            }

            int get_pos_id_line = 0;

            for (auto& get_now_line_tab : elems_column)
            {
                if (get_pos_id_line == 0) //1 column
                {
                    TextWithBoev(ImVec2(column0, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                else if (get_pos_id_line == 1) //2 column
                {
                    TextWithBoev(ImVec2(column1, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                else if (get_pos_id_line == 2) //3 column
                {
                    TextWithBoev(ImVec2(column2, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                else if (get_pos_id_line == 3) //3 column
                {
                    TextWithBoev(ImVec2(column3, render_tablist_header), ImColor(169, 196, 228, 255), get_now_line_tab.c_str());
                }
                get_pos_id_line++;
            }
        }

        list_left_up.y += ImGui::GetFontSize();

        //remove one line (because its header for tablist)
        clear_elems[0].erase();
        elems[0].erase();

        //obvodka
        ImGui::GetOverlayDrawList()->AddRect(ImVec2(list_left_up.x + space_offset, list_left_up.y + header_size + space_offset), ImVec2(list_right_down.x - space_offset, list_right_down.y - (height_button * 1.5) + header_size), IM_COL32(255, 255, 255, 200), 5.0f);
        //background child
        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, list_right_down.y - (height_button * 1.5) + header_size - 1), IM_COL32(0, 0, 0, 160), 5.0f);

        int get_id = 0;

        //==================================
        //block offset buttons to area
        if (maybe + offset < down_pos_area_list)
        {
            offset = maybe - down_pos_area_list;
            offset -= offset * 2; //negative
        }

        //if (266 + offset > 266)
        if (offset_start_y_orig + offset > offset_start_y_orig)
        {
            offset = 0;
        }
        //==================================

        float scroll_space = 0;
        if (maybe > down_pos_area_list) // > 455
        {
            scroll_space = space_offset;
        }

        ImGui::GetOverlayDrawList()->PushClipRect(ImVec2(list_left_up.x + space_offset + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, list_right_down.y - (height_button * 1.5) + header_size - 1), true);

        for (auto& it : elems)
        {
            const char* cstr = it.c_str();

            //detect id 0, for skip and start tablist to next step;
            if (get_id == 0)
            {
                get_id = 1;
                if(now_id == 0)
                    now_id = 1;
                continue;
            }
			
			std::vector<std::string> elems_get_column;

			std::stringstream pos_get_column(elems_orig[get_id].c_str());
			std::string item_pos_get_column;
			while (std::getline(pos_get_column, item_pos_get_column, '\t'))
			{
				elems_get_column.push_back(item_pos_get_column);
			}

			if(now_id == 0)
			{
				DialogWindowInputHandler(elems_get_column[0].c_str());
			}			

            //if mouse in rect
            //if ((io.MousePos.x > 460 && io.MousePos.x < 820) && (io.MousePos.y > offset_start_y + offset && io.MousePos.y < offset_start_y + 35 + offset))
            if ((io.MousePos.x > list_left_up.x + space_offset && io.MousePos.x < list_right_down.x - space_offset) && (io.MousePos.y > offset_start_y + offset && io.MousePos.y < offset_start_y + step_next_pos_button_list + offset))
            {
                if (now_id == get_id)
                {
					if (ImGui::IsMouseClicked(0) && io.MouseDownDuration[0] >= 0.0f)
					{
						if (io.MousePos.y < list_right_down.y - (height_button * 1.5) + header_size - 1)
						{
							if((GetTickCount() - save_click_time_dialog_now) < time_dbl_test && check_dbl_click_dialog_now)
							{
								//if(pChatWindow) pChatWindow->AddDebugMessage("[two]list test dialog!");						
								
								DialogWindowInputHandler(elems_get_column[0].c_str());	
								
								if(pNetGame) 
									pNetGame->SendDialogResponse(m_wDialogID, 1, now_id-1, szDialogInputBuffer);
								
								check_dbl_click_dialog_now = false;
								
								Show(false);
							}
							else
							{
								save_click_time_dialog_now = GetTickCount();
								check_dbl_click_dialog_now = true;
								
								DialogWindowInputHandler(elems_get_column[0].c_str());	
							}
						}
					}						
					
                    ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                }

                if (ImGui::IsMouseClicked(0) && io.MouseDownDuration[0] >= 0.0f)
                {
                    if (io.MousePos.y < list_right_down.y - (height_button * 1.5) + header_size - 1)
                    {
						if(now_id == get_id)
						{
							if((GetTickCount() - save_click_time_dialog) < time_dbl_test)
							{
								//if(pChatWindow) pChatWindow->AddDebugMessage("list test dialog!");														
								
								DialogWindowInputHandler(elems_get_column[0].c_str());	
								
								if(pNetGame) 
									pNetGame->SendDialogResponse(m_wDialogID, 1, now_id-1, szDialogInputBuffer);
								
								Show(false);
							}
						}
						else
						{
							now_id = get_id;
							save_click_time_dialog = GetTickCount();
							
							DialogWindowInputHandler(elems_get_column[0].c_str());	
                        }						
						
                        ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                    }
                }

                if (column == 1) //2 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }

                }
                else if (column == 2) //3 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
                else if (column == 3) //4 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 3) //3 column
                        {
                            TextWithBoev(ImVec2(column3, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
            }
            else //just render text and if clicked before button id == now id then render background
            {
                if (now_id == get_id)
                {
                    ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_left_up.x + space_offset + space_offset_start_list_button + 1, offset_start_y + offset), ImVec2(list_right_down.x - space_offset - space_offset_start_list_button - scroll_space, offset_start_y + step_next_pos_button_list + offset), ImColor(95, 0, 0, 255));
                }
                //TextWithBoevForeground(ImVec2(470 + 1, offset_start_y + offset + 8), ImColor(255, 255, 255, 255), cstr);
                //TextWithBoev(ImVec2(list_left_up.x + (space_offset * 2), offset_start_y + offset + ((height_button / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), cstr);
                if (column == 1) //2 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }

                }
                else if (column == 2) //3 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
                else if (column == 3) //4 column
                {
                    std::vector<std::string> elems_column;

                    std::stringstream pos_column(it.c_str());
                    std::string item_pos_column;
                    while (std::getline(pos_column, item_pos_column, '\t'))
                    {
                        elems_column.push_back(item_pos_column);
                    }

                    int get_pos_id_line = 0;

                    for (auto& get_now_line_tab : elems_column)
                    {
                        if (get_pos_id_line == 0) //1 column
                        {
                            TextWithBoev(ImVec2(column0, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 1) //2 column
                        {
                            TextWithBoev(ImVec2(column1, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 2) //3 column
                        {
                            TextWithBoev(ImVec2(column2, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        else if (get_pos_id_line == 3) //3 column
                        {
                            TextWithBoev(ImVec2(column3, offset_start_y + offset + ((step_next_pos_button_list / 2) - (ImGui::GetFontSize() / 2))), ImColor(255, 255, 255, 255), get_now_line_tab.c_str());
                        }
                        get_pos_id_line++;
                    }
                }
            }

            //add distance from button to button
            offset_start_y += step_next_pos_button_list + space_offset_start_list_button;

            //next step
            get_id++;
        }

        //if size area buttons > rect area, render custom slider
        float changed_calc = list_right_down.y - (height_button * 1.5) + header_size;

        //down_pos_area_list
        if (offset_start_y > changed_calc && detect_new_long) //455 - 1
        {
            //my slider, okey..
            //where rounding on rect
            ImDrawCornerFlags corners_tl_br = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight;
            //draw rect, background
            ImGui::GetOverlayDrawList()->AddRect(ImVec2(list_right_down.x - (space_offset * 2), list_left_up.y + header_size + space_offset), ImVec2(list_right_down.x, down_pos_area_list), ImColor(255, 255, 255, 190), 5.0f, corners_tl_br);
            //draw rect, main (the left side is already)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 1, list_left_up.y + header_size + space_offset + 1), ImVec2(list_right_down.x - space_offset - 1, changed_calc - 1), ImColor(0, 0, 0, 255), 5.0f, corners_tl_br);
            //draw slider
            if (elements <= 26)
            {
                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 3, list_left_up.y + header_size + space_offset + 3 + (-offset / 2)), ImVec2(list_right_down.x - space_offset - 3, changed_calc - 1 - 3 - ((offset_start_y - changed_calc - 1) / 2) + (-offset / 2)), ImColor(177, 32, 43, 255), 5.0f);
            }
            else
            {
                float razn = elements - 26;

                double test_offset = 2;
                test_offset += (razn * 0.115) + (razn / 500);

                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(list_right_down.x - (space_offset * 2) + 3, list_left_up.y + header_size + space_offset + 3 + (-offset / test_offset)), ImVec2(list_right_down.x - space_offset - 3, changed_calc - 1 - 3 - (((offset_start_y - (razn * (step_next_pos_button_list + space_offset_start_list_button))) - changed_calc - 1) / 2) + (-offset / test_offset)), ImColor(177, 32, 43, 255), 5.0f);
            }
        }
        else
        {
            offset = 0;
        }

        ImGui::GetOverlayDrawList()->PopClipRect();

        //left button
        const char* test_text_left_button = m_utf8Button1;

        std::string new_small_left_button = clear_text(test_text_left_button);

        //right button
        const char* test_text_right_button = m_utf8Button2;

        std::string new_small_right_button = clear_text(test_text_right_button);

        static bool get_clicked = false;

        if (ImGui::IsMouseClicked(0))
        {
            if (io.MousePos.y >= list_right_down.y - (height_button * 1.5) + header_size - 1)
            {
                get_clicked = true;
            }
            else
            {
                get_clicked = false;
            }
        }


        //find NOT NULL button

        if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) > 0)
        {
            //find long text for button
            if (new_small_left_button.length() > new_small_right_button.length()) //if left button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() < new_small_right_button.length()) //if right button is big
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_right_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }
            else if (new_small_left_button.length() == new_small_right_button.length())
            {
                float finded_long_text = ImGui::CalcTextSize(new_small_left_button.c_str()).x;
                float new_size_button = finded_long_text + space_center;
                if (new_size_button > size_button)
                {
                    size_button = size_button;
                }
                else
                {
                    size_button = new_size_button;
                }
            }

            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - space_center - size_button - (get_relation_true_x / 128), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position - space_center, height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position + space_center, height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + space_center + size_button + (get_relation_true_x / 128), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;


            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, now_id-1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, now_id-1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }
        else if (strlen(test_text_left_button) > 0 && strlen(test_text_right_button) == 0) //if LEFT button not null..
        {
            //left button position
            ImVec2 left_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 left_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            left_button_up_angle.x -= anim_b_left;
            left_button_down_angle.x -= anim_b_left;
            left_button_up_angle.y -= anim_b_left;
            left_button_down_angle.y -= anim_b_left;

            //left button
            //background color (white)
            if (!anim_b_left)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(left_button_up_angle.x + 1, left_button_up_angle.y + 1), ImVec2(left_button_down_angle.x - 1, left_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(left_button_up_angle.x + ((left_button_down_angle.x - left_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_left_button.c_str()).x / 2), left_button_up_angle.y + ((left_button_down_angle.y - left_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_left_button);

            if (IsMouseClickInArea(left_button_up_angle.x, left_button_up_angle.y, left_button_down_angle.x, left_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(left_button_up_angle.x, left_button_up_angle.y), ImVec2(left_button_down_angle.x, left_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_left = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 1, now_id-1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_left = 0;
            }

        }
        else if (strlen(test_text_left_button) == 0 && strlen(test_text_right_button) > 0) //if RIGHT button not null..
        {
            //right button position
            ImVec2 right_button_up_angle = ImVec2(center_position - (size_button / 3), height_buttons_up + change_height_pos);
            ImVec2 right_button_down_angle = ImVec2(center_position + (size_button / 3), height_button_down + change_height_pos);

            right_button_up_angle.x -= anim_b_right;
            right_button_down_angle.x -= anim_b_right;
            right_button_up_angle.y -= anim_b_right;
            right_button_down_angle.y -= anim_b_right;

            //right button
            //background color (white)
            if (!anim_b_right)
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(255, 255, 255, 255), 10.0f);
            //main color (dark)
            ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(right_button_up_angle.x + 1, right_button_up_angle.y + 1), ImVec2(right_button_down_angle.x - 1, right_button_down_angle.y - 1), ImColor(0, 0, 0, 255), 10.0f);

            TextWithBoev(ImVec2(right_button_up_angle.x + ((right_button_down_angle.x - right_button_up_angle.x) / 2) - (ImGui::CalcTextSize(new_small_right_button.c_str()).x / 2), right_button_up_angle.y + ((right_button_down_angle.y - right_button_up_angle.y) / 2) - (ImGui::GetFontSize() / 2)), ImColor(255, 255, 255, 255), test_text_right_button);

            if (IsMouseClickInArea(right_button_up_angle.x, right_button_up_angle.y, right_button_down_angle.x, right_button_down_angle.y + 10) && get_clicked)
            {
                ImGui::GetOverlayDrawList()->AddRect(ImVec2(right_button_up_angle.x, right_button_up_angle.y), ImVec2(right_button_down_angle.x, right_button_down_angle.y), ImColor(95, 0, 0, 255), 10.0f);
                anim_b_right = 1;
				
				Show(false);
				if(pNetGame) 
					pNetGame->SendDialogResponse(m_wDialogID, 0, now_id-1, szDialogInputBuffer);				
            }
            else
            {
                anim_b_right = 0;
            }
        }		
	}		
	
}