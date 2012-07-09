#ifndef SSCTESTVIEWCONTAINER_H_
#define SSCTESTVIEWCONTAINER_H_

class TestViewContainer : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();
	void testInitialize();
	void testEmptyViewContainer();
	void test_ACS_3D_Tool_Container();
	void test_AnyDual_3D_Tool_Container();
	void test_3D_Composite_Views_GPU_Container();

public:
	CPPUNIT_TEST_SUITE( TestViewContainer );
		CPPUNIT_TEST( testInitialize );
		CPPUNIT_TEST( testEmptyViewContainer );
		CPPUNIT_TEST( test_ACS_3D_Tool_Container );
		CPPUNIT_TEST( test_AnyDual_3D_Tool_Container );
		CPPUNIT_TEST( test_3D_Composite_Views_GPU_Container );
	CPPUNIT_TEST_SUITE_END();

private:
	class ContainerWindow *widget;
	QStringList image;
	bool runWidget();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestViewContainer );

#endif
