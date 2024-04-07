#include "cbase.h"
#include "screenspaceeffects.h"
#include "rendertexture.h"
#include "model_types.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imaterialvar.h"
#include "cdll_client_int.h"
#include "materialsystem/itexture.h"
#include "keyvalues.h"
#include "ClientEffectPrecacheSystem.h"
#include "viewrender.h"
#include "view_scene.h"
#include "c_basehlplayer.h"
#include "tier0/vprof.h"
#include "view.h"
#include "hl2_gamerules.h"

#include "deferred_screenspace_effects.h"
#include "deferred_rt.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar mat_specular( "mat_specular", "1", FCVAR_CHEAT );

//-----------------------------------------------------------------------------
// Specifically for Deferred lighting pass
//-----------------------------------------------------------------------------
static void DrawLightingPass(IMaterial* pMaterial, int x, int y, int w, int h, bool shouldScale = false)
{
	ITexture* pTexture = GetFullFrameFrameBufferTexture(0);
	UpdateScreenEffectTexture(0, x, y, w, h, false);

	CMatRenderContextPtr pRenderContext(materials);

	pRenderContext->DrawScreenSpaceRectangle(pMaterial, x, y, w * (shouldScale ? (pTexture->GetActualWidth() / w) : 1), h * (shouldScale ? (pTexture->GetActualHeight() / h) : 1),
		x, y, x + w - 1, y + h - 1,
		w, h);
}

static void SetRenderTargetAndViewPort(ITexture* rt)
{
	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->SetRenderTarget(rt);
	pRenderContext->Viewport(0, 0, rt->GetActualWidth(), rt->GetActualHeight());
}

void CFXAA::Init(void)
{
	PrecacheMaterial("shaders/fxaa_luma");
	PrecacheMaterial("shaders/fxaa");

	m_Luma.Init(materials->FindMaterial("shaders/fxaa_luma", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_FXAA.Init(materials->FindMaterial("shaders/fxaa", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CFXAA::Shutdown(void)
{
	m_Luma.Shutdown();
	m_FXAA.Shutdown();
}

ConVar r_post_fxaa("r_post_fxaa", "1", FCVAR_ARCHIVE);
ConVar r_post_fxaa_quality("r_post_fxaa_quality", "4", FCVAR_ARCHIVE, "0 = Very Low, 1 = Low, 2 = Medium, 3 = High, 4 = Very High", true, 0, true, 4);
void CFXAA::Render(int x, int y, int w, int h)
{
	VPROF("CFXAA::Render");

	if (!r_post_fxaa.GetBool() || (IsEnabled() == false))
		return;

	IMaterialVar *var;
	var = m_FXAA->FindVar("$QUALITY", NULL);
	var->SetIntValue(r_post_fxaa_quality.GetInt());

	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->OverrideDepthEnable(true, false);
	DrawScreenEffectMaterial(m_Luma, x, y, w, h);
	DrawScreenEffectMaterial(m_FXAA, x, y, w, h);
	pRenderContext->OverrideDepthEnable(false, true);
}

void CTonemap::Init(void)
{
	PrecacheMaterial("shaders/tonemap");

	m_Tonemap.Init(materials->FindMaterial("shaders/tonemap", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CTonemap::Shutdown(void)
{
	m_Tonemap.Shutdown();
}

ConVar r_post_tonemap("r_post_tonemap", "1", FCVAR_ARCHIVE);
void CTonemap::Render(int x, int y, int w, int h)
{
	VPROF("CFXAA::Render");

	if (!r_post_tonemap.GetBool() || (IsEnabled() == false))
		return;

	DrawScreenEffectMaterial(m_Tonemap, x, y, w, h);
}

class CEyeAdaption : public IScreenSpaceEffect
{
public:
	CEyeAdaption( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_EyeAdaption;
};

// Eye adaption
ADD_SCREENSPACE_EFFECT( CEyeAdaption, Eye_Adaption );

void CEyeAdaption::Init( void )
{
	PrecacheMaterial( "shaders/eyeadaption" );

	m_EyeAdaption.Init( materials->FindMaterial( "shaders/eyeadaption", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CEyeAdaption::Shutdown( void )
{
	m_EyeAdaption.Shutdown();
}

ConVar r_post_eyeadaption( "r_post_eyeadaption", "1", FCVAR_ARCHIVE );
void CEyeAdaption::Render( int x, int y, int w, int h )
{
	VPROF( "CEYEADAPTION::Render" );

	if ( !r_post_eyeadaption.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_EyeAdaption, x, y, w, h );
}

class CEyeDownsampling : public IScreenSpaceEffect
{
public:
	CEyeDownsampling( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_DownSampling;
};

// Eye adaption
ADD_SCREENSPACE_EFFECT( CEyeDownsampling, Down_Sampling );

void CEyeDownsampling::Init( void )
{
	PrecacheMaterial( "shaders/downsampling" );

	m_DownSampling.Init( materials->FindMaterial( "shaders/downsampling", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CEyeDownsampling::Shutdown( void )
{
	m_DownSampling.Shutdown();
}

ConVar r_post_downsampling( "r_post_downsampling", "1", FCVAR_ARCHIVE );
void CEyeDownsampling::Render( int x, int y, int w, int h )
{
	VPROF( "CEYEDOWNSAMPLING::Render" );

	if ( !r_post_eyeadaption.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_DownSampling, x, y, w, h );
}

class CDithering : public IScreenSpaceEffect
{
public:
	CDithering( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Dithering;
};

// Dithering
ADD_SCREENSPACE_EFFECT( CDithering, Dithering );

void CDithering::Init( void )
{
	PrecacheMaterial( "shaders/dithering" );

	m_Dithering.Init( materials->FindMaterial( "shaders/dithering", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CDithering::Shutdown( void )
{
	m_Dithering.Shutdown();
}

ConVar r_post_dithering( "r_post_dithering", "0", FCVAR_ARCHIVE );
void CDithering::Render( int x, int y, int w, int h )
{
	VPROF( "CDITHERING::Render" );

	if ( !r_post_dithering.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Dithering, x, y, w, h );
}

class CToon : public IScreenSpaceEffect
{
public:
	CToon( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Toon;
};

// Toon shading
ADD_SCREENSPACE_EFFECT( CToon, Toon );

void CToon::Init( void )
{
	PrecacheMaterial( "shaders/toon" );

	m_Toon.Init( materials->FindMaterial( "shaders/toon", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CToon::Shutdown( void )
{
	m_Toon.Shutdown();
}

ConVar r_post_toonshade( "r_post_toonshade", "0", FCVAR_ARCHIVE );
void CToon::Render( int x, int y, int w, int h )
{
	VPROF( "CTOON::Render" );

	if ( !r_post_toonshade.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Toon, x, y, w, h );
}

class CMainSunshaft: public IScreenSpaceEffect
{
public:
	CMainSunshaft( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Mainsunshaft;
};

// Sunshaft
ADD_SCREENSPACE_EFFECT( CMainSunshaft, Mainsunshaft );

void CMainSunshaft::Init( void )
{
	PrecacheMaterial( "shaders/mainsunshaft" );

	m_Mainsunshaft.Init( materials->FindMaterial( "shaders/mainsunshaft", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CMainSunshaft::Shutdown( void )
{
	m_Mainsunshaft.Shutdown();
}

ConVar r_post_mainsunshaft( "r_post_mainsunshaft", "0", FCVAR_ARCHIVE );
void CMainSunshaft::Render( int x, int y, int w, int h )
{
	VPROF( "CMAINSUNSHAFT::Render" );

	if ( !r_post_mainsunshaft.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Mainsunshaft, x, y, w, h );
}

class CBARRELDIS : public IScreenSpaceEffect
{
public:
	CBARRELDIS( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Barreldis;
};

// Barrel distortion
ADD_SCREENSPACE_EFFECT( CBARRELDIS, Barreldistortion );

void CBARRELDIS::Init( void )
{
	PrecacheMaterial( "shaders/barreldistortion" );

	m_Barreldis.Init( materials->FindMaterial( "shaders/barreldistortion", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CBARRELDIS::Shutdown( void )
{
	m_Barreldis.Shutdown();
}

ConVar r_post_barreldistortion( "r_post_barreldistortion", "0", FCVAR_ARCHIVE );
void CBARRELDIS::Render( int x, int y, int w, int h )
{
	VPROF( "CBARRELDISTORTION::Render" );

	if ( !r_post_barreldistortion.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Barreldis, x, y, w, h );
}

class CRADIALBLUR : public IScreenSpaceEffect
{
public:
	CRADIALBLUR( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Radialblur;
};

// Toon shading
ADD_SCREENSPACE_EFFECT( CRADIALBLUR, Radialblur );

void CRADIALBLUR::Init( void )
{
	PrecacheMaterial( "shaders/radialblur" );

	m_Radialblur.Init( materials->FindMaterial( "shaders/radialblur", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CRADIALBLUR::Shutdown( void )
{
	m_Radialblur.Shutdown();
}

ConVar r_post_radialblur( "r_post_radialblur", "0", FCVAR_ARCHIVE );
void CRADIALBLUR::Render( int x, int y, int w, int h )
{
	VPROF( "CRADIALBLUR::Render" );

	if ( !r_post_radialblur.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Radialblur, x, y, w, h );
}

class CASCII : public IScreenSpaceEffect
{
public:
	CASCII( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Ascii;
};

// Ascii
ADD_SCREENSPACE_EFFECT( CASCII, Ascii );

void CASCII::Init( void )
{
	PrecacheMaterial( "shaders/ascii" );

	m_Ascii.Init( materials->FindMaterial( "shaders/ascii", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CASCII::Shutdown( void )
{
	m_Ascii.Shutdown();
}

ConVar r_post_ascii( "r_post_ascii", "0", FCVAR_ARCHIVE );
void CASCII::Render( int x, int y, int w, int h )
{
	VPROF( "CASCII::Render" );

	if ( !r_post_ascii.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Ascii, x, y, w, h );
}

class CHeathaze : public IScreenSpaceEffect
{
public:
	CHeathaze( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Heathaze;
};

// Heat Haze
ADD_SCREENSPACE_EFFECT( CHeathaze, heathaze );

void CHeathaze::Init( void )
{
	PrecacheMaterial( "shaders/heathaze" );

	m_Heathaze.Init( materials->FindMaterial( "shaders/heathaze", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CHeathaze::Shutdown( void )
{
	m_Heathaze.Shutdown();
}

ConVar r_post_heathaze( "r_post_heathaze", "0", FCVAR_ARCHIVE );
void CHeathaze::Render( int x, int y, int w, int h )
{
	VPROF( "CHEATHAZE::Render" );

	if ( !r_post_heathaze.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Heathaze, x, y, w, h );
}

class CBlindness : public IScreenSpaceEffect
{
public:
	CBlindness( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Blindness;
};

// Heat Haze
ADD_SCREENSPACE_EFFECT( CBlindness, blindness );

void CBlindness::Init( void )
{
	PrecacheMaterial( "shaders/blindness" );

	m_Blindness.Init( materials->FindMaterial( "shaders/heathaze", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CBlindness::Shutdown( void )
{
	m_Blindness.Shutdown();
}

ConVar r_post_blindness( "r_post_blindness", "0", FCVAR_ARCHIVE );
void CBlindness::Render( int x, int y, int w, int h )
{
	VPROF( "CBLINDESS::Render" );

	if ( !r_post_blindness.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_Blindness, x, y, w, h );
}

void CSSAO::Init(void)
{
	PrecacheMaterial("shaders/ssgi");
	PrecacheMaterial("shaders/ssao_bilateralx");
	PrecacheMaterial("shaders/ssao_bilateraly");
	PrecacheMaterial("shaders/ssgi_combine");

	m_Normal.Init("_rt_Normals", TEXTURE_GROUP_RENDER_TARGET);
	m_SSAO.InitRenderTarget(ScreenWidth() / 2, ScreenHeight() / 2, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_SSAOFB");
	m_SSAOX.InitRenderTarget(ScreenWidth(), ScreenHeight(), RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_SSAOFBX");
	m_SSAOY.InitRenderTarget(ScreenWidth(), ScreenHeight(), RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_SSAOFBY");

	m_SSAO_Mat.Init(materials->FindMaterial("shaders/ssgi", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_SSAO_BilateralX.Init(materials->FindMaterial("shaders/ssao_bilateralx", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_SSAO_BilateralY.Init(materials->FindMaterial("shaders/ssao_bilateraly", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_SSAO_Combine.Init(materials->FindMaterial("shaders/ssgi_combine", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CSSAO::Shutdown(void)
{
	m_SSAO.Shutdown();
}

ConVar r_post_ssao("r_post_ssao", "0", FCVAR_ARCHIVE);
void CSSAO::Render(int x, int y, int w, int h)
{
	VPROF("CFXAA::Render");

	if (!r_post_ssao.GetBool() || (IsEnabled() == false))
		return;
	IMaterialVar* var;
	CMatRenderContextPtr pRenderContext(materials);

	UpdateScreenEffectTexture(0, x, y, w, h, false);
	pRenderContext->PushRenderTargetAndViewport(m_SSAO);
	DrawScreenEffectQuad(m_SSAO_Mat, m_SSAO->GetActualWidth(), m_SSAO->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	UpdateScreenEffectTexture(0, x, y, w, h, false);
	pRenderContext->PushRenderTargetAndViewport(m_SSAOX);
	DrawScreenEffectQuad(m_SSAO_BilateralX, m_SSAOX->GetActualWidth(), m_SSAOX->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	UpdateScreenEffectTexture(0, x, y, w, h, false);
	pRenderContext->PushRenderTargetAndViewport(m_SSAOY);
	DrawScreenEffectQuad(m_SSAO_BilateralY, m_SSAOY->GetActualWidth(), m_SSAOY->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	var = m_SSAO_Combine->FindVar("$AMOUNT", NULL);
	var->SetFloatValue(1.0f);
	DrawScreenEffectMaterial(m_SSAO_Combine, x, y, w, h);

	pRenderContext.SafeRelease();
}

void CUnsharpEffect::Init(void)
{
	m_UnsharpBlurFB.InitRenderTarget(ScreenWidth() / 2, ScreenHeight() / 2, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_UnsharpBlur");

	PrecacheMaterial("shaders/unsharp_blur");
	PrecacheMaterial("shaders/unsharp");

	m_UnsharpBlur.Init(materials->FindMaterial("shaders/unsharp_blur", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_Unsharp.Init(materials->FindMaterial("shaders/unsharp", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CUnsharpEffect::Shutdown(void)
{
	m_UnsharpBlurFB.Shutdown();
	m_UnsharpBlur.Shutdown();
	m_Unsharp.Shutdown();
}

ConVar r_post_unsharp("r_post_unsharp", "1", FCVAR_ARCHIVE);
ConVar r_post_unsharp_debug("r_post_unsharp_debug", "0", FCVAR_CHEAT);
ConVar r_post_unsharp_strength("r_post_unsharp_strength", "0.3", FCVAR_CHEAT);
ConVar r_post_unsharp_blursize("r_post_unsharp_blursize", "5.0", FCVAR_CHEAT);
void CUnsharpEffect::Render(int x, int y, int w, int h)
{
	VPROF("CUnsharpEffect::Render");

	if (!r_post_unsharp.GetBool() || (IsEnabled() == false))
		return;

	// Grab the render context
	CMatRenderContextPtr pRenderContext(materials);

	// Set to the proper rendering mode.
	pRenderContext->MatrixMode(MATERIAL_VIEW);
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();
	pRenderContext->MatrixMode(MATERIAL_PROJECTION);
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	IMaterialVar *var;
	var = m_UnsharpBlur->FindVar("$blursize", NULL);
	var->SetFloatValue(r_post_unsharp_blursize.GetFloat());

	if (r_post_unsharp_debug.GetBool())
	{
		DrawScreenEffectMaterial(m_UnsharpBlur, x, y, w, h);
		return;
	}

	Rect_t actualRect;
	UpdateScreenEffectTexture(0, x, y, w, h, false, &actualRect);
	pRenderContext->PushRenderTargetAndViewport(m_UnsharpBlurFB);
	DrawScreenEffectQuad(m_UnsharpBlur, m_UnsharpBlurFB->GetActualWidth(), m_UnsharpBlurFB->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	//Restore our state
	pRenderContext->MatrixMode(MATERIAL_VIEW);
	pRenderContext->PopMatrix();
	pRenderContext->MatrixMode(MATERIAL_PROJECTION);
	pRenderContext->PopMatrix();

	var = m_Unsharp->FindVar("$fbblurtexture", NULL);
	var->SetTextureValue(m_UnsharpBlurFB);
	var = m_Unsharp->FindVar("$unsharpstrength", NULL);
	var->SetFloatValue(r_post_unsharp_strength.GetFloat());
	var = m_Unsharp->FindVar("$blursize", NULL);
	var->SetFloatValue(r_post_unsharp_blursize.GetFloat());

	DrawScreenEffectMaterial(m_Unsharp, x, y, w, h);
}

ConVar r_post_watereffects_underwater_chromaticoffset("r_post_watereffects_underwater_chromaticoffset", "1.0", FCVAR_CHEAT);
ConVar r_post_watereffects_underwater_amount("r_post_watereffects_underwater_amount", "0.1", FCVAR_CHEAT);
ConVar r_post_watereffects_underwater_viscosity("r_post_watereffects_underwater_viscosity", "1.0", FCVAR_CHEAT);
ConVar r_post_watereffects_lerp_viscosity("r_post_watereffects_lerp_viscosity", "0.01", FCVAR_CHEAT);
ConVar r_post_watereffects_lerp_amount("r_post_watereffects_lerp_amount", "0.005", FCVAR_CHEAT);
ConVar r_post_watereffects_underwater_gaussianamount("r_post_watereffects_underwater_gaussianamount", "1.5", FCVAR_CHEAT);
void CWaterEffects::Init(void)
{
	fViscosity = 0.01;
	fAmount = 0;
	m_bUnderwater = false;

	PrecacheMaterial("shaders/chromaticDisp");
	PrecacheMaterial("shaders/screenwater");
	PrecacheMaterial("shaders/screen_blurx");
	PrecacheMaterial("shaders/screen_blury");

	m_ChromaticDisp.Init(materials->FindMaterial("shaders/chromaticDisp", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_WaterFX.Init(materials->FindMaterial("shaders/screenwater", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_BlurX.Init(materials->FindMaterial("shaders/screen_blurx", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_BlurY.Init(materials->FindMaterial("shaders/screen_blury", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CWaterEffects::Shutdown(void)
{
	m_ChromaticDisp.Shutdown();
	m_WaterFX.Shutdown();
	m_BlurX.Shutdown();
	m_BlurY.Shutdown();
}

void CWaterEffects::SetParameters(KeyValues *params)
{
	if (IsUnderwater())
		return;

	float in, temp;

	if (params->FindKey("amount"))
	{
		in = params->GetFloat("amount");
		temp = GetAmount();
		temp += in;
		if (temp > 0.1f)
			temp = 0.1f;

		SetAmount(temp);
	}

	if (params->FindKey("viscosity"))
	{
		in = params->GetFloat("viscosity");
		temp = GetViscosity();
		temp += in;
		if (temp > 1.0f)
			temp = 1.0f;

		SetViscosity(temp);
	}
}

ConVar r_post_watereffects("r_post_watereffects", "1", FCVAR_ARCHIVE);
ConVar r_post_watereffects_debug("r_post_watereffects_debug", "0", FCVAR_CHEAT);
void CWaterEffects::Render(int x, int y, int w, int h)
{
	VPROF("CWaterEffects::Render");

	if (!r_post_watereffects.GetBool() || (IsEnabled() == false))
		return;

	C_BaseHLPlayer *pPlayer = (C_BaseHLPlayer *)C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;

	IMaterialVar *var;

	if (pPlayer->GetWaterLevel() >= 3)
	{
		m_bUnderwater = true;
		fViscosity = r_post_watereffects_underwater_viscosity.GetFloat();
		fAmount = r_post_watereffects_underwater_amount.GetFloat();

		//Gaussian Blur the screen
		var = m_BlurX->FindVar("$BLURSIZE", NULL);
		var->SetFloatValue(r_post_watereffects_underwater_gaussianamount.GetFloat());
		var = m_BlurX->FindVar("$RESDIVISOR", NULL);
		var->SetIntValue(1);
		DrawScreenEffectMaterial(m_BlurX, x, y, w, h);
		var = m_BlurY->FindVar("$BLURSIZE", NULL);
		var->SetFloatValue(r_post_watereffects_underwater_gaussianamount.GetFloat());
		var = m_BlurY->FindVar("$RESDIVISOR", NULL);
		var->SetIntValue(1);
		DrawScreenEffectMaterial(m_BlurY, x, y, w, h);

		//Render Chromatic Dispersion
		var = m_ChromaticDisp->FindVar("$FOCUSOFFSET", NULL);
		var->SetFloatValue(r_post_watereffects_underwater_chromaticoffset.GetFloat());
		var = m_ChromaticDisp->FindVar("$radial", NULL);
		var->SetIntValue(0);
		DrawScreenEffectMaterial(m_ChromaticDisp, x, y, w, h);
	}
	else
	{
		m_bUnderwater = false;

		if (fViscosity != 0.01)
			fViscosity = FLerp(fViscosity, 0.01, r_post_watereffects_lerp_viscosity.GetFloat());

		if (fAmount != 0)
			fAmount = FLerp(fAmount, 0, r_post_watereffects_lerp_amount.GetFloat());

		if (fAmount < 0.01)
		{
			if (r_post_watereffects_debug.GetBool())
			{
				DevMsg("Water Effects Stopped.\n");
			}
			return;
		}
	}

	var = m_WaterFX->FindVar("$AMOUNT", NULL);
	var->SetFloatValue(fAmount);
	var = m_WaterFX->FindVar("$VISCOSITY", NULL);
	var->SetFloatValue(fViscosity);
	DrawScreenEffectMaterial(m_WaterFX, x, y, w, h);

	if (r_post_watereffects_debug.GetBool())
	{
		DevMsg("Water Amount: %.2f\n", fAmount);
		DevMsg("Water Viscosity: %.2f\n", fViscosity);
	}
}

extern void Generate8BitBloomTexture(IMatRenderContext *pRenderContext, float flBloomScale, int x, int y, int w, int h);
extern float GetBloomAmount();

void CBloom::Init(void)
{
	PrecacheMaterial("shaders/bloom_sample");
	PrecacheMaterial("shaders/bloom_gaussianx");
	PrecacheMaterial("shaders/bloom_gaussiany");
	PrecacheMaterial("shaders/bloom_combine");
	PrecacheMaterial("shaders/bloom_downsample");

	m_BloomDS.InitRenderTarget(ScreenWidth() / 2, ScreenHeight() / 2, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA16161616F, MATERIAL_RT_DEPTH_NONE, false, "_rt_BloomDS");
	m_BloomDS1.InitRenderTarget(ScreenWidth() / 4, ScreenHeight() / 4, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA16161616F, MATERIAL_RT_DEPTH_NONE, false, "_rt_BloomDS1");
	m_BloomDS2.InitRenderTarget(ScreenWidth() / 8, ScreenHeight() / 8, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA16161616F, MATERIAL_RT_DEPTH_NONE, false, "_rt_BloomDS2");
	m_BloomDS3.InitRenderTarget(ScreenWidth() / 16, ScreenHeight() / 16, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA16161616F, MATERIAL_RT_DEPTH_NONE, false, "_rt_BloomDS3");

	m_BloomFB0.InitRenderTarget(ScreenWidth() / 2, ScreenHeight() / 2, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA16161616F, MATERIAL_RT_DEPTH_NONE, false, "_rt_BloomFB0");
	m_BloomFB1.InitRenderTarget(ScreenWidth() / 2, ScreenHeight() / 2, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA16161616F, MATERIAL_RT_DEPTH_NONE, false, "_rt_BloomFB1");

	m_BloomSample.Init(materials->FindMaterial("shaders/bloom_sample", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_GaussianX.Init(materials->FindMaterial("shaders/bloom_gaussianx", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_GaussianY.Init(materials->FindMaterial("shaders/bloom_gaussiany", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_BloomDownsample.Init(materials->FindMaterial("shaders/bloom_downsample", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_BloomCombine.Init(materials->FindMaterial("shaders/bloom_combine", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CBloom::Shutdown(void)
{
	m_BloomFB0.Shutdown();
	m_BloomFB1.Shutdown();
	m_BloomDS.Shutdown();
	m_BloomDS1.Shutdown();
	m_BloomDS2.Shutdown();
	m_BloomDS3.Shutdown();

	m_BloomSample.Shutdown();
	m_GaussianX.Shutdown();
	m_GaussianY.Shutdown();
	m_BloomDownsample.Shutdown();
	m_BloomCombine.Shutdown();
}

ConVar r_post_bloom("r_post_bloom", "1", FCVAR_ARCHIVE);
ConVar r_post_bloom_amount("r_post_bloom_amount", "0.2", FCVAR_ARCHIVE);
ConVar r_post_bloom_gaussianamount("r_post_bloom_gaussianamount", "1", FCVAR_ARCHIVE);
#ifdef DXVK
ConVar r_post_bloom_exposure( "r_post_bloom_exposure", "0.3", FCVAR_ARCHIVE );
#else
ConVar r_post_bloom_exposure("r_post_bloom_exposure", "1", FCVAR_ARCHIVE);
#endif
void CBloom::Render(int x, int y, int w, int h)
{
	VPROF("CBloom::Render");

	if (!r_post_bloom.GetBool() || (IsEnabled() == false))
		return;

	IMaterialVar *var;
	CMatRenderContextPtr pRenderContext(materials);

	Generate8BitBloomTexture(pRenderContext, GetBloomAmount(), x, y, w, h);

	//Gaussian Blur the screen
	var = m_GaussianX->FindVar("$BLURSIZE", NULL);
	var->SetFloatValue(r_post_bloom_gaussianamount.GetFloat());
	var = m_GaussianY->FindVar("$BLURSIZE", NULL);
	var->SetFloatValue(r_post_bloom_gaussianamount.GetFloat());

	var = m_BloomSample->FindVar("$C1_X", NULL);
	var->SetFloatValue(r_post_bloom_exposure.GetFloat());

	pRenderContext->PushRenderTargetAndViewport();
	SetRenderTargetAndViewPort(m_BloomDS);
	DrawScreenEffectQuad(m_BloomSample, m_BloomDS->GetActualWidth(), m_BloomDS->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	pRenderContext->PushRenderTargetAndViewport();
	SetRenderTargetAndViewPort(m_BloomFB0);
	DrawScreenEffectQuad(m_GaussianX, m_BloomFB0->GetActualWidth(), m_BloomFB0->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	pRenderContext->PushRenderTargetAndViewport();
	SetRenderTargetAndViewPort(m_BloomFB1);
	DrawScreenEffectQuad(m_GaussianY, m_BloomFB1->GetActualWidth(), m_BloomFB1->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	var = m_BloomDownsample->FindVar("$C1_X", NULL);
	var->SetFloatValue(1.0f);

	pRenderContext->PushRenderTargetAndViewport();
	SetRenderTargetAndViewPort(m_BloomDS1);
	DrawScreenEffectQuad(m_BloomDownsample, m_BloomDS1->GetActualWidth(), m_BloomDS1->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	pRenderContext->PushRenderTargetAndViewport();
	SetRenderTargetAndViewPort(m_BloomDS2);
	DrawScreenEffectQuad(m_BloomDownsample, m_BloomDS2->GetActualWidth(), m_BloomDS2->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	pRenderContext->PushRenderTargetAndViewport();
	SetRenderTargetAndViewPort(m_BloomDS3);
	DrawScreenEffectQuad(m_BloomDownsample, m_BloomDS3->GetActualWidth(), m_BloomDS3->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	var = m_BloomCombine->FindVar("$AMOUNT", NULL);
	var->SetFloatValue(GetBloomAmount() * r_post_bloom_amount.GetFloat());
	DrawScreenEffectMaterial(m_BloomCombine, x, y, w, h);
}

void CVolumetrics::Init(void)
{
	PrecacheMaterial("shaders/volumetrics_combine");
	PrecacheMaterial("shaders/volumetrics_downsample");

	//m_VolumetricsFB0.InitRenderTarget(ScreenWidth() / 16, ScreenHeight() / 16, RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_VolumetricsFB0");
	m_VolumetricsFB0.Init( materials->FindTexture("_rt_VolumetricsBuffer", TEXTURE_GROUP_RENDER_TARGET) );
	m_VolumetricsCombine.Init(materials->FindMaterial("shaders/volumetrics_combine", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_VolumetricsSample.Init(materials->FindMaterial("shaders/volumetrics_downsample", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CVolumetrics::Shutdown(void)
{
	m_VolumetricsFB0.Shutdown();
	m_VolumetricsCombine.Shutdown();
}

extern ConVar r_volumetrics;
void CVolumetrics::Render(int x, int y, int w, int h)
{
	VPROF("CVolumetrics::Render");

	if (!r_volumetrics.GetBool() || (IsEnabled() == false))
		return;

	CMatRenderContextPtr pRenderContext(materials);

	IMaterialVar* var = m_VolumetricsCombine->FindVar("$C1_X", NULL);
	var->SetFloatValue(1.0f);

	DrawScreenEffectMaterial(m_VolumetricsCombine, x, y, w, h);
}

void CSSR::Init(void)
{
	PrecacheMaterial("shaders/SSR");
	PrecacheMaterial("shaders/ssr_add");

	m_SSR.InitRenderTarget( ScreenWidth(), ScreenHeight(), RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_SSR" );
	m_SSRX.InitRenderTarget(ScreenWidth(), ScreenHeight(), RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_SSRX");
	m_SSRY.InitRenderTarget(ScreenWidth(), ScreenHeight(), RT_SIZE_DEFAULT, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_NONE, false, "_rt_SSRY");

	m_SSR_Mat.Init(materials->FindMaterial("shaders/SSR", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_SSR_Add.Init(materials->FindMaterial("shaders/ssr_add", TEXTURE_GROUP_PIXEL_SHADERS, true));

	m_SSR_BilateralX.Init(materials->FindMaterial("shaders/ssr_bilateralx", TEXTURE_GROUP_PIXEL_SHADERS, true));
	m_SSR_BilateralY.Init(materials->FindMaterial("shaders/ssr_bilateraly", TEXTURE_GROUP_PIXEL_SHADERS, true));
}

void CSSR::Shutdown(void)
{
	m_SSR.Shutdown();
	m_SSR_Mat.Shutdown();
	m_SSR_Add.Shutdown();
}

ConVar r_post_ssr("r_post_ssr", "0", FCVAR_ARCHIVE);
ConVar r_post_ssr_raystep("r_post_ssr_raystep", "1", FCVAR_ARCHIVE);
ConVar r_post_ssr_maxdepth("r_post_ssr_maxdepth", "1", FCVAR_ARCHIVE);
ConVar r_post_ssr_stepmul("r_post_ssr_stepmul", "1.0", FCVAR_ARCHIVE);
void CSSR::Render(int x, int y, int w, int h)
{
	VPROF("CSSR::Render");

	if (!r_post_ssr.GetBool() || (IsEnabled() == false))
		return;

	if ( !mat_specular.GetBool() )
		return;

	CMatRenderContextPtr pRenderContext(materials);

	IMaterialVar* var;
	var = m_SSR_Mat->FindVar("$C1_X", NULL);
	var->SetFloatValue(r_post_ssr_raystep.GetFloat());
	var = m_SSR_Mat->FindVar("$C1_Y", NULL);
	var->SetFloatValue(r_post_ssr_maxdepth.GetFloat());
	var = m_SSR_Mat->FindVar("$C1_Z", NULL);
	var->SetFloatValue(r_post_ssr_stepmul.GetFloat());

	UpdateScreenEffectTexture(0, x, y, w, h, false);
	pRenderContext->PushRenderTargetAndViewport(m_SSR);
	DrawScreenEffectQuad(m_SSR_Mat, m_SSR->GetActualWidth(), m_SSR->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	UpdateScreenEffectTexture(0, x, y, w, h, false);
	pRenderContext->PushRenderTargetAndViewport(m_SSRX);
	DrawScreenEffectQuad(m_SSR_BilateralX, m_SSRX->GetActualWidth(), m_SSRX->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	UpdateScreenEffectTexture(0, x, y, w, h, false);
	pRenderContext->PushRenderTargetAndViewport(m_SSRY);
	DrawScreenEffectQuad(m_SSR_BilateralY, m_SSRY->GetActualWidth(), m_SSRY->GetActualHeight());
	pRenderContext->PopRenderTargetAndViewport();

	var = m_SSR_Add->FindVar("$C1_X", NULL);
	var->SetFloatValue(1.0f);

	DrawScreenEffectMaterial(m_SSR_Add, x, y, w, h);

	pRenderContext.SafeRelease();
}

//------------------------------------------------------------------------------
// Vignetting post-processing effect
//------------------------------------------------------------------------------
class CVignettingEffect : public IScreenSpaceEffect
{
public:
	CVignettingEffect(){};
	~CVignettingEffect(){};

	void Init();
	void Shutdown();

	void SetParameters( KeyValues *params ){};

	void Render( int x, int y, int w, int h );

	void Enable( bool bEnable )
	{
		m_bEnable = bEnable;
	}
	bool IsEnabled()
	{
		return m_bEnable;
	}

private:
	bool m_bEnable;

	float fVignettingAmount;
	float fVignettingLerpTo;

	CMaterialReference m_VignetMat;
};

ADD_SCREENSPACE_EFFECT( CVignettingEffect, c17_vignetting );

ConVar r_post_vignetting_darkness( "r_post_vignetting_darkness", "2", FCVAR_CHEAT, "Controls the vignetting shader's power. 0 for off." );
ConVar r_post_vignettingeffect_debug( "r_post_vignettingeffect_debug", "0", FCVAR_CHEAT );

ConVar r_post_vignettingeffect( "r_post_vignettingeffect", "1", FCVAR_ARCHIVE );

//------------------------------------------------------------------------------
// CVignettingEffect init
//------------------------------------------------------------------------------
void CVignettingEffect::Init()
{
	m_VignetMat.Init( materials->FindMaterial( "shaders/vignetting", TEXTURE_GROUP_PIXEL_SHADERS, true ) );

	fVignettingAmount = r_post_vignetting_darkness.GetFloat();
	fVignettingLerpTo = r_post_vignetting_darkness.GetFloat();
}

//------------------------------------------------------------------------------
// CVignettingEffect shutdown
//------------------------------------------------------------------------------
void CVignettingEffect::Shutdown()
{
	m_VignetMat.Shutdown();
}

//------------------------------------------------------------------------------
// CVignettingEffect render
//------------------------------------------------------------------------------
void CVignettingEffect::Render( int x, int y, int w, int h )
{
	if ( !r_post_vignettingeffect.GetBool() || ( IsEnabled() == false ) )
		return;

	fVignettingLerpTo = r_post_vignetting_darkness.GetFloat();

	if ( fVignettingAmount != fVignettingLerpTo )
		fVignettingAmount = FLerp( fVignettingAmount, fVignettingLerpTo, 0.03f );

	IMaterialVar *var;

	if ( fVignettingAmount >= 0.01f )
	{
		var = m_VignetMat->FindVar( "$VIGNETDARKNESS", NULL );
		var->SetFloatValue( fVignettingAmount );
		DrawScreenEffectMaterial( m_VignetMat, x, y, w, h );
		if ( r_post_vignettingeffect_debug.GetBool() )
			DevMsg( "Vignetting Amount: %.2f\n", fVignettingAmount );
	}
}

//------------------------------------------------------------------------------
// Health post-processing effects
//------------------------------------------------------------------------------
class CHealthEffects : public IScreenSpaceEffect
{
public:
	CHealthEffects(){};
	~CHealthEffects(){};

	void Init();
	void Shutdown();

	void SetParameters( KeyValues *params ){};

	void Render( int x, int y, int w, int h );

	void Enable( bool bEnable )
	{
		m_bEnable = bEnable;
	}
	bool IsEnabled()
	{
		return m_bEnable;
	}

private:
	bool m_bEnable;

	float fDispersionAmount;
	float fDispersionLerpTo;

	int iLastHealth;
	bool bDisableDispersionOneFrame;

	CMaterialReference m_ChromaticDisp;
};

ADD_SCREENSPACE_EFFECT( CHealthEffects, c17_healthfx );

ConVar r_post_chromatic_dispersion_offset( "r_post_chromatic_dispersion_offset", "0.2", FCVAR_CHEAT, "Controls constant chromatic dispersion strength, 0 for off." );
ConVar r_post_chromatic_dispersion_offset_heavydamage( "r_post_chromatic_dispersion_offset_heavydamage", "1.5", FCVAR_CHEAT, "Controls constant chromatic dispersion strength when the player takes heavy damage." );
ConVar r_post_chromatic_dispersion_offset_damage( "r_post_chromatic_dispersion_offset_damage", "8.0", FCVAR_CHEAT, "Controls constant chromatic dispersion strength when the player takes damage." );
ConVar r_post_healtheffects_debug( "r_post_healtheffects_debug", "0", FCVAR_CHEAT );

//------------------------------------------------------------------------------
// CHealthEffects init
//------------------------------------------------------------------------------
void CHealthEffects::Init()
{
	m_ChromaticDisp.Init( materials->FindMaterial( "shaders/chromaticDisp", TEXTURE_GROUP_PIXEL_SHADERS, true ) );

	fDispersionAmount = r_post_chromatic_dispersion_offset.GetFloat();
	fDispersionLerpTo = r_post_chromatic_dispersion_offset.GetFloat();

	iLastHealth = -1;
	bDisableDispersionOneFrame = false;
}

//------------------------------------------------------------------------------
// CHealthEffects shutdown
//------------------------------------------------------------------------------
void CHealthEffects::Shutdown()
{
	m_ChromaticDisp.Shutdown();
}

ConVar r_post_healtheffects( "r_post_healtheffects", "1", FCVAR_ARCHIVE );

//------------------------------------------------------------------------------
// CHealthEffects render
//------------------------------------------------------------------------------
void CHealthEffects::Render( int x, int y, int w, int h )
{
	if ( !r_post_healtheffects.GetBool() || ( IsEnabled() == false ) )
		return;

	C_BaseHLPlayer *pPlayer = (C_BaseHLPlayer *)C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	fDispersionLerpTo = r_post_chromatic_dispersion_offset.GetFloat();

	if ( iLastHealth > pPlayer->GetHealth() /*&& pPlayer->GetHealth() > 15*/ )
	{
		//Player took damage.
		if ( iLastHealth - pPlayer->GetHealth() >= 20 )
		{
			fDispersionAmount = r_post_chromatic_dispersion_offset_heavydamage.GetFloat();
		}
		else
		{
			fDispersionAmount = r_post_chromatic_dispersion_offset_damage.GetFloat();
		}
		bDisableDispersionOneFrame = true;
	}
	iLastHealth = pPlayer->GetHealth();

	if ( fDispersionAmount != fDispersionLerpTo && !bDisableDispersionOneFrame )
		fDispersionAmount = FLerp( fDispersionAmount, fDispersionLerpTo, 0.1f );
	else if ( bDisableDispersionOneFrame )
		bDisableDispersionOneFrame = false;

	IMaterialVar *var;

	if ( fDispersionAmount >= 0.01f )
	{
		var = m_ChromaticDisp->FindVar( "$FOCUSOFFSET", NULL );
		var->SetFloatValue( fDispersionAmount );
		var = m_ChromaticDisp->FindVar( "$radial", NULL );
		var->SetIntValue( 0 );
		DrawScreenEffectMaterial( m_ChromaticDisp, x, y, w, h );
		if ( r_post_healtheffects_debug.GetBool() )
			DevMsg( "Dispersion Amount: %.2f\n", fDispersionAmount );
	}
}

//------------------------------------------------------------------------------
// Screen Space Global Illumination
//------------------------------------------------------------------------------
class CSSGIEffect : public IScreenSpaceEffect
{
public:
	CSSGIEffect(){};
	~CSSGIEffect(){};

	void Init();
	void Shutdown();

	void SetParameters( KeyValues *params ){};

	void Render( int x, int y, int w, int h );

	void Enable( bool bEnable )
	{
		m_bEnable = bEnable;
	}
	bool IsEnabled()
	{
		return m_bEnable;
	}

private:
	bool m_bEnable;


	CMaterialReference m_SSGIMat;
};

ADD_SCREENSPACE_EFFECT( CSSGIEffect, vance_ssgi );

ConVar r_post_ssgieffect( "r_post_ssgieffect", "1", FCVAR_ARCHIVE );

//------------------------------------------------------------------------------
// CSSGIEffect init
//------------------------------------------------------------------------------
void CSSGIEffect::Init()
{
	PrecacheMaterial( "shaders/ssgi0" );
	m_SSGIMat.Init( materials->FindMaterial( "shaders/ssgi0", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

//------------------------------------------------------------------------------
// CVignettingEffect shutdown
//------------------------------------------------------------------------------
void CSSGIEffect::Shutdown()
{
	m_SSGIMat.Shutdown();
}

//------------------------------------------------------------------------------
// CVignettingEffect render
//------------------------------------------------------------------------------
void CSSGIEffect::Render( int x, int y, int w, int h )
{
	if ( !r_post_ssgieffect.GetBool() || ( IsEnabled() == false ) )
		return;

	DrawScreenEffectMaterial( m_SSGIMat, x, y, w, h );
}

class CColorCorrectionEffect : public IScreenSpaceEffect
{
public:
	CColorCorrectionEffect( void ){};

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params ){};
	virtual void Enable( bool bEnable )
	{
		m_bEnabled = bEnable;
	}
	virtual bool IsEnabled()
	{
		return m_bEnabled;
	}

	virtual void Render( int x, int y, int w, int h );

private:
	bool m_bEnabled;

	CMaterialReference m_Negative;
	CMaterialReference m_BleachBypass;
	CMaterialReference m_ColorClipping;
	CMaterialReference m_CrossProcessing;
	CMaterialReference m_NextGen;
	CMaterialReference m_Complements;
	CMaterialReference m_CubicDistortion;
	CMaterialReference m_Desaturate;
};

// Color Correction Extensions
ADD_SCREENSPACE_EFFECT( CColorCorrectionEffect, c17_colorcorrection );

ConVar r_post_negative( "r_post_negative", "0", FCVAR_CHEAT );

ConVar r_post_bleach_bypass( "r_post_bleach_bypass", "0", FCVAR_CHEAT );
ConVar r_post_bleach_bypass_opacity( "r_post_bleach_bypass_opacity", "1.0", FCVAR_CHEAT );

ConVar r_post_color_clipping( "r_post_color_clipping", "0", FCVAR_CHEAT );
ConVar r_post_color_clipping_mincolor_r( "r_post_color_clipping_mincolor_r", "0", FCVAR_CHEAT );
ConVar r_post_color_clipping_mincolor_g( "r_post_color_clipping_mincolor_g", "0", FCVAR_CHEAT );
ConVar r_post_color_clipping_mincolor_b( "r_post_color_clipping_mincolor_b", "0", FCVAR_CHEAT );
ConVar r_post_color_clipping_mincolor_a( "r_post_color_clipping_mincolor_a", "1", FCVAR_CHEAT );
ConVar r_post_color_clipping_maxcolor_r( "r_post_color_clipping_maxcolor_r", "1", FCVAR_CHEAT );
ConVar r_post_color_clipping_maxcolor_g( "r_post_color_clipping_maxcolor_g", "1", FCVAR_CHEAT );
ConVar r_post_color_clipping_maxcolor_b( "r_post_color_clipping_maxcolor_b", "1", FCVAR_CHEAT );
ConVar r_post_color_clipping_maxcolor_a( "r_post_color_clipping_maxcolor_a", "1", FCVAR_CHEAT );
ConVar r_post_color_clipping_squish( "r_post_color_clipping_squish", "1", FCVAR_CHEAT );

ConVar r_post_cross_processing( "r_post_cross_processing", "0", FCVAR_CHEAT );
ConVar r_post_cross_processing_saturation( "r_post_cross_processing_saturation", "0.8", FCVAR_CHEAT );
ConVar r_post_cross_processing_contrast( "r_post_cross_processing_contrast", "1.0", FCVAR_CHEAT );
ConVar r_post_cross_processing_brightness( "r_post_cross_processing_brightness", "0.0", FCVAR_CHEAT );
ConVar r_post_cross_processing_intensity( "r_post_cross_processing_intensity", "0.2", FCVAR_CHEAT );

ConVar r_post_complements_guidehue_r( "r_post_complements_guidehue_r", "1", FCVAR_CHEAT );
ConVar r_post_complements_guidehue_g( "r_post_complements_guidehue_g", "1", FCVAR_CHEAT );
ConVar r_post_complements_guidehue_b( "r_post_complements_guidehue_b", "1", FCVAR_CHEAT );
ConVar r_post_complements_amount( "r_post_complements_amount", "1.0", FCVAR_CHEAT );
ConVar r_post_complements_concentrate( "r_post_complements_concentrate", "1.0", FCVAR_CHEAT );
ConVar r_post_complements_desatcorr( "r_post_complements_desatcorr", "1.0", FCVAR_CHEAT );

ConVar r_post_cubic_distortion( "r_post_cubic_distortion", "0", FCVAR_CHEAT );
ConVar r_post_cubic_distortion_amount( "r_post_cubic_distortion_amount", "-0.15", FCVAR_CHEAT );
ConVar r_post_cubic_distortion_cubicamount( "r_post_cubic_distortion_cubicamount", "0.5", FCVAR_CHEAT );

ConVar r_post_desaturate( "r_post_desaturate", "0", FCVAR_CHEAT );
ConVar r_post_desaturate_strength( "r_post_desaturate_strength", "1.0", FCVAR_CHEAT );



void nextgen_callback( IConVar *pConVar, char const *pOldString, float flOldValue );

static ConVar r_post_nextgen( "r_post_nextgen", "0", FCVAR_CHEAT, "THE MOST IMPORTANT ASPECT OF CITY17! IT'LL BLOW. YOUR. MIND.", nextgen_callback );

ConVar r_post_complements( "r_post_complements", "0", FCVAR_CHEAT );

void nextgen_callback( IConVar *pConVar, char const *pOldString, float flOldValue )
{
	if ( r_post_nextgen.GetBool() )
	{
		Msg( "Shit just got unreal!\n" );
	}
}

void CColorCorrectionEffect::Init( void )
{
	m_Negative.Init( materials->FindMaterial( "shaders/negative", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_BleachBypass.Init( materials->FindMaterial( "shaders/bleach_bypass", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_ColorClipping.Init( materials->FindMaterial( "shaders/color_clipping", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_CrossProcessing.Init( materials->FindMaterial( "shaders/cross_processing", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_NextGen.Init( materials->FindMaterial( "shaders/nextgen", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_Complements.Init( materials->FindMaterial( "shaders/complements", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_CubicDistortion.Init( materials->FindMaterial( "shaders/cubic_distortion", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_Desaturate.Init( materials->FindMaterial( "shaders/desaturate", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

void CColorCorrectionEffect::Shutdown( void )
{
	m_Negative.Shutdown();
	m_BleachBypass.Shutdown();
	m_ColorClipping.Shutdown();
	m_CrossProcessing.Shutdown();
	m_NextGen.Shutdown();
	m_Complements.Shutdown();
	m_CubicDistortion.Shutdown();
	m_Desaturate.Shutdown();
}

extern ConVar mat_colorcorrection;

ConVar r_post_colorcorrection( "r_post_colorcorrection", "1", FCVAR_ARCHIVE );
void CColorCorrectionEffect::Render( int x, int y, int w, int h )
{
	VPROF( "CColorCorrectionEffect::Render" );

	if ( !r_post_colorcorrection.GetBool() || !IsEnabled() )
		return;

	if ( !mat_colorcorrection.GetBool() )
		return;

	IMaterialVar *var;

	// Grab the render context
	CMatRenderContextPtr pRenderContext( materials );

	if ( r_post_negative.GetBool() )
	{
		DrawScreenEffectMaterial( m_Negative, x, y, w, h );
	}

	if ( r_post_bleach_bypass.GetBool() )
	{
		var = m_BleachBypass->FindVar( "$OPACITY", NULL );
		var->SetFloatValue( r_post_bleach_bypass_opacity.GetFloat() );
		DrawScreenEffectMaterial( m_BleachBypass, x, y, w, h );
	}

	if ( r_post_color_clipping.GetBool() )
	{
		var = m_ColorClipping->FindVar( "$mincolor", NULL );
		var->SetVecValue( r_post_color_clipping_mincolor_r.GetFloat(), r_post_color_clipping_mincolor_g.GetFloat(),
						  r_post_color_clipping_mincolor_b.GetFloat(), r_post_color_clipping_mincolor_a.GetFloat() );
		var = m_ColorClipping->FindVar( "$maxcolor", NULL );
		var->SetVecValue( r_post_color_clipping_maxcolor_r.GetFloat(), r_post_color_clipping_maxcolor_g.GetFloat(),
						  r_post_color_clipping_maxcolor_b.GetFloat(), r_post_color_clipping_maxcolor_a.GetFloat() );
		var = m_ColorClipping->FindVar( "$squish", NULL );
		var->SetIntValue( r_post_color_clipping_squish.GetInt() );
		DrawScreenEffectMaterial( m_ColorClipping, x, y, w, h );
	}

	if ( r_post_cross_processing.GetBool() )
	{
		var = m_CrossProcessing->FindVar( "$MUTABLE_01", NULL );
		var->SetFloatValue( r_post_cross_processing_saturation.GetFloat() );
		var = m_CrossProcessing->FindVar( "$MUTABLE_02", NULL );
		var->SetFloatValue( r_post_cross_processing_contrast.GetFloat() );
		var = m_CrossProcessing->FindVar( "$MUTABLE_03", NULL );
		var->SetFloatValue( r_post_cross_processing_brightness.GetFloat() );
		var = m_CrossProcessing->FindVar( "$MUTABLE_04", NULL );
		var->SetFloatValue( r_post_cross_processing_intensity.GetFloat() );
		DrawScreenEffectMaterial( m_CrossProcessing, x, y, w, h );
	}

	if ( r_post_nextgen.GetBool() )
	{
		DrawScreenEffectMaterial( m_NextGen, x, y, w, h );
	}

	if ( r_post_complements.GetBool() )
	{
		var = m_Complements->FindVar( "$guidehue", NULL );
		var->SetVecValue( r_post_complements_guidehue_r.GetFloat(), r_post_complements_guidehue_g.GetFloat(),
						  r_post_complements_guidehue_b.GetFloat() );
		var = m_Complements->FindVar( "$amount", NULL );
		var->SetFloatValue( r_post_complements_amount.GetFloat() );
		var = m_Complements->FindVar( "$concentrate", NULL );
		var->SetFloatValue( r_post_complements_concentrate.GetFloat() );
		var = m_Complements->FindVar( "$desatcorr", NULL );
		var->SetFloatValue( r_post_complements_desatcorr.GetFloat() );
		DrawScreenEffectMaterial( m_Complements, x, y, w, h );
	}

	if ( r_post_cubic_distortion.GetBool() )
	{
		var = m_CubicDistortion->FindVar( "$distortion", NULL );
		var->SetFloatValue( r_post_cubic_distortion_amount.GetFloat() );
		var = m_CubicDistortion->FindVar( "$cubicdistortion", NULL );
		var->SetFloatValue( r_post_cubic_distortion_cubicamount.GetFloat() );
		DrawScreenEffectMaterial( m_CubicDistortion, x, y, w, h );
	}

	if ( r_post_desaturate.GetBool() )
	{
		var = m_Desaturate->FindVar( "$strength", NULL );
		var->SetFloatValue( r_post_desaturate_strength.GetFloat() );
		DrawScreenEffectMaterial( m_Desaturate, x, y, w, h );
	}

	
}