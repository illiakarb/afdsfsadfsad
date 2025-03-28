#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

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
};

struct Pixel {
    unsigned char B;
    unsigned char G;
    unsigned char R;
};

struct Image {
    Header header;
    vector<Pixel> pixels;
};

Image readImage(const string& path) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open input file: " << path << endl;
        exit(1);
    }

    Header h;
    file.read(&h.idLength, 1);
    file.read(&h.colorMapType, 1);
    file.read(&h.dataTypeCode, 1);
    file.read(reinterpret_cast<char*>(&h.colorMapOrigin), 2);
    file.read(reinterpret_cast<char*>(&h.colorMapLength), 2);
    file.read(&h.colorMapDepth, 1);
    file.read(reinterpret_cast<char*>(&h.xOrigin), 2);
    file.read(reinterpret_cast<char*>(&h.yOrigin), 2);
    file.read(reinterpret_cast<char*>(&h.width), 2);
    file.read(reinterpret_cast<char*>(&h.height), 2);
    file.read(&h.bitsPerPixel, 1);
    file.read(&h.imageDescriptor, 1);

    vector<Pixel> pixels(h.width * h.height);
    for (int i = 0; i < pixels.size(); i++) {
        file.read(reinterpret_cast<char*>(&pixels[i].B), 1);
        file.read(reinterpret_cast<char*>(&pixels[i].G), 1);
        file.read(reinterpret_cast<char*>(&pixels[i].R), 1);
    }

    Image img;
    img.header = h;
    img.pixels = pixels;
    return img;
}

void writeImage(const Image& img, const string& path) {
    ofstream file(path, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to create output file: " << path << endl;
        exit(1);
    }

    file.write(&img.header.idLength, 1);
    file.write(&img.header.colorMapType, 1);
    file.write(&img.header.dataTypeCode, 1);
    file.write(reinterpret_cast<const char*>(&img.header.colorMapOrigin), 2);
    file.write(reinterpret_cast<const char*>(&img.header.colorMapLength), 2);
    file.write(&img.header.colorMapDepth, 1);
    file.write(reinterpret_cast<const char*>(&img.header.xOrigin), 2);
    file.write(reinterpret_cast<const char*>(&img.header.yOrigin), 2);
    file.write(reinterpret_cast<const char*>(&img.header.width), 2);
    file.write(reinterpret_cast<const char*>(&img.header.height), 2);
    file.write(&img.header.bitsPerPixel, 1);
    file.write(&img.header.imageDescriptor, 1);

    for (int i = 0; i < img.pixels.size(); i++) {
        file.write(reinterpret_cast<const char*>(&img.pixels[i].B), 1);
        file.write(reinterpret_cast<const char*>(&img.pixels[i].G), 1);
        file.write(reinterpret_cast<const char*>(&img.pixels[i].R), 1);
    }
}

Image multiply(const Image& a, const Image& b) {
    vector<Pixel> result;
    for (int i = 0; i < a.pixels.size(); i++) {
        Pixel px;
        px.B = static_cast<unsigned char>((a.pixels[i].B / 255.0f) * (b.pixels[i].B / 255.0f) * 255 + 0.5f);
        px.G = static_cast<unsigned char>((a.pixels[i].G / 255.0f) * (b.pixels[i].G / 255.0f) * 255 + 0.5f);
        px.R = static_cast<unsigned char>((a.pixels[i].R / 255.0f) * (b.pixels[i].R / 255.0f) * 255 + 0.5f);
        result.push_back(px);
    }
    
    Image out;
    out.header = a.header;
    out.pixels = result;
    return out;
}

Image subtract(const Image& base, const Image& layer) {
    vector<Pixel> result;
    for (int i = 0; i < base.pixels.size(); i++) {
        Pixel px;
        px.B = max(0, base.pixels[i].B - layer.pixels[i].B);
        px.G = max(0, base.pixels[i].G - layer.pixels[i].G);
        px.R = max(0, base.pixels[i].R - layer.pixels[i].R);
        result.push_back(px);
    }
    
    Image out;
    out.header = base.header;
    out.pixels = result;
    return out;
}

Image screen(const Image& a, const Image& b) {
    vector<Pixel> result;
    for (int i = 0; i < a.pixels.size(); i++) {
        Pixel px;
        px.B = static_cast<unsigned char>((1 - (1 - a.pixels[i].B/255.0f)*(1 - b.pixels[i].B/255.0f)) * 255 + 0.5f);
        px.G = static_cast<unsigned char>((1 - (1 - a.pixels[i].G/255.0f)*(1 - b.pixels[i].G/255.0f)) * 255 + 0.5f);
        px.R = static_cast<unsigned char>((1 - (1 - a.pixels[i].R/255.0f)*(1 - b.pixels[i].R/255.0f)) * 255 + 0.5f);
        result.push_back(px);
    }
    
    Image out;
    out.header = a.header;
    out.pixels = result;
    return out;
}

Image overlay(const Image& base, const Image& layer) {
    vector<Pixel> result;
    for (int i = 0; i < base.pixels.size(); i++) {
        float (*calc)(float, float) = [](float b, float l) {
            if (l <= 0.5) return 2 * b * l;
            else return 1 - 2 * (1 - b) * (1 - l);
        };
        
        Pixel px;
        px.B = static_cast<unsigned char>(calc(base.pixels[i].B/255.0f, layer.pixels[i].B/255.0f) * 255 + 0.5f);
        px.G = static_cast<unsigned char>(calc(base.pixels[i].G/255.0f, layer.pixels[i].G/255.0f) * 255 + 0.5f);
        px.R = static_cast<unsigned char>(calc(base.pixels[i].R/255.0f, layer.pixels[i].R/255.0f) * 255 + 0.5f);
        result.push_back(px);
    }
    
    Image out;
    out.header = base.header;
    out.pixels = result;
    return out;
}

int main() {
    Image layer1 = readImage("input/layer1.tga");
    Image layer2 = readImage("input/layer2.tga");
    Image pattern1 = readImage("input/pattern1.tga");
    Image pattern2 = readImage("input/pattern2.tga");
    Image car = readImage("input/car.tga");
    Image circles = readImage("input/circles.tga");
    Image text = readImage("input/text.tga");
    Image text2 = readImage("input/text2.tga");
    Image layer_blue = readImage("input/layer_blue.tga");
    Image layer_green = readImage("input/layer_green.tga");
    Image layer_red = readImage("input/layer_red.tga");

    // Part 1: Multiply layer1 with pattern1
    Image part1 = multiply(layer1, pattern1);
    writeImage(part1, "output/part1.tga");

    // Part 2: Subtract layer2 from car
    Image part2 = subtract(car, layer2);
    writeImage(part2, "output/part2.tga");

    // Part 3: Screen text with multiplied layer1 and pattern2
    Image multiplied = multiply(layer1, pattern2);
    Image part3 = screen(multiplied, text);
    writeImage(part3, "output/part3.tga");

    // Part 4: Multiply layer2 with circles then subtract pattern2
    Image multiplied2 = multiply(layer2, circles);
    Image part4 = subtract(multiplied2, pattern2);
    writeImage(part4, "output/part4.tga");

    // Part 5: Overlay layer1 with pattern1
    Image part5 = overlay(layer1, pattern1);
    writeImage(part5, "output/part5.tga");

    // Part 6: Add 200 to green channel of car (clamped to 255)
    Image part6 = car;
    for (int i = 0; i < part6.pixels.size(); i++) {
        part6.pixels[i].G = min(255, part6.pixels[i].G + 200);
    }
    writeImage(part6, "output/part6.tga");

    // Part 7: Scale red by 4 and zero blue in car image
    Image part7 = car;
    for (int i = 0; i < part7.pixels.size(); i++) {
        part7.pixels[i].R = min(255, part7.pixels[i].R * 4);
        part7.pixels[i].B = 0;
    }
    writeImage(part7, "output/part7.tga");

    // Part 8: Separate car into individual color channels
    Image part8_b, part8_g, part8_r;
    part8_b.header = part8_g.header = part8_r.header = car.header;
    for (int i = 0; i < car.pixels.size(); i++) {
        Pixel b_px = {car.pixels[i].B, car.pixels[i].B, car.pixels[i].B};
        Pixel g_px = {car.pixels[i].G, car.pixels[i].G, car.pixels[i].G};
        Pixel r_px = {car.pixels[i].R, car.pixels[i].R, car.pixels[i].R};
        part8_b.pixels.push_back(b_px);
        part8_g.pixels.push_back(g_px);
        part8_r.pixels.push_back(r_px);
    }
    writeImage(part8_b, "output/part8_b.tga");
    writeImage(part8_g, "output/part8_g.tga");
    writeImage(part8_r, "output/part8_r.tga");

    // Part 9: Combine color layers
    Image part9;
    part9.header = layer_blue.header;
    for (int i = 0; i < layer_blue.pixels.size(); i++) {
        Pixel px = {layer_blue.pixels[i].B, layer_green.pixels[i].G, layer_red.pixels[i].R};
        part9.pixels.push_back(px);
    }
    writeImage(part9, "output/part9.tga");

    // Part 10: Flip text2 vertically
    reverse(text2.pixels.begin(), text2.pixels.end());
    writeImage(text2, "output/part10.tga");

    return 0;
}
