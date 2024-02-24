#include "cbase.h"
#include "mapbase_rendertargets.h"

void CMapbaseRenderTargets::InitClientRenderTargets(IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig)
{
	BaseClass::InitClientRenderTargets(pMaterialSystem, pHardwareConfig);

	for (int i = 0; i < MAX_FB_TEXTURES - 2; i++)
	{
		m_ExtraFullFrameTextures[i].Init(CreateExtraFullFrameTexture(pMaterialSystem, i + 2));
	}

	if (pHardwareConfig->HasStencilBuffer())
		m_HologramTexture.Init(CreateHologramTexture(pMaterialSystem));
}

void CMapbaseRenderTargets::ShutdownClientRenderTargets(void)
{
	BaseClass::ShutdownClientRenderTargets();

	for (int i = 0; i < MAX_FB_TEXTURES - 2; i++)
	{
		m_ExtraFullFrameTextures[i].Shutdown();
	}

	m_HologramTexture.Shutdown();
}

ITexture* CMapbaseRenderTargets::CreateExtraFullFrameTexture(IMaterialSystem* pMaterialSystem, int textureIndex)
{
	char name[256];
	if (textureIndex != 0)
	{
		sprintf(name, "_rt_FullFrameFB%d", textureIndex);
	}
	else
	{
		Q_strcpy(name, "_rt_FullFrameFB");
	}

	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		name,
		1, 1, RT_SIZE_FULL_FRAME_BUFFER,
		pMaterialSystem->GetBackBufferFormat(),
		MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		CREATERENDERTARGETFLAGS_HDR);
}

ITexture* CMapbaseRenderTargets::CreateHologramTexture(IMaterialSystem* pMaterialSystem)
{
	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		"_rt_Hologram", 1, 1, RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP,
		pMaterialSystem->GetBackBufferFormat(),
		MATERIAL_RT_DEPTH_SEPARATE,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		CREATERENDERTARGETFLAGS_HDR);
}

CMapbaseRenderTargets g_MapbaseRenderTargets;
IClientRenderTargets* g_pClientRenderTargets = &g_MapbaseRenderTargets;

CMapbaseRenderTargets* GetMapbaseRenderTargets()
{
	return assert_cast<CMapbaseRenderTargets*> (g_pClientRenderTargets);
}

static void* GetClientRenderTargetsInterface() { return g_pClientRenderTargets; }
EXPOSE_INTERFACE_FN(GetClientRenderTargetsInterface, IClientRenderTargets, CLIENTRENDERTARGETS_INTERFACE_VERSION);