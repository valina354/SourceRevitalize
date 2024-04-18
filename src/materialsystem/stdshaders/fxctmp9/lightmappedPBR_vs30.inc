#include "shaderlib/cshader.h"
class lightmappedpbr_vs30_Static_Index
{
private:
	int m_nBUMPMAP;
#ifdef _DEBUG
	bool m_bBUMPMAP;
#endif
public:
	void SetBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP = i;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif
	}
	void SetBUMPMAP( bool i )
	{
		m_nBUMPMAP = i ? 1 : 0;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif
	}
private:
	int m_nDIFFUSEBUMPMAP;
#ifdef _DEBUG
	bool m_bDIFFUSEBUMPMAP;
#endif
public:
	void SetDIFFUSEBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDIFFUSEBUMPMAP = i;
#ifdef _DEBUG
		m_bDIFFUSEBUMPMAP = true;
#endif
	}
	void SetDIFFUSEBUMPMAP( bool i )
	{
		m_nDIFFUSEBUMPMAP = i ? 1 : 0;
#ifdef _DEBUG
		m_bDIFFUSEBUMPMAP = true;
#endif
	}
private:
	int m_nVERTEXCOLOR;
#ifdef _DEBUG
	bool m_bVERTEXCOLOR;
#endif
public:
	void SetVERTEXCOLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLOR = i;
#ifdef _DEBUG
		m_bVERTEXCOLOR = true;
#endif
	}
	void SetVERTEXCOLOR( bool i )
	{
		m_nVERTEXCOLOR = i ? 1 : 0;
#ifdef _DEBUG
		m_bVERTEXCOLOR = true;
#endif
	}
private:
	int m_nVERTEXALPHATEXBLENDFACTOR;
#ifdef _DEBUG
	bool m_bVERTEXALPHATEXBLENDFACTOR;
#endif
public:
	void SetVERTEXALPHATEXBLENDFACTOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXALPHATEXBLENDFACTOR = i;
#ifdef _DEBUG
		m_bVERTEXALPHATEXBLENDFACTOR = true;
#endif
	}
	void SetVERTEXALPHATEXBLENDFACTOR( bool i )
	{
		m_nVERTEXALPHATEXBLENDFACTOR = i ? 1 : 0;
#ifdef _DEBUG
		m_bVERTEXALPHATEXBLENDFACTOR = true;
#endif
	}
private:
	int m_nWVT;
#ifdef _DEBUG
	bool m_bWVT;
#endif
public:
	void SetWVT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWVT = i;
#ifdef _DEBUG
		m_bWVT = true;
#endif
	}
	void SetWVT( bool i )
	{
		m_nWVT = i ? 1 : 0;
#ifdef _DEBUG
		m_bWVT = true;
#endif
	}
private:
	int m_nDISPLACEMENTMAPPING;
#ifdef _DEBUG
	bool m_bDISPLACEMENTMAPPING;
#endif
public:
	void SetDISPLACEMENTMAPPING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDISPLACEMENTMAPPING = i;
#ifdef _DEBUG
		m_bDISPLACEMENTMAPPING = true;
#endif
	}
	void SetDISPLACEMENTMAPPING( bool i )
	{
		m_nDISPLACEMENTMAPPING = i ? 1 : 0;
#ifdef _DEBUG
		m_bDISPLACEMENTMAPPING = true;
#endif
	}
public:
	lightmappedpbr_vs30_Static_Index( )
	{
#ifdef _DEBUG
		m_bBUMPMAP = false;
#endif // _DEBUG
		m_nBUMPMAP = 0;
#ifdef _DEBUG
		m_bDIFFUSEBUMPMAP = false;
#endif // _DEBUG
		m_nDIFFUSEBUMPMAP = 0;
#ifdef _DEBUG
		m_bVERTEXCOLOR = false;
#endif // _DEBUG
		m_nVERTEXCOLOR = 0;
#ifdef _DEBUG
		m_bVERTEXALPHATEXBLENDFACTOR = false;
#endif // _DEBUG
		m_nVERTEXALPHATEXBLENDFACTOR = 0;
#ifdef _DEBUG
		m_bWVT = false;
#endif // _DEBUG
		m_nWVT = 0;
#ifdef _DEBUG
		m_bDISPLACEMENTMAPPING = false;
#endif // _DEBUG
		m_nDISPLACEMENTMAPPING = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllStaticVarsDefined = m_bBUMPMAP && m_bDIFFUSEBUMPMAP && m_bVERTEXCOLOR && m_bVERTEXALPHATEXBLENDFACTOR && m_bWVT && m_bDISPLACEMENTMAPPING;
		Assert( bAllStaticVarsDefined );
#endif // _DEBUG
		return ( 4 * m_nBUMPMAP ) + ( 8 * m_nDIFFUSEBUMPMAP ) + ( 16 * m_nVERTEXCOLOR ) + ( 32 * m_nVERTEXALPHATEXBLENDFACTOR ) + ( 64 * m_nWVT ) + ( 128 * m_nDISPLACEMENTMAPPING ) + 0;
	}
};
#define shaderStaticTest_lightmappedpbr_vs30 vsh_forgot_to_set_static_BUMPMAP + vsh_forgot_to_set_static_DIFFUSEBUMPMAP + vsh_forgot_to_set_static_VERTEXCOLOR + vsh_forgot_to_set_static_VERTEXALPHATEXBLENDFACTOR + vsh_forgot_to_set_static_WVT + vsh_forgot_to_set_static_DISPLACEMENTMAPPING + 0
class lightmappedpbr_vs30_Dynamic_Index
{
private:
	int m_nFASTPATH;
#ifdef _DEBUG
	bool m_bFASTPATH;
#endif
public:
	void SetFASTPATH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFASTPATH = i;
#ifdef _DEBUG
		m_bFASTPATH = true;
#endif
	}
	void SetFASTPATH( bool i )
	{
		m_nFASTPATH = i ? 1 : 0;
#ifdef _DEBUG
		m_bFASTPATH = true;
#endif
	}
private:
	int m_nDOWATERFOG;
#ifdef _DEBUG
	bool m_bDOWATERFOG;
#endif
public:
	void SetDOWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDOWATERFOG = i;
#ifdef _DEBUG
		m_bDOWATERFOG = true;
#endif
	}
	void SetDOWATERFOG( bool i )
	{
		m_nDOWATERFOG = i ? 1 : 0;
#ifdef _DEBUG
		m_bDOWATERFOG = true;
#endif
	}
public:
	lightmappedpbr_vs30_Dynamic_Index()
	{
#ifdef _DEBUG
		m_bFASTPATH = false;
#endif // _DEBUG
		m_nFASTPATH = 0;
#ifdef _DEBUG
		m_bDOWATERFOG = false;
#endif // _DEBUG
		m_nDOWATERFOG = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bFASTPATH && m_bDOWATERFOG;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nFASTPATH ) + ( 2 * m_nDOWATERFOG ) + 0;
	}
};
#define shaderDynamicTest_lightmappedpbr_vs30 vsh_forgot_to_set_dynamic_FASTPATH + vsh_forgot_to_set_dynamic_DOWATERFOG + 0
