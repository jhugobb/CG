#ifndef OBJECT_H_
#define OBJECT_H_

#include "material.h"
#include "point2D.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "triple.h"

#include <memory>
#include <cassert>
#include <iostream>

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object
{
    public:
        Material material;

        virtual ~Object() = default;

        virtual Hit intersect(Ray const &ray) = 0;  // must be implemented
                                                    // in derived class
        virtual Point2D textureCoords(Point const &p) = 0;

        Color getColor(Point const &p) {
            if (material.color.r != -1 || material.color.g != -1 || material.color.b != -1)
            {
                return material.color;
            }
            assert(material.texture.width() > 0 && material.texture.height() > 0);
            Point2D textCoor = textureCoords(p);

            //std::cout << "\nThe texture has been loaded the following dimension " << material.texture.d_pixels.size() << "\n";
            //std::cout << "\nCoor: " << textCoor.x << " X " << textCoor.y << "\n";
            return material.texture.colorAt(textCoor.x, textCoor.y);
        }
};

#endif
