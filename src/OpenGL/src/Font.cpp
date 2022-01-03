#include "OpenGL.h"

#include "freetype/freetype.h"

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
            , m_shader(OpenGL::Shader::LoadFromResource("text"))
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

        void RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, float width, float height)
        {
            // activate corresponding render state	
            auto uniforms = m_shader.Use("model","projection","color","text");
            auto model = OpenGL::Mat4::Identity();
            auto projection = OpenGL::Mat4::Ortho(0,width,0,height,-1,1);
            GLCHECK(glUniformMatrix4fv(uniforms.at(0), 1, true, model.data())); // model
            GLCHECK(glUniformMatrix4fv(uniforms.at(1), 1, true, projection.data())); // projection
            GLCHECK(glUniform3f(uniforms.at(2), color.R, color.G, color.B)); // color
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
                GLCHECK(glUniform1i(uniforms.at(3), 0));
                // update content of VBO memory
                GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
                GLCHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
                // render quad
                GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)

                GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
            }
            GLCHECK(glBindVertexArray(0));
            GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
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
                    (int)m_face->glyph->advance.x
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
        Shader m_shader;
        unsigned int m_VAO, m_VBO;
    };

    Font::Font(const std::string& filename)
        : m_imp(std::make_unique<FontImp>(filename))
    {}

    Font::~Font() = default;

    void Font::RenderText(const std::string& text, float x, float y, float scale, RGBColorf color, float width, float height) 
    {
        m_imp->RenderText(text, x, y, scale, color, width, height);
    }
}
