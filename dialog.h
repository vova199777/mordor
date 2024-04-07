#pragma once

#define DIALOG_STYLE_MSGBOX				0
#define DIALOG_STYLE_INPUT				1
#define DIALOG_STYLE_LIST				2
#define DIALOG_STYLE_PASSWORD			3
#define DIALOG_STYLE_TABLIST			4
#define DIALOG_STYLE_TABLIST_HEADERS	5

class CDialogWindow
{
public:
	CDialogWindow();
	~CDialogWindow();

	void Render();
	void Clear();
	void Show(bool bShow);
	void SetInfo(char* szInfo, int length);

	void DrawFormattedText();
	
	bool IsDialogOpen() { return m_bIsActive; }

public:
	bool		m_bIsActive;
	uint8_t 	m_byteDialogStyle;
	uint16_t	m_wDialogID;
	char		m_utf8Title[64*3 + 1];
	char*		m_putf8Info;
	char*		m_putf8Info_orig;
	char* 		m_pszInfo;
	char		m_utf8Button1[64*3 + 1];
	char		m_utf8Button2[64*3 + 1];

	float		m_fSizeX;
	float 		m_fSizeY;
	
	bool 		m_DetectKeyBoard;
	
	bool		m_StylePassword;
	
private:
	//from VS adaptations
	
	float anim_b_left;
	float anim_b_center;
	float anim_b_right;

	int now_id;
	
	int time_dbl_test;
	uint32_t save_click_time_dialog;
	uint32_t save_click_time_dialog_now;
	bool check_dbl_click_dialog_now;
	
};