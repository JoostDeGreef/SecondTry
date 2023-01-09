#pragma once

namespace OpenGL
{
    class RenderSize
    {
    public:
        RenderSize(const Core::Vector2f & size);
        RenderSize(const Core::Vector2f & offset,const Core::Vector2f & size,const Core::Vector2f & overflow);

        // rendering space below/left of text origin (<=origin)
        const Core::Vector2f & GetOffset() const;
        // redering width/height of core text
        const Core::Vector2f & GetSize() const;
        // rendering space above/right of text (>=size)
        const Core::Vector2f & GetOverflow() const;
    private:
        Core::Vector2f m_offset;
        Core::Vector2f m_size;
        Core::Vector2f m_overflow;
    };

    class Font
    {
    public:
        Font(const std::string& filename);
        ~Font();

        RenderSize CalcTextSize(const std::string& text, float scale);
        void RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, const OpenGL::State & state2d);
        void RenderText(const std::string& text, const Core::Vector2f & pos, float scale, RGBColorf color, const OpenGL::State & state2d);
    private:
        class FontImp;
        std::unique_ptr<FontImp> m_imp;
    };
}