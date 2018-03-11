#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "triple.h"
#include "image.h"

class Material
{
    public:
        Color color;        // base color
        Image texture;      // Texture
        double ka;          // ambient intensity
        double kd;          // diffuse intensity
        double ks;          // specular intensity
        double n;           // exponent for specular highlight size

        Material() = default;

        Material(Color const &color, Image texture, double ka, double kd, double ks, double n)
        :
            color(color),
            texture(texture),
            ka(ka),
            kd(kd),
            ks(ks),
            n(n)
        {}
};

#endif
