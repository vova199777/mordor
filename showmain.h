#pragma once

class CShowMain
{
public:
	CShowMain();
	~CShowMain() {};

	void Render();
	void Show(bool bShow) { m_bEnabled = bShow; }

private:
	bool				m_bEnabled;	
	
    bool settings_main;
    bool show_main;

    bool show_keyboard;
    bool show_dialogs;
    bool show_hud;
    bool show_g;	
public:

};