#ifndef PNG_METADATA_READER_H
#define PNG_METADATA_READER_H

#include <png.h>
#include <wx/string.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


#if wxCHECK_VERSION(3, 1, 0)

#else
    namespace std {
        template <>
        struct hash<wxString> {
            size_t operator()(const wxString& key) const {
                return std::hash<std::string>()(key.ToStdString());
            }
        };
    }
#endif

class PngMetadataReader {
public:
    static std::unordered_map<wxString, wxString> ReadMetadata(const std::string& filepath) {
        std::unordered_map<wxString, wxString, std::hash<wxString>> metadata;

        FILE* fp = fopen(filepath.c_str(), "rb");
        if (!fp) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        png_byte header[8];
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8)) {
            fclose(fp);
            throw std::runtime_error("File is not a valid PNG: " + filepath);
        }

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            fclose(fp);
            throw std::runtime_error("Failed to create PNG read struct");
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            fclose(fp);
            throw std::runtime_error("Failed to create PNG info struct");
        }

        if (setjmp(png_jmpbuf(png))) {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(fp);
            throw std::runtime_error("Error during PNG processing");
        }

        png_init_io(png, fp);
        png_set_sig_bytes(png, 8);  // We already read 8 bytes
        png_read_info(png, info);

        png_textp text_ptr;
        int num_text;
        if (png_get_text(png, info, &text_ptr, &num_text) > 0) {
            for (int i = 0; i < num_text; ++i) {
                wxString key = wxString::FromUTF8(text_ptr[i].key);
                wxString value;

                if (text_ptr[i].compression == PNG_TEXT_COMPRESSION_NONE || 
                    text_ptr[i].compression == PNG_TEXT_COMPRESSION_zTXt ||
                    text_ptr[i].compression == PNG_ITXT_COMPRESSION_NONE ||
                    text_ptr[i].compression == PNG_ITXT_COMPRESSION_zTXt) {
                    value = wxString::FromUTF8(text_ptr[i].text);
                } else {
                    throw std::runtime_error("Unsupported compression type in metadata: " + std::to_string(text_ptr[i].compression));
                }

                metadata[key] = value;
            }
        }

        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);

        return metadata;
    }

    static void WriteMetadata(const std::string& filepath, const std::unordered_map<wxString, wxString>& newMeta) {
        auto existingMeta = ReadMetadata(filepath);

        for (const auto& pair : newMeta) {
            existingMeta[pair.first] = pair.second;
        }

        FILE* input = fopen(filepath.c_str(), "rb");
        if (!input) {
            throw std::runtime_error("Failed to open file for reading: " + filepath);
        }

        png_structp readPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop readInfo  = png_create_info_struct(readPng);

        if (setjmp(png_jmpbuf(readPng))) {
            png_destroy_read_struct(&readPng, &readInfo, nullptr);
            fclose(input);
            throw std::runtime_error("Error during PNG reading");
        }

        png_init_io(readPng, input);
        png_read_info(readPng, readInfo);

        const png_uint_32 width   = png_get_image_width(readPng, readInfo);
        const png_uint_32 height  = png_get_image_height(readPng, readInfo);
        const png_byte color_type = png_get_color_type(readPng, readInfo);
        const png_byte bit_depth  = png_get_bit_depth(readPng, readInfo);

        png_read_update_info(readPng, readInfo);

        png_bytep* rowPointers = new png_bytep[height];
        for (png_uint_32 y = 0; y < height; ++y) {
            rowPointers[y] = static_cast<png_bytep>(malloc(png_get_rowbytes(readPng, readInfo)));
        }

        png_read_image(readPng, rowPointers);
        fclose(input);

        FILE* output = fopen(filepath.c_str(), "wb");
        if (!output) {
            png_destroy_read_struct(&readPng, &readInfo, nullptr);
            for (png_uint_32 y = 0; y < height; ++y) {
                free(rowPointers[y]);
            }
            throw std::runtime_error("Failed to open file for writing: " + filepath);
        }

        png_structp writePng = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop writeInfo  = png_create_info_struct(writePng);

        if (setjmp(png_jmpbuf(writePng))) {
            png_destroy_write_struct(&writePng, &writeInfo);
            fclose(output);
            for (png_uint_32 y = 0; y < height; ++y) {
                free(rowPointers[y]);
            }
            throw std::runtime_error("Error during PNG writing");
        }

        png_init_io(writePng, output);

        png_set_IHDR(writePng, writeInfo, width, height, bit_depth, color_type,
                     png_get_interlace_type(readPng, readInfo),
                     png_get_compression_type(readPng, readInfo),
                     png_get_filter_type(readPng, readInfo));

        png_write_info(writePng, writeInfo);

        if (!existingMeta.empty()) {
            std::vector<png_text> textChunks;
            for (const auto& pair : existingMeta) {
                png_text text;
                text.compression = PNG_TEXT_COMPRESSION_NONE;
                text.key         = const_cast<char*>(pair.first.ToUTF8().data());
                text.text        = const_cast<char*>(pair.second.ToUTF8().data());
                text.text_length = pair.second.ToUTF8().length();
                textChunks.push_back(text);
            }
            png_set_text(writePng, writeInfo, textChunks.data(), static_cast<int>(textChunks.size()));
            png_write_info(writePng, writeInfo);
        }

        png_write_image(writePng, rowPointers);
        png_write_end(writePng, nullptr);

        fclose(output);
        png_destroy_read_struct(&readPng, &readInfo, nullptr);
        png_destroy_write_struct(&writePng, &writeInfo);
        for (png_uint_32 y = 0; y < height; ++y) {
            free(rowPointers[y]);
        }
    }
};

#endif  // PNG_METADATA_READER_H
