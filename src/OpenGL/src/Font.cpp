#include "OpenGL.h"

// see https://learnopengl.com/In-Practice/Text-Rendering

//
//#include "ft2build.h"
//#include FT_FREETYPE_H
//
#include "freetype/Freetype.h"
//#include "freetype/CharacterMap.h"
//#include "freetype/Font.h"

// todo: move this to the data container
static const std::string vertexShaderSource = R"SRC(
#version 330 core
attribute vec4 coord;
varying vec2 texpos;

void main(void) {
  gl_Position = vec4(coord.xy, 0, 1);
  texpos = coord.zw;
}
)SRC";
static const std::string fragmentShaderSource = R"SRC(
#version 330 core
varying vec2 texpos;
uniform sampler2D tex;
uniform vec4 color;

void main(void) {
  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
}

)SRC";

namespace OpenGL
{
    class FreetypeLibrary
    {
    public:
        FreetypeLibrary()
        {
            if (FT_Init_FreeType(&m_ft))
            {
                // log error: std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                // throw
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
        Character(const unsigned int textureID, Vector2i& size, Vector2i& bearing, const int advance)
            : TextureID(textureID)
            , Size(std::move(size))
            , Bearing(std::move(bearing))
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
            , m_shader(vertexShaderSource,fragmentShaderSource)
        {
            if (FT_New_Face(m_ft, filename.c_str(), 0, &m_face))
            {
                // log error: std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
                // throw
            }
            FT_Set_Pixel_Sizes(m_face, 0, m_height);

        }

        ~FontImp()
        {
            if (FT_Done_Face(m_face))
            {
                // log error on shutdown?
            }
        }

#define GLERROR(f)                                    \
{                                                     \
   f;                                                 \
   GLenum glerror = glGetError();                     \
   switch(glerror)                                    \
   {                                                  \
       case GL_NO_ERROR:                              \
           break;                                     \
       case GL_INVALID_ENUM:                          \
           throw std::exception("GL_INVALID_ENUM");                   \
       case GL_INVALID_VALUE:                         \
           throw std::exception("GL_INVALID_VALUE");                  \
       case GL_INVALID_OPERATION:                     \
           throw std::exception("GL_INVALID_OPERATION");              \
       case GL_INVALID_FRAMEBUFFER_OPERATION:         \
           throw std::exception("GL_INVALID_FRAMEBUFFER_OPERATION");  \
       case GL_OUT_OF_MEMORY:                         \
           throw std::exception("GL_OUT_OF_MEMORY");                  \
       default:                                       \
           throw std::exception("unknown error");                     \
   }                                                  \
}

        void RenderText(const std::string& text, float x, float y, float scale_x, float scale_y, RGBAColorf color)
        {
            GLuint uniform_tex, attribute_coord, uniform_color;

            m_shader.Use();
            uniform_tex = glGetUniformLocation(m_shader.Program, "tex");
            uniform_color = glGetUniformLocation(m_shader.Program, "color");
            attribute_coord = glGetAttribLocation(m_shader.Program, "coord");

            GLERROR(glUniform4fv(uniform_color, 1, color.RGBA));

            // Create the vertex buffer object
            unsigned int vbo;
            GLERROR(glGenBuffers(1, &vbo));
            GLuint vao;
            glGenVertexArrays(1, &vao);

            FT_GlyphSlot g = m_face->glyph;

            /* Create a texture that will be used to hold one "glyph" */
            GLuint tex;

            GLERROR(glActiveTexture(GL_TEXTURE0));
            GLERROR(glGenTextures(1, &tex));
            GLERROR(glBindTexture(GL_TEXTURE_2D, tex));
            GLERROR(glUniform1i(uniform_tex, 0));

            /* We require 1 byte alignment when uploading texture data */
            GLERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

            /* Clamping to edges is important to prevent artifacts when scaling */
            GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            /* Linear filtering usually looks best for text */
            GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GLERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            /* Set up the VBO for our vertex data */
            GLERROR(glBindVertexArray(vao));
            GLERROR(glBindBuffer(GL_ARRAY_BUFFER, vbo));
            GLERROR(glEnableVertexAttribArray(attribute_coord));
            GLERROR(glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0));

            /* Loop through all characters */
            for (const char c: text) 
            {
                /* Try to load and render the character */
                if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
                {
                    continue;
                }

                /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
                GLERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer));

                /* Calculate the vertex and texture coordinates */
                float x2 = x + g->bitmap_left * scale_x;
                float y2 = -y - g->bitmap_top * scale_y;
                float w = g->bitmap.width * scale_x;
                float h = g->bitmap.rows * scale_y;

                GLfloat box[4][4] = {
                    {x2, -y2, 0, 0},
                    {x2 + w, -y2, 1, 0},
                    {x2, -y2 - h, 0, 1},
                    {x2 + w, -y2 - h, 1, 1},
                };

                /* Draw the character on the screen */
                GLERROR(glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW));
                GLERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

                /* Advance the cursor to the start of the next character */
                x += (g->advance.x >> 6) * scale_x;
                y += (g->advance.y >> 6) * scale_y;
            }

            GLERROR(glUseProgram(0));

            GLERROR(glDisableVertexAttribArray(attribute_coord));
            GLERROR(glDeleteTextures(1, &tex));

            GLERROR(glDeleteVertexArrays(1, &vao));
            GLERROR(glDeleteBuffers(1, &vbo));

            //// activate corresponding render state	
            //m_shader.Use();
            //glUniform3f(glGetUniformLocation(m_shader.Program, "textColor"), color.R, color.G, color.B);
            //TMatrix<float, 4, 4> projection;
            //projection.Fill(0);
            //projection(0, 0) = 2 / (800 - 0.0f);
            //projection(1, 1) = 2 / (600 - 0.f);
            //projection(2, 2) = -2 / (1 - -1.0f);
            //projection(0, 3) = -(800 + 0) / (800 - 0.0f);
            //projection(1, 3) = -(600 + 0) / (600 - 0.0f);
            //projection(2, 3) = -(1 + -1.0f) / (1 - -1.0f);
            //glUniformMatrix4fv(glGetUniformLocation(m_shader.Program, "projection"), 1, GL_FALSE, projection.Raw());

            //glGenVertexArrays(1, &m_VAO);
            //glGenBuffers(1, &m_VBO);
            //glBindVertexArray(m_VAO);
            //glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            //glEnableVertexAttribArray(0);
            //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            //glBindVertexArray(m_VAO);
            //glActiveTexture(GL_TEXTURE0);

            //// iterate through all characters
            //for (auto c = text.cbegin(); c != text.cend(); c++)
            //{
            //    Character ch = LoadCharacter(*c);

            //    float xpos = x + ch.Bearing[0] * scale;
            //    float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

            //    float w = ch.Size[0] * scale;
            //    float h = ch.Size[1] * scale;

            //    // update VBO for each character
            //    float vertices[6][4] = {
            //        { xpos,     ypos + h,   0.0f, 0.0f },
            //        { xpos,     ypos,       0.0f, 1.0f },
            //        { xpos + w, ypos,       1.0f, 1.0f },

            //        { xpos,     ypos + h,   0.0f, 0.0f },
            //        { xpos + w, ypos,       1.0f, 1.0f },
            //        { xpos + w, ypos + h,   1.0f, 0.0f }
            //    };
            //    // render glyph texture over quad
            //    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            //    // update content of VBO memory
            //    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            //    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            //    glBindBuffer(GL_ARRAY_BUFFER, 0);
            //    // render quad
            //    glDrawArrays(GL_TRIANGLES, 0, 6);
            //    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            //    x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            //}
            //glBindVertexArray(0);
            //glBindTexture(GL_TEXTURE_2D, 0);
            //glBindBuffer(GL_ARRAY_BUFFER, 0);

            //glDeleteVertexArrays(1, &m_VAO);
            //glDeleteBuffers(1, &m_VBO);
        }

    protected:
        //Character& LoadCharacter(int c)
        //{
        //    auto iter = m_characters.find(c);
        //    if (iter == m_characters.end())
        //    {
        //        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
        //        {
        //            // log warning:  Failed to load Glyph
        //            if (FT_Load_Char(m_face, '?', FT_LOAD_RENDER))
        //            {
        //                // log error: unrecoverable error
        //                // throw
        //            }
        //        }
        //        // generate texture
        //        unsigned int texture;
        //        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //        glGenTextures(1, &texture);
        //        glBindTexture(GL_TEXTURE_2D, texture);
        //        glTexImage2D(
        //            GL_TEXTURE_2D,
        //            0,
        //            GL_RED,
        //            m_face->glyph->bitmap.width,
        //            m_face->glyph->bitmap.rows,
        //            0,
        //            GL_RED,
        //            GL_UNSIGNED_BYTE,
        //            m_face->glyph->bitmap.buffer
        //        );
        //        // set texture options
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //        // now store character for later use
        //        iter = m_characters.emplace(std::make_pair(c, Character(
        //            texture,
        //            Vector2i(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
        //            Vector2i(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
        //            (int)m_face->glyph->advance.x
        //        ))).first;
        //    }
        //    return iter->second;
        //}
    private:
        FT_Library& m_ft;
        FT_Face m_face;
        //std::map<int, Character> m_characters;
        int m_height;
        Shader m_shader;
        //unsigned int m_VAO, m_VBO;
    };

    Font::Font(const std::string& filename)
        : m_imp(std::make_unique<FontImp>(filename))
    {}

    Font::~Font() = default;

    void Font::RenderText(const std::string& text, float x, float y, float scale_x, float scale_y, RGBAColorf color)
    {
        m_imp->RenderText(text, x, y, scale_x, scale_y, color);
    }
}