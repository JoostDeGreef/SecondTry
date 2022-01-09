
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

TEST_F(MenuTest, Count)
{
    OpenGL::Menu menu;
    int id1 = menu.Add<OpenGL::MenuItem>("line 1")->GetID();
    int id2 = menu.Add<OpenGL::MenuItem>("line 2")->GetID();
    int count = menu.Count();
    EXPECT_EQ(2,count);
    auto & submenu = *menu.Add<OpenGL::SubMenuItem>("line 3");
    int id3 = submenu.GetID();
    int id4 = submenu.Add<OpenGL::MenuItem>("line 4")->GetID();
    int id5 = submenu.Add<OpenGL::MenuItem>("line 5")->GetID();
    count = menu.Count();
    EXPECT_EQ(5,count);
    count = submenu.Count();
    EXPECT_EQ(2,count);
}

TEST_F(MenuTest, Remove)
{
    OpenGL::Menu menu;
    int id1 = menu.Add<OpenGL::MenuItem>("line 1")->GetID();
    int id2 = menu.Add<OpenGL::MenuItem>("line 2")->GetID();
    auto & submenu = *menu.Add<OpenGL::SubMenuItem>("line 3");
    int id3 = submenu.GetID();
    int id4 = submenu.Add<OpenGL::MenuItem>("line 4")->GetID();
    int id5 = submenu.Add<OpenGL::MenuItem>("line 5")->GetID();
    int id6 = submenu.Add<OpenGL::MenuItem>("line 6")->GetID();
    int id7 = submenu.Add<OpenGL::MenuItem>("line 7")->GetID();
    menu.Remove(42);
    int count = menu.Count();
    EXPECT_EQ(7,count);
    count = submenu.Count();
    EXPECT_EQ(4,count);
    submenu.Remove(id6);
    count = menu.Count();
    EXPECT_EQ(6,count);
    count = submenu.Count();
    EXPECT_EQ(3,count);
    menu.Remove(id5);
    count = menu.Count();
    EXPECT_EQ(5,count);
    count = submenu.Count();
    EXPECT_EQ(2,count);
}
