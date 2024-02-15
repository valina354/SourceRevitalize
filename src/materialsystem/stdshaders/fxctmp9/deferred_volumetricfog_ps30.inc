#include "shaderlib/cshader.h"
class deferred_volumetricfog_ps30_Static_Index
{
public:
	deferred_volumetricfog_ps30_Static_Index( )
	{
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
		return 0;
	}
};
#define shaderStaticTest_deferred_volumetricfog_ps30 0
class deferred_volumetricfog_ps30_Dynamic_Index
{
private:
	int m_nFOG;
#ifdef _DEBUG
	bool m_bFOG;
#endif
public:
	void SetFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFOG = i;
#ifdef _DEBUG
		m_bFOG = true;
#endif
	}
	void SetFOG( bool i )
	{
		m_nFOG = i ? 1 : 0;
#ifdef _DEBUG
		m_bFOG = true;
#endif
	}
private:
	int m_nCSM;
#ifdef _DEBUG
	bool m_bCSM;
#endif
public:
	void SetCSM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCSM = i;
#ifdef _DEBUG
		m_bCSM = true;
#endif
	}
	void SetCSM( bool i )
	{
		m_nCSM = i ? 1 : 0;
#ifdef _DEBUG
		m_bCSM = true;
#endif
	}
public:
	deferred_volumetricfog_ps30_Dynamic_Index()
	{
#ifdef _DEBUG
		m_bFOG = false;
#endif // _DEBUG
		m_nFOG = 0;
#ifdef _DEBUG
		m_bCSM = false;
#endif // _DEBUG
		m_nCSM = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bFOG && m_bCSM;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nFOG ) + ( 2 * m_nCSM ) + 0;
	}
};
#define shaderDynamicTest_deferred_volumetricfog_ps30 psh_forgot_to_set_dynamic_FOG + psh_forgot_to_set_dynamic_CSM + 0
