#pragma once

#include <sstream>
#include <string>

class CKeyboardHistory
{
public:
	CKeyboardHistory();
	~CKeyboardHistory() {};

	void Render();
	void Show(bool bShow) { m_bEnabled = bShow; }

public:
	bool				m_bEnabled;	
	std::string 		OrigHistory[10];
	std::string 		CopyHistory[10];
	int					get_now_pos;
	uint32_t			get_time_use;
	
};