#ifndef MAPBASE_RENDERTARGETS_H
#define MAPBASE_RENDERTARGETS_H
#ifdef _WIN32
#pragma once  
#endif // _WIN32  

#include "baseclientrendertargets.h"

class CMapbaseRenderTargets : public CBaseClientRenderTargets
{
	DECLARE_CLASS(CMapbaseRenderTargets, CBaseClientRenderTargets);
public:
	// Interface called by engine during material system startup.
	virtual void InitClientRenderTargets(IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig);
	// Shutdown all custom render targets here.
	virtual void ShutdownClientRenderTargets(void);

protected:
	// Extra generic fullframe textures
	CTextureReference m_ExtraFullFrameTextures[2];

	CTextureReference m_HologramTexture;

	ITexture* CreateExtraFullFrameTexture(IMaterialSystem* pMaterialSystem, int textureIndex);
	ITexture* CreateHologramTexture(IMaterialSystem* pMaterialSystem);
public:
	ITexture* GetHologramTexture() { return m_HologramTexture; }
};

extern CMapbaseRenderTargets* GetMapbaseRenderTargets();
#endif // !MAPBASE_RENDERTARGETS_H