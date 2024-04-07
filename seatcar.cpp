#include "main.h"
#include "gui/gui.h"
#include "game/game.h"
#include "seatcar.h"
#include "chatwindow.h"

#include "game/game.h"
#include "net/netgame.h"
#include "util/armhook.h"

//for check exists
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;


bool exists_test(const std::string& name) 
{
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

CSeatCar::CSeatCar()
{	
	m_pPlayerPed = pGame->FindPlayerPed();
	
	if(!RtPNGImageRead(BOEV("./SAMP/button_g.png")))
	{
		Log("File not found: /SAMP/button_g not load | Find please and fix!");
		std::terminate();
	}
	
	RwImage* hi_png = RtPNGImageRead(BOEV("./SAMP/button_g.png"));
	
	RwInt32 width, height, depth, flags;

	RwImageFindRasterFormat(hi_png, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);

	RwRaster *raster = RwRasterCreate(width, height, depth, flags);
	
	if(width != 256)
	{
		Log("Texture: button_g.png | Please, load image to size 256x256");
		std::terminate();		
	}
	
	if(height != 256)
	{
		Log("Texture: button_g.png | Please, load image to size 256x256");
		std::terminate();		
	}	

	RwRasterSetFromImage(raster, hi_png);

	RwImageDestroy(hi_png);
	
	m_pSeatInCar = RwTextureCreate(raster);	

	m_bEnabled = false;
	m_dwPassengerEnterExit = GetTickCount();
}

bool IsMouseClickInButton(float x1, float y1, float x2, float y2)
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

void CSeatCar::Render()
{
	if(!m_bEnabled) return;
	
	lastfunc("sc1");
	
	ImGuiIO& io = ImGui::GetIO();
	
	float get_plus_size = (io.DisplaySize.y / 1.7061) - (io.DisplaySize.y / 2.2360);
	
	// if player is freezed
	if(pGame->m_freezed == false) return;
	
	if(IsMouseClickInButton(io.DisplaySize.x / 1.2319, io.DisplaySize.y / 2.2360, io.DisplaySize.x / 1.2319 + get_plus_size, io.DisplaySize.y / 1.7061))
	{
		ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pSeatInCar->raster, ImVec2(io.DisplaySize.x / 1.2319 + 5, io.DisplaySize.y / 2.2360 + 5),  ImVec2((io.DisplaySize.x / 1.2319 + get_plus_size) - 5, io.DisplaySize.y / 1.7061 - 5));
		
		if(GetTickCount() - m_dwPassengerEnterExit < 1000 )
			return;
		
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

		VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
		if(ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
		{
			CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
			if(pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f)
			{		
				m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
				
				CLocalPlayer *pLocalPlayer;
				pLocalPlayer->SendEnterVehicleNotification(ClosetVehicleID, true);

				m_dwPassengerEnterExit = GetTickCount();
			}
		}		
	}
	else
	{
		ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pSeatInCar->raster, ImVec2(io.DisplaySize.x / 1.2319, io.DisplaySize.y / 2.2360),  ImVec2(io.DisplaySize.x / 1.2319 + get_plus_size, io.DisplaySize.y / 1.7061));
	}
	
	m_bEnabled = false;
}