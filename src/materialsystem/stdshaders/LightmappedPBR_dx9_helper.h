//===================== Copyright (c) AdV Software And Source Revitalize For Improving It. All Rights Reserved. ======================
//
// The .h Helper File for the brush PBR shader
//
//==================================================================================================

#ifndef EXAMPLE_MODEL_DX9_HELPER_H
#define EXAMPLE_MODEL_DX9_HELPER_H

#include <string.h>

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CBaseVSShader;
class IMaterialVar;
class IShaderDynamicAPI;
class IShaderShadow;

//-----------------------------------------------------------------------------
// Init params/ init/ draw methods
//-----------------------------------------------------------------------------
struct LightmappedPBR_DX9_Vars_t
{
	LightmappedPBR_DX9_Vars_t()
	{
		memset( this, 0xFF, sizeof( *this ) );
	}

	int m_nBaseTexture;
	int m_nBaseTexture2;
	int m_nBaseTextureFrame;
	int m_nBaseTextureTransform;
	int m_nAlphaTestReference;
	int m_nRoughness;
	int m_nRoughness2;
	int m_nMetallic;
	int m_nMetallic2;
	int m_nAO;
	int m_nAO2;
	int m_nDetail;
	int m_nDetailStrength;
	int m_nEnvmap;
	int m_nBumpmap;
	int m_nBumpmap2;
	int m_nFlashlightTexture;
	int m_nFlashlightTextureFrame;
	int m_nBRDF;
	int m_nUseSmoothness;
	int m_nSeamlessMappingScale;
	int ParallaxDepth;
	int ParallaxScaling;
	int ParallaxStep;
	int DisplacementDepth;
	int DisplacementEnabled;
	int DisplacementMap;

	// Parallax cubemaps
	int m_nEnvmapParallax; // Needed for editor
	int m_nEnvmapParallaxObb1;
	int m_nEnvmapParallaxObb2;
	int m_nEnvmapParallaxObb3;
	int m_nEnvmapOrigin;
};

void InitParamsLightmappedPBR_DX9( CBaseVSShader *pShader, IMaterialVar **params, const char *pMaterialName, LightmappedPBR_DX9_Vars_t &info );

void InitLightmappedPBR_DX9( CBaseVSShader *pShader, IMaterialVar **params, LightmappedPBR_DX9_Vars_t &info );

void DrawLightmappedPBR_DX9( CBaseVSShader *pShader, IMaterialVar **params, IShaderDynamicAPI *pShaderAPI, IShaderShadow *pShaderShadow, bool bHasFlashlight, LightmappedPBR_DX9_Vars_t &info, VertexCompressionType_t vertexCompression, CBasePerMaterialContextData **pContextDataPtr );

#endif // EXAMPLE_MODEL_DX9_HELPER_H