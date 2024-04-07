#pragma once
#include "game/RW/RenderWare.h"

class CSeatCar
{
public:
	CSeatCar();
	~CSeatCar() {};

	void Render();
	void Show(bool bShow) { m_bEnabled = bShow; }

private:
	CPlayerPed			*m_pPlayerPed;
	bool				m_bEnabled;	
	uint32_t 			m_dwPassengerEnterExit;
public:
	RwTexture*          m_pSeatInCar; 
};