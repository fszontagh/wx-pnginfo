#include <iostream>
#include "PngMetadataReader.h"

void printUsage(const std::string& programName) {
    std::cerr << "Usage:\n"
              << programName << " <png_file> [key=value ...]\n"
              << "If key-value pairs are provided, they will be written to the PNG file.\n"
              << "Otherwise, metadata will be read from the PNG file." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string filepath = argv[1];

    try {
        if (argc == 2) {
            auto metadata = PngMetadataReader::ReadMetadata(filepath);

            if (metadata.empty()) {
                std::cout << "No metadata found in the PNG file." << std::endl;
            } else {
                std::cout << "Metadata from PNG file:" << std::endl;
                for (const auto& pair : metadata) {
                    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
                }
            }
        } else {
            std::unordered_map<wxString, wxString> newMeta;
            for (int i = 2; i < argc; ++i) {
                std::string arg = argv[i];
                size_t eqPos = arg.find('=');
                if (eqPos == std::string::npos) {
                    std::cerr << "Invalid metadata format: " << arg << std::endl;
                    return 1;
                }

                wxString key = wxString::FromUTF8(arg.substr(0, eqPos).c_str());
                wxString value = wxString::FromUTF8(arg.substr(eqPos + 1).c_str());
                newMeta[key] = value;
            }

            PngMetadataReader::WriteMetadata(filepath, newMeta);
            std::cout << "Metadata successfully written to the PNG file." << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
