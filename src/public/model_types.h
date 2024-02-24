//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( MODEL_TYPES_H )
#define MODEL_TYPES_H
#ifdef _WIN32
#pragma once
#endif

#define STUDIO_NONE						0x00000000
#define STUDIO_RENDER					0x00000001
#define STUDIO_VIEWXFORMATTACHMENTS		0x00000002
#define STUDIO_DRAWTRANSLUCENTSUBMODELS 0x00000004
#define STUDIO_TWOPASS					0x00000008
#define STUDIO_STATIC_LIGHTING			0x00000010
#define STUDIO_WIREFRAME				0x00000020
#define STUDIO_ITEM_BLINK				0x00000040
#define STUDIO_NOSHADOWS				0x00000080
#define STUDIO_WIREFRAME_VCOLLIDE		0x00000100
	#ifdef VANCE
		//#define STUDIO_NOLIGHTING_OR_CUBEMAP	0x00000200
		#define STUDIO_SKIP_FLEXES 0x00000400
		#define STUDIO_DONOTMODIFYSTENCILSTATE 0x00000800 // TERROR
	#endif												  // MAPBASE

// Not a studio flag, but used to flag when we want studio stats
#define STUDIO_GENERATE_STATS			0x01000000

// Not a studio flag, but used to flag when updating shadow direction
#define STUDIO_UPDATE_SHADOW_DIR 0x02000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SSAODEPTHTEXTURE			0x08000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SHADOWDEPTHTEXTURE		0x40000000

// Not a studio flag, but used to flag model as a non-sorting brush model
#define STUDIO_TRANSPARENCY				0x80000000

#ifdef VANCE
		#define STUDIO_SKIP_DECALS 0x10000000

		#define STUDIO_ENGINE_FLAGS 0xC90001FF
	#endif // MAPBASE


enum modtype_t
{
	mod_bad = 0, 
	mod_brush, 
	mod_sprite, 
	mod_studio
};

#endif // MODEL_TYPES_H
