#include "FontImporter.h"
#include "Log.h"
#include <vector>

namespace Dive
{
    // Properties of the texture font atlas which holds all visible ASCII characters
    static const uint32_t GLYPH_START = 32;
    static const uint32_t GLYPH_END = 127;
    static const uint32_t ATLAS_WIDTH = 512;

    static FT_UInt32 g_glyph_load_flags = 0;

    // FreeType questionable design, but it's free, so we just write this namespace and forget about it
    namespace ft_helper
    {
        struct ft_bitmap
        {
            ~ft_bitmap()
            {
                if (buffer)
                {
                    delete buffer;
                    buffer = nullptr;
                }
            }

            uint32_t width = 0;
            uint32_t height = 0;
            unsigned char pixel_mode = 0;
            unsigned char* buffer = nullptr;
        };

        inline bool handle_error(int error_code)
        {
            if (error_code == FT_Err_Ok)
                return true;

            switch (error_code)
            {
                // Generic errors
            case FT_Err_Cannot_Open_Resource:           CORE_ERROR("FreeType: Cannot open resource."); break;
            case FT_Err_Unknown_File_Format:            CORE_ERROR("FreeType: Unknown file format."); break;
            case FT_Err_Invalid_File_Format:            CORE_ERROR("FreeType: Broken file."); break;
            case FT_Err_Invalid_Version:                CORE_ERROR("FreeType: Invalid FreeType version."); break;
            case FT_Err_Lower_Module_Version:           CORE_ERROR("FreeType: Module version is too low."); break;
            case FT_Err_Invalid_Argument:               CORE_ERROR("FreeType: Invalid argument."); break;
            case FT_Err_Unimplemented_Feature:          CORE_ERROR("FreeType: Unimplemented feature."); break;
            case FT_Err_Invalid_Table:                  CORE_ERROR("FreeType: Invalid table."); break;
            case FT_Err_Invalid_Offset:                 CORE_ERROR("FreeType: Invalid offset."); break;
            case FT_Err_Array_Too_Large:                CORE_ERROR("FreeType: Array allocation size too large."); break;
            case FT_Err_Missing_Module:                 CORE_ERROR("FreeType: Missing module."); break;
            case FT_Err_Missing_Property:               CORE_ERROR("FreeType: Missing property."); break;
                // Glyph/character errors
            case FT_Err_Invalid_Glyph_Index:            CORE_ERROR("FreeType: Invalid glyph index."); break;
            case FT_Err_Invalid_Character_Code:         CORE_ERROR("FreeType: Invalid character code."); break;
            case FT_Err_Invalid_Glyph_Format:           CORE_ERROR("FreeType: Unsupported glyph format."); break;
            case FT_Err_Cannot_Render_Glyph:            CORE_ERROR("FreeType: Cannot render this glyph format."); break;
            case FT_Err_Invalid_Outline:                CORE_ERROR("FreeType: Invalid outline."); break;
            case FT_Err_Invalid_Composite:              CORE_ERROR("FreeType: Invalid composite glyph."); break;
            case FT_Err_Too_Many_Hints:                 CORE_ERROR("FreeType: Too many hints."); break;
            case FT_Err_Invalid_Pixel_Size:             CORE_ERROR("FreeType: Invalid pixel size."); break;
                // Handle errors
            case FT_Err_Invalid_Handle:                 CORE_ERROR("FreeType: Invalid object handle."); break;
            case FT_Err_Invalid_Library_Handle:         CORE_ERROR("FreeType: Invalid library handle."); break;
            case FT_Err_Invalid_Driver_Handle:          CORE_ERROR("FreeType: Invalid module handle."); break;
            case FT_Err_Invalid_Face_Handle:            CORE_ERROR("FreeType: Invalid face handle."); break;
            case FT_Err_Invalid_Size_Handle:            CORE_ERROR("FreeType: Invalid size handle."); break;
            case FT_Err_Invalid_Slot_Handle:            CORE_ERROR("FreeType: Invalid glyph slot handle."); break;
            case FT_Err_Invalid_CharMap_Handle:         CORE_ERROR("FreeType: Invalid charmap handle."); break;
            case FT_Err_Invalid_Cache_Handle:           CORE_ERROR("FreeType: Invalid cache manager handle."); break;
            case FT_Err_Invalid_Stream_Handle:          CORE_ERROR("FreeType: Invalid stream handle."); break;
                // Driver errors
            case FT_Err_Too_Many_Drivers:               CORE_ERROR("FreeType: Too many modules."); break;
            case FT_Err_Too_Many_Extensions:            CORE_ERROR("FreeType: Too many extensions."); break;
                // Memory errors
            case FT_Err_Out_Of_Memory:                  CORE_ERROR("FreeType: Out of memory."); break;
            case FT_Err_Unlisted_Object:                CORE_ERROR("FreeType: Unlisted object."); break;
                // Stream errors
            case FT_Err_Cannot_Open_Stream:             CORE_ERROR("FreeType: Cannot open stream."); break;
            case FT_Err_Invalid_Stream_Seek:            CORE_ERROR("FreeType: Invalid stream seek."); break;
            case FT_Err_Invalid_Stream_Skip:            CORE_ERROR("FreeType: Invalid stream skip."); break;
            case FT_Err_Invalid_Stream_Read:            CORE_ERROR("FreeType: Invalid stream read."); break;
            case FT_Err_Invalid_Stream_Operation:       CORE_ERROR("FreeType: Invalid stream operation."); break;
            case FT_Err_Invalid_Frame_Operation:        CORE_ERROR("FreeType: Invalid frame operation."); break;
            case FT_Err_Nested_Frame_Access:            CORE_ERROR("FreeType: Nested frame access."); break;
            case FT_Err_Invalid_Frame_Read:             CORE_ERROR("FreeType: Invalid frame read."); break;
                // Raster errors
            case FT_Err_Raster_Uninitialized:           CORE_ERROR("FreeType: Raster uninitialized."); break;
            case FT_Err_Raster_Corrupted:               CORE_ERROR("FreeType: Raster corrupted."); break;
            case FT_Err_Raster_Overflow:                CORE_ERROR("FreeType: Raster overflow."); break;
            case FT_Err_Raster_Negative_Height:         CORE_ERROR("FreeType: Negative height while rastering."); break;
                // Cache errors
            case FT_Err_Too_Many_Caches:                CORE_ERROR("FreeType: Too many registered caches."); break;
                // TrueType and SFNT errors 
            case FT_Err_Invalid_Opcode:                 CORE_ERROR("FreeType: Invalid opcode."); break;
            case FT_Err_Too_Few_Arguments:              CORE_ERROR("FreeType: Too few arguments."); break;
            case FT_Err_Stack_Overflow:                 CORE_ERROR("FreeType: Stack overflow."); break;
            case FT_Err_Code_Overflow:                  CORE_ERROR("FreeType: Code overflow."); break;
            case FT_Err_Bad_Argument:                   CORE_ERROR("FreeType: Bad argument."); break;
            case FT_Err_Divide_By_Zero:                 CORE_ERROR("FreeType: Division by zero."); break;
            case FT_Err_Invalid_Reference:              CORE_ERROR("FreeType: Invalid reference."); break;
            case FT_Err_Debug_OpCode:                   CORE_ERROR("FreeType: Found debug opcode."); break;
            case FT_Err_ENDF_In_Exec_Stream:            CORE_ERROR("FreeType: Found ENDF opcode in execution stream."); break;
            case FT_Err_Nested_DEFS:                    CORE_ERROR("FreeType: Nested DEFS."); break;
            case FT_Err_Invalid_CodeRange:              CORE_ERROR("FreeType: Invalid code range."); break;
            case FT_Err_Execution_Too_Long:             CORE_ERROR("FreeType: Execution context too long."); break;
            case FT_Err_Too_Many_Function_Defs:         CORE_ERROR("FreeType: Too many function definitions."); break;
            case FT_Err_Too_Many_Instruction_Defs:      CORE_ERROR("FreeType: Too many instruction definitions."); break;
            case FT_Err_Table_Missing:                  CORE_ERROR("FreeType: SFNT font table missing."); break;
            case FT_Err_Horiz_Header_Missing:           CORE_ERROR("FreeType: Horizontal header (hhea) table missing."); break;
            case FT_Err_Locations_Missing:              CORE_ERROR("FreeType: Locations (loca) table missing."); break;
            case FT_Err_Name_Table_Missing:             CORE_ERROR("FreeType: Name table missing."); break;
            case FT_Err_CMap_Table_Missing:             CORE_ERROR("FreeType: Character map (cmap) table missing."); break;
            case FT_Err_Hmtx_Table_Missing:             CORE_ERROR("FreeType: Horizontal metrics (hmtx) table missing."); break;
            case FT_Err_Post_Table_Missing:             CORE_ERROR("FreeType: PostScript (post) table missing."); break;
            case FT_Err_Invalid_Horiz_Metrics:          CORE_ERROR("FreeType: Invalid horizontal metrics."); break;
            case FT_Err_Invalid_CharMap_Format:         CORE_ERROR("FreeType: Invalid character map (cma) format."); break;
            case FT_Err_Invalid_PPem:                   CORE_ERROR("FreeType: Invalid ppem value."); break;
            case FT_Err_Invalid_Vert_Metrics:           CORE_ERROR("FreeType: Invalid vertical metrics."); break;
            case FT_Err_Could_Not_Find_Context:         CORE_ERROR("FreeType: Could not find context."); break;
            case FT_Err_Invalid_Post_Table_Format:      CORE_ERROR("FreeType: Invalid PostScript (post) table format."); break;
            case FT_Err_Invalid_Post_Table:             CORE_ERROR("FreeType: Invalid PostScript (post) table."); break;
            case FT_Err_DEF_In_Glyf_Bytecode:           CORE_ERROR("FreeType: Found FDEF or IDEF opcode in glyf bytecode."); break;
                // CFF, CID, and Type 1 errors 
            case FT_Err_Syntax_Error:                   CORE_ERROR("FreeType: Opcode syntax error."); break;
            case FT_Err_Stack_Underflow:                CORE_ERROR("FreeType: Argument stack underflow."); break;
            case FT_Err_Ignore:                         CORE_ERROR("FreeType: Ignore."); break;
            case FT_Err_No_Unicode_Glyph_Name:          CORE_ERROR("FreeType: No Unicode glyph name found."); break;
            case FT_Err_Glyph_Too_Big:                  CORE_ERROR("FreeType: Glyph too big for hinting."); break;
                // BDF errors
            case FT_Err_Missing_Startfont_Field:        CORE_ERROR("FreeType: 'STARTFONT' field missing."); break;
            case FT_Err_Missing_Font_Field:             CORE_ERROR("FreeType: 'FONT' field missing."); break;
            case FT_Err_Missing_Size_Field:             CORE_ERROR("FreeType: 'SIZE' field missing."); break;
            case FT_Err_Missing_Fontboundingbox_Field:  CORE_ERROR("FreeType: 'FONTBOUNDINGBOX' field missing."); break;
            case FT_Err_Missing_Chars_Field:            CORE_ERROR("FreeType: 'CHARS' field missing."); break;
            case FT_Err_Missing_Startchar_Field:        CORE_ERROR("FreeType: 'STARTCHAR' field missing."); break;
            case FT_Err_Missing_Encoding_Field:         CORE_ERROR("FreeType: 'ENCODING' field missing."); break;
            case FT_Err_Missing_Bbx_Field:              CORE_ERROR("FreeType: 'BBX' field missing."); break;
            case FT_Err_Bbx_Too_Big:                    CORE_ERROR("FreeType: 'BBX' too big."); break;
            case FT_Err_Corrupted_Font_Header:          CORE_ERROR("FreeType: Font header corrupted or missing fields."); break;
            case FT_Err_Corrupted_Font_Glyphs:          CORE_ERROR("FreeType: Font glyphs corrupted or missing fields."); break;
                // None
            default: CORE_ERROR("FreeType: Unknown error code."); break;
            }

            return false;
        }

        inline FT_UInt32 get_load_flags(const Dive_Font* pFont)
        {
            FT_UInt32 flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER;
            /*
            flags |= font->GetForceAutohint() ? FT_LOAD_FORCE_AUTOHINT : 0;

            switch (font->GetHinting())
            {
            case Font_Hinting_None:
                flags |= FT_LOAD_NO_HINTING;
                break;
            case Font_Hinting_Light:
                flags |= FT_LOAD_TARGET_LIGHT;
                break;
            default: // Hinting_Normal
                flags |= FT_LOAD_TARGET_NORMAL;
                break;
            }
            */
            return flags;
        }

        inline bool load_glyph(const FT_Face& face, const uint32_t char_code, const uint32_t flags = g_glyph_load_flags)
        {
            // get_char_index, load_glyph, render_glyph를 하나로 줄인 함수...
            return ft_helper::handle_error(FT_Load_Char(face, char_code, flags));
        }

        inline void get_character_max_dimensions(uint32_t* max_width, uint32_t* max_height, FT_Face& face, const uint32_t outline_size)
        {
            uint32_t width = 0;
            uint32_t height = 0;

            for (uint32_t char_code = GLYPH_START; char_code < GLYPH_END; char_code++)
            {
                // 원래 여기에서 char_code는 출력할 문자이다.
                // 그런데 이 곳에서는 순회하며 ascii 영문자들의 전부를 사용하는 듯 하다. 
                if (!load_glyph(face, char_code))
                    continue;

                // 전용 bitmap 타입이다.
                FT_Bitmap* bitmap = &face->glyph->bitmap;
                //width = Helper::Max<uint32_t>(width, bitmap->width);
                //height = Helper::Max<uint32_t>(height, bitmap->rows);
            }

            *max_width = width + outline_size * 2;
            *max_height = height + outline_size * 2;
        }

        inline void get_texture_atlas_dimensions(uint32_t* atlas_width, uint32_t* atlas_height, uint32_t* atlas_cell_width, uint32_t* atlas_cell_height, FT_Face& face, const uint32_t outline_size)
        {
            uint32_t max_width = 0;
            uint32_t max_height = 0;
            // 위의 함수로 부터 값을 가져온 후
            get_character_max_dimensions(&max_width, &max_height, face, outline_size);

            const uint32_t glyph_count = GLYPH_END - GLYPH_START;
            const uint32_t glyphs_per_row = ATLAS_WIDTH / max_width;
            const uint32_t row_count = 0;// static_cast<uint32_t>(Helper::Ceil(float(glyph_count) / float(glyphs_per_row)));

            // 최종적으로 계산 결과를 전달한다.
            *atlas_width = ATLAS_WIDTH;
            *atlas_height = max_height * row_count;
            *atlas_cell_width = max_width;
            *atlas_cell_height = max_height;
        }
        /*
        inline void get_bitmap(ft_bitmap* bitmap, const Font* font, const FT_Stroker& stroker, FT_Face& ft_font, const uint32_t char_code)
        {
            // Load glyph
            if (!load_glyph(ft_font, char_code, stroker ? FT_LOAD_NO_BITMAP : g_glyph_load_flags))
                return;

            FT_Bitmap* bitmap_temp = nullptr; // will deallocate it's buffer the moment will load another glyph

            // Get bitmap
            if (!stroker)
            {
                bitmap_temp = &ft_font->glyph->bitmap;
            }
            else
            {
                if (ft_font->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                {
                    FT_Glyph glyph;
                    if (handle_error(FT_Get_Glyph(ft_font->glyph, &glyph)))
                    {
                        bool stroked = false;

                        if (font->GetOutline() == Font_Outline_Edge)
                        {
                            stroked = handle_error(FT_Glyph_Stroke(&glyph, stroker, true));
                        }
                        else if (font->GetOutline() == Font_Outline_Positive)
                        {
                            stroked = handle_error(FT_Glyph_StrokeBorder(&glyph, stroker, false, true));
                        }
                        else if (font->GetOutline() == Font_Outline_Negative)
                        {
                            stroked = handle_error(FT_Glyph_StrokeBorder(&glyph, stroker, true, true));
                        }

                        if (stroked)
                        {
                            if (handle_error(FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true)))
                            {
                                bitmap_temp = &reinterpret_cast<FT_BitmapGlyph>(glyph)->bitmap;
                            }
                        }
                    }
                }
                else
                {
                    CORE_ERROR("Can't apply outline as the glyph doesn't have an outline format");
                    bitmap_temp = &ft_font->glyph->bitmap;
                }
            }

            // Copy bitmap
            if (bitmap_temp && bitmap_temp->buffer)
            {
                bitmap->width = bitmap_temp->width;
                bitmap->height = bitmap_temp->rows;
                bitmap->pixel_mode = bitmap_temp->pixel_mode;
                bitmap->buffer = new unsigned char[bitmap->width * bitmap->height];
                memcpy(bitmap->buffer, bitmap_temp->buffer, bitmap->width * bitmap->height);
            }
        }

        inline void copy_to_atlas(vector<std::byte>& atlas, const ft_bitmap& bitmap, const Vector2& pen, const uint32_t atlas_width, const uint32_t outline_size)
        {
            for (uint32_t glyph_y = 0; glyph_y < bitmap.height; glyph_y++)
            {
                for (uint32_t glyph_x = 0; glyph_x < bitmap.width; glyph_x++)
                {
                    // Compute 
                    uint32_t atlas_x = static_cast<uint32_t>(pen.x + glyph_x);
                    uint32_t atlas_y = static_cast<uint32_t>(pen.y + glyph_y);

                    // In case there is an outline, the text has to offset away from the outline's edge
                    atlas_x += outline_size;
                    atlas_y += outline_size;

                    // Ensure we are not doing any wrong math
                    const uint32_t atlas_pos = atlas_x + atlas_y * atlas_width;
                    SP_ASSERT(atlas.size() > atlas_pos);

                    switch (bitmap.pixel_mode)
                    {
                    case FT_PIXEL_MODE_MONO: {
                        // implement if it's ever needed
                    } break;

                    case FT_PIXEL_MODE_GRAY: {
                        atlas[atlas_pos] = static_cast<std::byte>(bitmap.buffer[glyph_x + glyph_y * bitmap.width]);
                    } break;

                    case FT_PIXEL_MODE_BGRA: {
                        // implement if it's ever needed
                    } break;

                    default:
                        CORE_ERROR("Font uses unsupported pixel format");
                        break;
                    }
                }
            }
        }

        // 이 과정도 쉽지 않네...
        inline Glyph get_glyph(const FT_Face& ft_font, const uint32_t char_code, const Vector2& pen, const uint32_t atlas_width, const uint32_t atlas_height, const uint32_t outline_size)
        {
            // The glyph metrics refer to whatever the last loaded glyph was, this is up to the caller of the function
            FT_Glyph_Metrics& metrics = ft_font->glyph->metrics;

            Glyph glyph = {};
            glyph.offset_x = metrics.horiBearingX >> 6;
            glyph.offset_y = metrics.horiBearingY >> 6;
            glyph.width = (metrics.width >> 6) + outline_size * 2;
            glyph.height = (metrics.height >> 6) + outline_size * 2;
            glyph.horizontal_advance = metrics.horiAdvance >> 6;
            glyph.uv_x_left = static_cast<float>(pen.x) / static_cast<float>(atlas_width);
            glyph.uv_x_right = static_cast<float>(pen.x + glyph.width) / static_cast<float>(atlas_width);
            glyph.uv_y_top = static_cast<float>(pen.y) / static_cast<float>(atlas_height);
            glyph.uv_y_bottom = static_cast<float>(pen.y + glyph.height) / static_cast<float>(atlas_height);

            // Kerning is the process of adjusting the position of two subsequent glyph images 
            // in a string of text in order to improve the general appearance of text. 
            // For example, if a glyph for an uppercase ‘A’ is followed by a glyph for an 
            // uppercase ‘V’, the space between the two glyphs can be slightly reduced to 
            // avoid extra ‘diagonal whitespace’.
            if (char_code >= 1 && FT_HAS_KERNING(ft_font))
            {
                FT_Vector kerningVec;
                FT_Get_Kerning(ft_font, char_code - 1, char_code, FT_KERNING_DEFAULT, &kerningVec);
                glyph.horizontal_advance += kerningVec.x >> 6;
            }

            return glyph;
        } */
    }
 
	FontImporter::FontImporter()
	{
		// 라이브러리 초기화
	}

	FontImporter::~FontImporter()
	{
		// 라이브러리 릴리즈
	}
	
	bool FontImporter::ImporterFile(Dive_Font* pFont, const std::string& filepath)
	{
		// 매개변수 확인

		// 임포트

		// 가공

		// 전달

		return false;
	}
}