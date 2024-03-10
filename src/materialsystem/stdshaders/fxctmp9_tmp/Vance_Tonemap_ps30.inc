#include "shaderlib/cshader.h"
class vance_tonemap_ps30_Static_Index
{
public:
	vance_tonemap_ps30_Static_Index( )
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
#define shaderStaticTest_vance_tonemap_ps30 0
class vance_tonemap_ps30_Dynamic_Index
{
private:
	int m_nMODE;
#ifdef _DEBUG
	bool m_bMODE;
#endif
public:
	void SetMODE( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nMODE = i;
#ifdef _DEBUG
		m_bMODE = true;
#endif
	}
	void SetMODE( bool i )
	{
		m_nMODE = i ? 1 : 0;
#ifdef _DEBUG
		m_bMODE = true;
#endif
	}
private:
	int m_nAUTO_EXPOSURE;
#ifdef _DEBUG
	bool m_bAUTO_EXPOSURE;
#endif
public:
	void SetAUTO_EXPOSURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nAUTO_EXPOSURE = i;
#ifdef _DEBUG
		m_bAUTO_EXPOSURE = true;
#endif
	}
	void SetAUTO_EXPOSURE( bool i )
	{
		m_nAUTO_EXPOSURE = i ? 1 : 0;
#ifdef _DEBUG
		m_bAUTO_EXPOSURE = true;
#endif
	}
public:
	vance_tonemap_ps30_Dynamic_Index()
	{
#ifdef _DEBUG
		m_bMODE = false;
#endif // _DEBUG
		m_nMODE = 0;
#ifdef _DEBUG
		m_bAUTO_EXPOSURE = false;
#endif // _DEBUG
		m_nAUTO_EXPOSURE = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bMODE && m_bAUTO_EXPOSURE;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nMODE ) + ( 4 * m_nAUTO_EXPOSURE ) + 0;
	}
};
#define shaderDynamicTest_vance_tonemap_ps30 psh_forgot_to_set_dynamic_MODE + psh_forgot_to_set_dynamic_AUTO_EXPOSURE + 0
