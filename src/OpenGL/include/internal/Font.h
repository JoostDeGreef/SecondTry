#pragma once

namespace OpenGL
{
    class Font
    {
    public:
        Font(const std::string& filename);
        ~Font();

        void RenderText(const std::string& text, float x, float y, float scale_x, float scale_y, RGBAColorf color);

    private:
        class FontImp;
        std::unique_ptr<FontImp> m_imp;
    };
}