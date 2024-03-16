#include "shaderlib/cshader.h"
class colorblindsimulator_ps30_Static_Index
{
public:
	colorblindsimulator_ps30_Static_Index( )
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
#define shaderStaticTest_colorblindsimulator_ps30 0
class colorblindsimulator_ps30_Dynamic_Index
{
private:
	int m_nCOLOR;
#ifdef _DEBUG
	bool m_bCOLOR;
#endif
public:
	void SetCOLOR( int i )
	{
		Assert( i >= 0 && i <= 3 );
		m_nCOLOR = i;
#ifdef _DEBUG
		m_bCOLOR = true;
#endif
	}
	void SetCOLOR( bool i )
	{
		m_nCOLOR = i ? 1 : 0;
#ifdef _DEBUG
		m_bCOLOR = true;
#endif
	}
public:
	colorblindsimulator_ps30_Dynamic_Index()
	{
#ifdef _DEBUG
		m_bCOLOR = false;
#endif // _DEBUG
		m_nCOLOR = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllDynamicVarsDefined = m_bCOLOR;
		Assert( bAllDynamicVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nCOLOR ) + 0;
	}
};
#define shaderDynamicTest_colorblindsimulator_ps30 psh_forgot_to_set_dynamic_COLOR + 0
