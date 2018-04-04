#ifndef SHAPE_H
#define SHAPE_H

#include "models.h"

class Shape {
    private:
        Point _center;
        BoundingBox _boundingBox;

    public:
        Shape()=default;
        virtual ~Shape() = default;
};

#endif // SHAPE_H