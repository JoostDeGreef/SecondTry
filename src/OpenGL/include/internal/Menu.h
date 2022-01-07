#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace OpenGL
{

class MenuItem
{
    friend class SubMenuItem;
public:
    int GetID() const
    {
        return m_id;
    }

protected:
    MenuItem(const int id, MenuItem * parent, const std::string & caption)
       : m_parentItem(parent)
       , m_caption(caption)
       , m_id(id)
    {}

    virtual int GenerateID()
    {
        return m_parentItem->GenerateID();
    }
private:
    MenuItem * m_parentItem;
    std::string m_caption;
    int m_id;
};

class SubMenuItem : public MenuItem
{
public:
    // adds a menuitem and returns an unique id
    template<typename MENUITEM,typename ...ARGS>
    MENUITEM * Add(ARGS... args)
    {
        m_items.emplace_back(new MENUITEM(GenerateID(), this, args...));
        return static_cast<MENUITEM *>(m_items.back());
    }

    // clear all submenus
    void Clear()
    {
        m_items.clear();
    }

protected:
    SubMenuItem(const int id, MenuItem * parent, const std::string & caption)
       : MenuItem(id,parent,caption)
       , m_items()
       , m_openItem(nullptr)
    {}
    ~SubMenuItem()
    {
        for(MenuItem * item:m_items)
        {
            delete item;
        }
    }

private:
    std::vector<MenuItem*> m_items;
    MenuItem * m_openItem;
};

class Menu: public SubMenuItem
{
public:
    Menu()
        : SubMenuItem(GenerateID(), nullptr, "Menu")
    {}

    // override this function to receive menu item mouse clicks
    virtual void Click(MenuItem & item) {};

protected:
    virtual int GenerateID() override final
    {
        static int id = 0;
        return id++;
    }

private:
    
}; // Menu

} // OpenGL
