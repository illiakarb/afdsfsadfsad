#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;

    Header() {
        idLength = 0;
        colorMapType = 0;
        dataTypeCode = 0;
        colorMapOrigin = 0;
        colorMapLength = 0;
        colorMapDepth = 0;
        xOrigin = 0;
        yOrigin = 0;
        width = 0;
        height = 0;
        bitsPerPixel = 0;
        imageDescriptor = 0;
    }

    Header(char idLength, char colorMapType, char dataTypeCode, short colorMapOrigin,
           short colorMapLength, char colorMapDepth, short xOrigin, short yOrigin,
           short width, short height, char bitsPerPixel, char imageDescriptor) {
        this->idLength = idLength;
        this->colorMapType = colorMapType;
        this->dataTypeCode = dataTypeCode;
        this->colorMapOrigin = colorMapOrigin;
        this->colorMapLength = colorMapLength;
        this->colorMapDepth = colorMapDepth;
        this->xOrigin = xOrigin;
        this->yOrigin = yOrigin;
        this->width = width;
        this->height = height;
        this->bitsPerPixel = bitsPerPixel;
        this->imageDescriptor = imageDescriptor;
    }
};

struct Pixel {
    unsigned char BLUE;
    unsigned char GREEN;
    unsigned char RED;

    Pixel() {
        BLUE = 0;
        GREEN = 0;
        RED = 0;
    }

    Pixel(unsigned char BLUE, unsigned char GREEN, unsigned char RED) {
        this->BLUE = BLUE;
        this->GREEN = GREEN;
        this->RED = RED;
    }
};

struct Image {
    Header header;
    std::vector<Pixel> pixelData;

    Image() {
        header = Header();
        pixelData = std::vector<Pixel>();
    }

    Image(Header header, std::vector<Pixel> pixelData) {
        this->header = header;
        this->pixelData = pixelData;
    }
};

Header readHeader(std::string file);
Image ReadTGA(std::string file);
void displayHeader(Header header);
void displayPixels(std::vector<Pixel> pixels);

Image operator*(Image img1, Image img2);
Image operator-(Image img1, Image img2);
Image operator+(Image img1, Image img2);
Image screen(Image img1, Image img2);
Image overlay(Image img1, Image img2);

void WriteFile(Image image, std::string filename);