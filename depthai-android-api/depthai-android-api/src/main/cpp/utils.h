//
// Created by ibaig on 12/28/2021.
//

#ifndef DEPTHAI_ANDROID_API_UTILS_H
#define DEPTHAI_ANDROID_API_UTILS_H

void colorDisparity(uint8_t *colorDisparity, uint8_t disparity, uint8_t maxDisparity) {
    // Ref: https://www.particleincell.com/2014/colormap

    uint8_t r, g, b;

    auto a = (1.0f - (float) disparity / maxDisparity) * 5.0f;
    auto X = (uint8_t) floor(a);
    auto Y = (uint8_t) (255 * (a - X));
    switch (X) {
        case 0:
            r = 255; g = Y; b = 0;
            break;
        case 1:
            r = 255 - Y; g = 255; b = 0;
            break;
        case 2:
            r = 0; g = 255; b = Y;
            break;
        case 3:
            r = 0; g = 255 - Y; b = 255;
            break;
        case 4:
            r = Y; g = 0; b = 255;
            break;
        case 5:
            r = 255; g = 0; b = 255;
            break;
        default:
            r = 0; g = 0; b = 0;
            break;
    }

    colorDisparity[0] = r;
    colorDisparity[1] = g;
    colorDisparity[2] = b;
}

#endif //DEPTHAI_ANDROID_API_UTILS_H
