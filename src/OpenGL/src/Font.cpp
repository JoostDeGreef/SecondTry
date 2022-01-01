#include "OpenGL.h"

// see https://learnopengl.com/In-Practice/Text-Rendering

//
// #include "ft2build.h"
// #include FT_FREETYPE_H
//
#include "freetype/freetype.h"
//#include "freetype/CharacterMap.h"
//#include "freetype/Font.h"

#include "Log.h"

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
        Character(const unsigned int textureID, const Vector2i& size, const Vector2i& bearing, const int advance)
            : TextureID(textureID)
            , Size(size)
            , Bearing(bearing)
            , Advance(advance)
        {}

        unsigned int TextureID;  // ID handle of the glyph texture
        Vector2i     Size;       // Size of glyph
        Vector2i     Bearing;    // Offset from baseline to left/top of glyph
        int          Advance;    // Offset to advance to next glyph
    };

    class Font::FontImp
    {
    public:
        FontImp(const std::string& filename)
            : m_ft(FreetypeLibrary::Instance())
            , m_height(48)
            , m_shader("text")
        {
            if (FT_New_Face(m_ft, filename.c_str(), 0, &m_face))
            {
                LogError("Failed to load font \"{}\"",filename);
                throw;
            }
            FT_Set_Pixel_Sizes(m_face, 0, m_height);

            glGenVertexArrays(1, &m_VAO);
            glGenBuffers(1, &m_VBO);
            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        ~FontImp()
        {
            if (FT_Done_Face(m_face))
            {
                // log error on shutdown?
            }
        }

        void RenderText(const std::string& text, float x, float y, float scale, RGBColorf color)
        {
            // activate corresponding render state	
            auto [attributes,uniforms] = m_shader.Use();
            glUniform3f(uniforms.at(0), color.R, color.G, color.B);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(m_VAO);

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
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            }
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    protected:
        Character& LoadCharacter(int c)
        {
            auto iter = m_characters.find(c);
            if (iter == m_characters.end())
            {
                if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
                {
                    // log warning:  Failed to load Glyph
                    if (FT_Load_Char(m_face, '?', FT_LOAD_RENDER))
                    {
                        // log error: unrecoverable error
                        // throw
                    }
                }
                // generate texture
                unsigned int texture;
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
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
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                iter = m_characters.emplace(std::make_pair(c, Character(
                    texture,
                    Vector2i(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                    Vector2i(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                    (int)m_face->glyph->advance.x
                ))).first;
            }
            return iter->second;
        }
    private:
        FT_Library& m_ft;
        FT_Face m_face;
        std::map<int, Character> m_characters;
        int m_height;
        Shader m_shader;
        unsigned int m_VAO, m_VBO;
    };

    Font::Font(const std::string& filename)
        : m_imp(std::make_unique<FontImp>(filename))
    {}

    Font::~Font() = default;

    void Font::RenderText(const std::string& text, float x, float y, float scale, RGBColorf color) 
    {
        m_imp->RenderText(text, x, y, scale, color);
    }
}