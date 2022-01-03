#pragma once

namespace OpenGL
{
    class Font
    {
    public:
        Font(const std::string& filename);
        ~Font();

        void RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, float width, float height);

    private:
        class FontImp;
        std::unique_ptr<FontImp> m_imp;
    };
}