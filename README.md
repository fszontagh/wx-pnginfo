# PNG Metadata Reader and Writer

This C++ library provides functionality to read and write metadata (such as `tEXt` chunks) in PNG files. It utilizes the [libpng](http://www.libpng.org/) library for handling PNG files. The library supports extracting metadata from PNG files and adding or updating metadata without altering the image data.

## Features

- **Read PNG metadata**: Extract textual metadata from a PNG file (e.g., Description, Author, etc.).
- **Write PNG metadata**: Add or update metadata in an existing PNG file, while preserving the original image data.
- **Compatibility**: Supports PNG files with text chunks (`tEXt`).

## Prerequisites

- C++17 or higher.
- [libpng](http://www.libpng.org/) library (for reading and writing PNG files).
- wxWidgets (for handling wxString objects).

### Installing libpng (Linux)

On a Linux-based system, you can install libpng using your package manager:

```bash
sudo apt-get install libpng-dev
```

### Installing wxWidgets (Linux)

On Ubuntu or Debian, you can install wxWidgets using:

```bash
sudo apt-get install libwxgtk3.0-dev
```

## Building the Project

This project uses [CMake](https://cmake.org/) for building. You can build the project by following these steps:

### 1. Clone the repository:

```bash
git clone https://github.com/fszontagh/wx-pnginfo.git cd wx-pnginfo.git
```

### 2. Create a build directory and run CMake:

```bash
mkdir build 
cd build cmake ..
```

### 3. Build the project:

```bash
make
```

### 4. Running the example:

Once the build completes, you can run the example:

```bash
./png_metadata_reader input.png Description="This is an image"
```

This command will write the metadata to the specified PNG file.

To read the metadata from a PNG file:

```bash
./png_metadata_reader input.png
```

## Example Usage

### Reading Metadata from a PNG

```cpp
#include "PngMetadataReader.h" 

int main() { 
    std::unordered_map<wxString, wxString> metadata = PngMetadataReader::ReadMetadata("image.png"); 
    
    for (const auto& pair : metadata) { 
	    std::cout << pair.first.ToStdString() << ": " << pair.second.ToStdString() << std::endl; 
    } 


return 0; 

}
```

### Writing Metadata to a PNG

```cpp
#include "PngMetadataReader.h" 

int main() { 

    std::unordered_map<wxString, wxString> metadata; metadata["Description"] = "This is an image"; metadata["Author"] = "John Doe"; 

    PngMetadataReader::WriteMetadata("image.png", metadata); 

return 0;

}
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please fork this repository, create a branch for your feature, and submit a pull request.

## Acknowledgments

- [libpng](http://www.libpng.org/) for the core PNG handling
- [wxWidgets](https://www.wxwidgets.org/) for string handling
