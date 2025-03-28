#include "tga.h"

Image ReadTGA(std::string file) {
    std::ifstream infile("input/" + file + ".tga", std::ios::binary);

    if (!infile) {
        std::cout << "ERROR INFILE" << std::endl;
    }

    char headerIdLength;
    char headerColorMapType;
    char headerDataTypeCode;
    short headerColorMapOrigin;
    short headerColorMapLength;
    char headerColorMapDepth;
    short headerXOrigin;
    short headerYOrigin;
    short headerWidth;
    short headerHeight;
    char headerBitsPerPixel;
    char headerImageDescriptor;

    infile.read(reinterpret_cast<char*>(&headerIdLength), sizeof(headerIdLength));
    infile.read(reinterpret_cast<char*>(&headerColorMapType), sizeof(headerColorMapType));
    infile.read(reinterpret_cast<char*>(&headerDataTypeCode), sizeof(headerDataTypeCode));
    infile.read(reinterpret_cast<char*>(&headerColorMapOrigin), sizeof(headerColorMapOrigin));
    infile.read(reinterpret_cast<char*>(&headerColorMapLength), sizeof(headerColorMapLength));
    infile.read(reinterpret_cast<char*>(&headerColorMapDepth), sizeof(headerColorMapDepth));
    infile.read(reinterpret_cast<char*>(&headerXOrigin), sizeof(headerXOrigin));
    infile.read(reinterpret_cast<char*>(&headerYOrigin), sizeof(headerYOrigin));
    infile.read(reinterpret_cast<char*>(&headerWidth), sizeof(headerWidth));
    infile.read(reinterpret_cast<char*>(&headerHeight), sizeof(headerHeight));
    infile.read(reinterpret_cast<char*>(&headerBitsPerPixel), sizeof(headerBitsPerPixel));
    infile.read(reinterpret_cast<char*>(&headerImageDescriptor), sizeof(headerImageDescriptor));

    Header header(headerIdLength, headerColorMapType, headerDataTypeCode, headerColorMapOrigin,
                  headerColorMapLength, headerColorMapDepth, headerXOrigin, headerYOrigin,
                  headerWidth, headerHeight, headerBitsPerPixel, headerImageDescriptor);

    unsigned int pixelCount = headerWidth * headerHeight;
    std::vector<Pixel> imagePixels;

    for (int i = 0; i < pixelCount; i++) {
        unsigned char blue;
        unsigned char green;
        unsigned char red;

        infile.read(reinterpret_cast<char*>(&blue), 1);
        infile.read(reinterpret_cast<char*>(&green), 1);
        infile.read(reinterpret_cast<char*>(&red), 1);

        Pixel p(blue, green, red);
        imagePixels.push_back(p);
    }

    return Image(header, imagePixels);
}

void WriteFile(Image img, std::string filename) {
    std::ofstream outfile("output/" + filename + ".tga", std::ios::binary);

    if (!outfile) {
        std::cout << "ERROR OUTFILE" << std::endl;
    }

    outfile.write(reinterpret_cast<const char*>(&img.header.idLength), sizeof(img.header.idLength));
    outfile.write(reinterpret_cast<const char*>(&img.header.colorMapType), sizeof(img.header.colorMapType));
    outfile.write(reinterpret_cast<const char*>(&img.header.dataTypeCode), sizeof(img.header.dataTypeCode));
    outfile.write(reinterpret_cast<const char*>(&img.header.colorMapOrigin), sizeof(img.header.colorMapOrigin));
    outfile.write(reinterpret_cast<const char*>(&img.header.colorMapLength), sizeof(img.header.colorMapLength));
    outfile.write(reinterpret_cast<const char*>(&img.header.colorMapDepth), sizeof(img.header.colorMapDepth));
    outfile.write(reinterpret_cast<const char*>(&img.header.xOrigin), sizeof(img.header.xOrigin));
    outfile.write(reinterpret_cast<const char*>(&img.header.yOrigin), sizeof(img.header.yOrigin));
    outfile.write(reinterpret_cast<const char*>(&img.header.width), sizeof(img.header.width));
    outfile.write(reinterpret_cast<const char*>(&img.header.height), sizeof(img.header.height));
    outfile.write(reinterpret_cast<const char*>(&img.header.bitsPerPixel), sizeof(img.header.bitsPerPixel));
    outfile.write(reinterpret_cast<const char*>(&img.header.imageDescriptor), sizeof(img.header.imageDescriptor));

    for (size_t i = 0; i < img.pixelData.size(); i++) {
        outfile.write(reinterpret_cast<const char*>(&img.pixelData[i].BLUE), 1);
        outfile.write(reinterpret_cast<const char*>(&img.pixelData[i].GREEN), 1);
        outfile.write(reinterpret_cast<const char*>(&img.pixelData[i].RED), 1);
    }
}

Image operator*(Image img1, Image img2) {
    std::vector<Pixel> imagePixels;

    for (size_t i = 0; i < img1.pixelData.size(); i++) {
        unsigned char blue = static_cast<unsigned char>((img1.pixelData[i].BLUE / 255.0f) * (img2.pixelData[i].BLUE / 255.0f) * 255.0f + 0.5f);
        unsigned char green = static_cast<unsigned char>((img1.pixelData[i].GREEN / 255.0f) * (img2.pixelData[i].GREEN / 255.0f) * 255.0f + 0.5f);
        unsigned char red = static_cast<unsigned char>((img1.pixelData[i].RED / 255.0f) * (img2.pixelData[i].RED / 255.0f) * 255.0f + 0.5f);
        
        Pixel p(blue, green, red);
        imagePixels.push_back(p);
    }

    return Image(img1.header, imagePixels);
}

Image operator-(Image img1, Image img2) {
    std::vector<Pixel> imagePixels;

    for (size_t i = 0; i < img1.pixelData.size(); i++) {
        unsigned char blue = std::max(0, img1.pixelData[i].BLUE - img2.pixelData[i].BLUE);
        unsigned char green = std::max(0, img1.pixelData[i].GREEN - img2.pixelData[i].GREEN);
        unsigned char red = std::max(0, img1.pixelData[i].RED - img2.pixelData[i].RED);

        Pixel p(blue, green, red);
        imagePixels.push_back(p);
    }

    return Image(img1.header, imagePixels);
}

Image operator+(Image img1, Image img2) {
    std::vector<Pixel> imagePixels;

    for (size_t i = 0; i < img1.pixelData.size(); i++) {
        unsigned char blue = std::min(255, img1.pixelData[i].BLUE + img2.pixelData[i].BLUE);
        unsigned char green = std::min(255, img1.pixelData[i].GREEN + img2.pixelData[i].GREEN);
        unsigned char red = std::min(255, img1.pixelData[i].RED + img2.pixelData[i].RED);

        Pixel p(blue, green, red);
        imagePixels.push_back(p);
    }

    return Image(img1.header, imagePixels);
}

Image screen(Image img1, Image img2) {
    std::vector<Pixel> imagePixels;

    for (size_t i = 0; i < img1.pixelData.size(); i++) {
        float blue1 = img1.pixelData[i].BLUE / 255.0f;
        float blue2 = img2.pixelData[i].BLUE / 255.0f;
        unsigned char blue = static_cast<unsigned char>((1 - (1 - blue1) * (1 - blue2)) * 255.0f + 0.5f);

        float green1 = img1.pixelData[i].GREEN / 255.0f;
        float green2 = img2.pixelData[i].GREEN / 255.0f;
        unsigned char green = static_cast<unsigned char>((1 - (1 - green1) * (1 - green2)) * 255.0f + 0.5f);

        float red1 = img1.pixelData[i].RED / 255.0f;
        float red2 = img2.pixelData[i].RED / 255.0f;
        unsigned char red = static_cast<unsigned char>((1 - (1 - red1) * (1 - red2)) * 255.0f + 0.5f);

        Pixel pixel(blue, green, red);
        imagePixels.push_back(pixel);
    }

    return Image(img1.header, imagePixels);
}