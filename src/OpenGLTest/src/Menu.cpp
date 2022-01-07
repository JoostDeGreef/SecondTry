
#include "gtest/gtest.h"
#include "OpenGL.h"

using namespace testing;

class MenuTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};


TEST_F(MenuTest, MenuHasID0)
{
    OpenGL::Menu menu;
    int id = menu.GetID();
    EXPECT_EQ(0,id);
}

TEST_F(MenuTest, Add)
{
    OpenGL::Menu menu;
    int id1 = menu.Add<OpenGL::MenuItem>("line 1")->GetID();
    int id2 = menu.Add<OpenGL::MenuItem>("line 2")->GetID();
    auto & submenu = *menu.Add<OpenGL::SubMenuItem>("line 3");
    int id3 = submenu.GetID();
    int id4 = submenu.Add<OpenGL::MenuItem>("line 4")->GetID();
    EXPECT_LT(0,id1);
    EXPECT_LT(id1,id2);
    EXPECT_LT(id2,id3);
    EXPECT_LT(id3,id4);
}
