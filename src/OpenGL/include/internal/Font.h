#pragma once

namespace OpenGL
{
    class RenderSize
    {
    public:
        RenderSize(const Vector2f & size);
        RenderSize(const Vector2f & offset,const Vector2f & size,const Vector2f & overflow);

        // rendering space below/left of text origin (<=origin)
        const Vector2f & GetOffset() const;
        // redering width/height of core text
        const Vector2f & GetSize() const;
        // rendering space above/right of text (>=size)
        const Vector2f & GetOverflow() const;
    private:
        Vector2f m_offset;
        Vector2f m_size;
        Vector2f m_overflow;
    };

    class Font
    {
    public:
        Font(const std::string& filename);
        ~Font();

        RenderSize CalcTextSize(const std::string& text, float scale);
        void RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, float screenWidth, float screenHeight);
        void RenderText(const std::string& text, const Vector2f & pos, float scale, RGBColorf color, const Vector2f & screenSize);

    private:
        class FontImp;
        std::unique_ptr<FontImp> m_imp;
    };
}