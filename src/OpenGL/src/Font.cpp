#include "OpenGL.h"

#include "freetype/freetype.h"

#include "Log.h"

using namespace Core;

namespace OpenGL
{
    class FreetypeLibrary
    {
    public:
        FreetypeLibrary()
        {
            if (FT_Init_FreeType(&m_ft))
            {
                LogError("Could not init FreeType Library\n");
                throw;
            }
        }

        ~FreetypeLibrary()
        {
            if (FT_Done_FreeType(m_ft))
            {
                // log error on shutdown? no, just debug                
            }
        }

        operator FT_Library& ()
        {
            return m_ft;
        }

        static FreetypeLibrary& Instance()
        {
            static FreetypeLibrary freetypeLibrary;
            return freetypeLibrary;
        }

    private:
        FT_Library m_ft;
    };

    class Character
    {
    public:
        Character(const unsigned int textureID, const Vector2i& size, const Vector2i& bearing, const Vector2i& advance)
            : TextureID(textureID)
            , Size(size)
            , Bearing(bearing)
            , Advance(advance)
        {}

        unsigned int TextureID;  // ID handle of the glyph texture
        Vector2i     Size;       // Size of glyph
        Vector2i     Bearing;    // Offset from baseline to left/top of glyph
        Vector2i     Advance;    // Offset to advance to next glyph
    };

    class Font::FontImp
    {
    public:
        FontImp(const std::string& filename)
            : m_ft(FreetypeLibrary::Instance())
            , m_height(72)
            , m_shader()
        {
            if (FT_New_Face(m_ft, filename.c_str(), 0, &m_face))
            {
                LogError("Failed to load font \"{}\"\n",filename);
                throw;
            }
            FT_Set_Pixel_Sizes(m_face, 0, m_height);

            GLCHECK(glGenVertexArrays(1, &m_VAO));
            GLCHECK(glGenBuffers(1, &m_VBO));
            GLCHECK(glBindVertexArray(m_VAO));
            GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
            GLCHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW));
            GLCHECK(glEnableVertexAttribArray(0));
            GLCHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0));
            GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
            GLCHECK(glBindVertexArray(0));
        }

        ~FontImp()
        {
            if (FT_Done_Face(m_face))
            {
                // log error on shutdown?
            }
        }

        void RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, const OpenGL::State & state2d)
        {
            // activate corresponding render state	
            m_shader.Activate();
            m_shader.SetModel(state2d.Model());
            m_shader.SetProjection(state2d.Projection());
            m_shader.SetColor(color);
            
            GLCHECK(glActiveTexture(GL_TEXTURE0));
            GLCHECK(glBindVertexArray(m_VAO));

            // iterate through all characters
            for (auto c = text.cbegin(); c != text.cend(); c++)
            {
                Character ch = LoadCharacter(*c);

                float xpos = x + ch.Bearing[0] * scale;
                float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

                float w = ch.Size[0] * scale;
                float h = ch.Size[1] * scale;
                // update VBO for each character
                float vertices[6][4] = {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
                };
                // render glyph texture over quad
                GLCHECK(glBindTexture(GL_TEXTURE_2D, ch.TextureID));
                m_shader.SetText(0);
                // update content of VBO memory
                GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
                GLCHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
                // render quad
                GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.Advance[0] >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)

                GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
            }
            GLCHECK(glBindVertexArray(0));
            GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
        }
        RenderSize CalcTextSize(const std::string& text, float scale)
        {
            Vector2f pos(0.0,0.0);
            Vector2f offset(0.0,0.0);
            Vector2f size(0.0,0.0);
            Vector2f overflow(0.0,0.0);
            GLCHECK(glActiveTexture(GL_TEXTURE0));
            // iterate through all characters
            for (auto c = text.cbegin(); c != text.cend(); c++)
            {
                Character ch = LoadCharacter(*c);

                float xpos = pos[0] + ch.Bearing[0] * scale;
                float ypos = pos[1] - (ch.Size[1] - ch.Bearing[1]) * scale;
                
                if(xpos<pos[0]) pos[0] = xpos;
                if(ypos<pos[1]) pos[1] = ypos;

                xpos += ch.Size[0] * scale;
                ypos += ch.Size[1] * scale;

                if(xpos>size[0]) size[0] = xpos;
                if(ypos>size[1]) size[1] = ypos;

                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                pos[0] += (ch.Advance[0] >> 6) * scale; 
                float y = (ch.Advance[1] >> 6) * scale; 
                if(y>overflow[1]) overflow[1]=y;
            }
            overflow[0] = pos[0];
            return RenderSize(offset,size,overflow);
        }
    protected:
        Character& LoadCharacter(int c)
        {
            auto iter = m_characters.find(c);
            if (iter == m_characters.end())
            {
                if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
                {
                    LogWarning("character \'{}\' not available\n",c);
                    if (FT_Load_Char(m_face, '?', FT_LOAD_RENDER))
                    {
                        LogError("unrecoverable error loading character \'{}\'\n",c);
                        throw;
                    }
                }
                // generate texture
                unsigned int texture;
                GLCHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
                GLCHECK(glGenTextures(1, &texture));
                GLCHECK(glBindTexture(GL_TEXTURE_2D, texture));
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    m_face->glyph->bitmap.width,
                    m_face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    m_face->glyph->bitmap.buffer
                );
                // set texture options
                GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
                GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
                GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
                GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
                // now store character for later use
                iter = m_characters.emplace(std::make_pair(c, Character(
                    texture,
                    Vector2i(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                    Vector2i(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                    Vector2i((int)m_face->glyph->advance.x, (int)m_face->glyph->advance.y)
                ))).first;
                // unload texture
                GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
            }
            return iter->second;
        }
    private:
        FT_Library& m_ft;
        FT_Face m_face;
        std::map<int, Character> m_characters;
        int m_height;
        Shader_text m_shader;
        unsigned int m_VAO, m_VBO;
    };

    Font::Font(const std::string& filename)
        : m_imp(std::make_unique<FontImp>(filename))
    {}

    Font::~Font() = default;

    void Font::RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, const OpenGL::State & state2d)
    {
        m_imp->RenderText(text, x, y, scale, color, state2d);
    }
    void Font::RenderText(const std::string& text, const Vector2f & pos, float scale, RGBColorf color, const OpenGL::State & state2d)
    {
        RenderText(text, pos[0], pos[1], scale, color, state2d);
    }

    RenderSize Font::CalcTextSize(const std::string& text, float scale)
    {
        return m_imp->CalcTextSize(text,scale);
    }

    RenderSize::RenderSize(const Vector2f & size)
        : RenderSize(Vector2f(0,0),size,size)
    {}
    RenderSize::RenderSize(const Vector2f & offset,const Vector2f & size,const Vector2f & overflow)
        : m_offset(offset)
        , m_size(size)
        , m_overflow(overflow)
    {}

    const Vector2f & RenderSize::GetOffset() const
    {
        return m_offset;
    }

    const Vector2f & RenderSize::GetSize() const
    {
        return m_size;
    }

    const Vector2f & RenderSize::GetOverflow() const
    {
        return m_overflow;
    }
}

