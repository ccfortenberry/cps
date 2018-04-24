#define _USE_MATH_DEFINES

#include <algorithm>
using std::max;
#include <cmath>
using std::round;
#include <fstream>
using std::ofstream;
#include <iostream>
using std::cout;
using std::endl;
#include <memory>
using std::make_shared;
#include <ostream>
using std::ostream;
#include <string>
using std::string;
using std::to_string;
#include <sstream>
using std::ostringstream;
#include <vector>
using std::vector;

#include "cps_crtp.hpp"


//****************************** Struct Functions
string BoundingBox::toString() const {
    ostringstream out;
    out << "("<< width << ", " << height << ")";

    return out.str();
}


//******************************* Shape Base
void Shape::scale(double x, double y) {
    transformation += StringTemplate("${x} ${y} scale\n")
        .replace("x", x)
        .replace("y", y)
        .get();

    _boundingBox.scale(x, y);
}

void Shape::translate(double x, double y) {
    transformation += StringTemplate("${x} ${y} translate\n")
        .replace("x", x)
        .replace("y", y)
        .get();
}

void Shape::rotate(int ticks) {
    auto degrees = 90. * ticks;

    transformation += StringTemplate("${degrees} rotate\n")
        .replace("degrees", degrees)
        .get();
}

std::string Shape::getTransform() const {
    return transformation;
}

BoundingBox Shape::getBoundingBox() const {
    return _boundingBox;
}

void Shape::setBoundingBox(BoundingBox bb) {
    _boundingBox = bb;
}


//******************************* Circle
Circle::Circle(double r):
    Shape(),
    _radius(r) {

    setBoundingBox(makeBoundingBox());
};

const BoundingBox Circle::makeBoundingBox() const {
    return BoundingBox(_radius * 2, _radius * 2);
}

string Circle::postscript() {
    int r = round(_radius);
    std::string transforms = this->getTransform();

    std::string circlePsText = R"ps(
        gsave

        ${transformations}

        1 setlinewidth
        0 0 ${radius} 0 360 arc closepath stroke

        grestore
    )ps";

    auto formattedPsOutput = StringTemplate(circlePsText)
        .replace("transformations", transforms)
        .replace("radius", r)
        .get();

    return formattedPsOutput;
}


//*********************************************** Polygon
Polygon::Polygon(const size_type & numSides, const size_type & sideLen)
	: Shape(),
    _numSides(numSides),
	_sideLen(sideLen) {
	if (numSides < 2) {
		string err = "ERROR: Polygon cannot have less than 5 sides! Did you mean: ";
		if (numSides == 1)
			err = err + "Circle?";
		else if (numSides == 0)
			err = err + "Spacer?";
		else
			err = err + "undefined shape";
		throw err;
	}

    setBoundingBox(makeBoundingBox());
}

BoundingBox Polygon::makeBoundingBox() {
    size_type width, height;

	if (fmod(_numSides,2)==1) {
		height = (_sideLen*(1+cos(M_PI/_numSides)))/(2*sin(M_PI/_numSides));
		width = (_sideLen*sin(M_PI_2*(_numSides-1)/(2*_numSides))/(sin(M_PI/_numSides)));
	}
	else if (fmod(_numSides,4)==0) {
		height = _sideLen*(cos(M_PI/_numSides))/(sin(M_PI/_numSides));
		width = (_sideLen*cos(M_PI/_numSides))/(sin(M_PI/_numSides));
	}
	else if (fmod(_numSides,2)==0) {
		height = _sideLen*(cos(M_PI/_numSides))/(sin(M_PI/_numSides));
		width = _sideLen/(sin(M_PI/_numSides));
	}
	else throw "An error hath!: unable to determine bounding box width and height";

    std::cout << width << ", " << height << std::endl;

    return BoundingBox(height, width);
}

const Polygon::size_type Polygon::getNumOfSides() const {
	return _numSides;
}

const Polygon::size_type Polygon::getLenOfSides() const {
	return _sideLen;
}

// takes a pair to choose point on page to draw on with (0,0)
// printing at bottom left, and a string
string Polygon::postscript() {
    auto box = getBoundingBox();
	size_type xStart = 0; //(box.width - _sideLen) / 2.;
	size_type yStart = 0; //_sideLen - (box.height / 2.);

    translate(-box.width / 2.,- box.height / 2.);

    string polygonPsText = R"ps(
        gsave

        ${transform}

        newpath
        ${x} ${y} moveto
        ${poly-path}
        stroke
        grestore
    )ps";

    auto formattedPs = StringTemplate(polygonPsText)
        .replace("transform", getTransform())
        .replace("x", xStart)
        .replace("y", yStart)
        .replace("poly-path", getPolyPath())
        .get();
	return formattedPs;
}

string Polygon::getPolyPath() const {
    string polyPath = "";

	size_type angle = 360 / _numSides;
	for(size_t i = 0; i < _numSides; ++i) {
        string polyPathText = R"ps(
            ${side-length} 0 rlineto
            ${angle} rotate
        )ps";

        polyPath += StringTemplate(polyPathText)
            .replace("side-length", _sideLen)
            .replace("angle", angle)
            .get();
	}

    return polyPath;
}


//************************************** Rectangle
Rectangle::Rectangle(double width, double height)
	: Shape() {
    setBoundingBox(makeBoundingBox(width, height));
}

BoundingBox Rectangle::makeBoundingBox(double width, double height) {
    return BoundingBox(height, width);
}

std::string Rectangle::postscript() {
    auto transforms = getTransform();
    auto box = getBoundingBox();

    std::cout << box.width << std::endl;

    string rectanglePs = R"ps(
        gsave

        ${transform}

        newpath
        -${width} 2 div -${height} 2 div
         ${width} ${height} rectstroke

        grestore
    )ps";

    string formattedPs = StringTemplate(rectanglePs)
        .replace("transform", transforms)
        .replace("width", box.width)
        .replace("height", box.height)
        .get();

	return formattedPs;
}


//**************************************** Spacer
Spacer::Spacer(double width, double height): Rectangle(width, height) {}

std::string Spacer::postscript() {
    return "\n";
}


//********************************** Triangle
Triangle::Triangle(double sideLength): Polygon(3, sideLength) {}


//************************************ Composite Base
string CompositeShape::postscript() {
    auto stackedPsText = R"ps(
        gsave

        ${transform}
        ${stacked-shapes}

        grestore
    )ps";

    auto stackedShapes = getCompositeShapePS();

    auto formatted = StringTemplate(stackedPsText)
        .replace("transform", getTransform())
        .replace("stacked-shapes", stackedShapes)
        .get();

    return formatted;
}

void CompositeShape::add(ShapePtr shape) {
    shapes.push_back(shape);
}


//****************************************** Layered
string LayeredShape::getCompositeShapePS() {
    string total = "";
    BoundingBox newBox(0, 0);

    for (const auto & shape : shapes) {
        auto box = shape->getBoundingBox();

        newBox.width = max(newBox.width, box.width);
        newBox.height = max(newBox.height, box.height);

        total += shape->postscript();
    }

    setBoundingBox(newBox);

    return total;
}


//************************************* Horizontal
string HorizontalShape::getCompositeShapePS() {
    string total = "";
    auto translation = 0.;
    BoundingBox newBox(0., 0.);

    auto size = shapes.size();
    for (size_t i = 0; i < size; ++i) {
        auto shape = shapes[i];
        auto box = shape->getBoundingBox();

        newBox.width += box.width;
        newBox.height = max(newBox.height, box.height);

        total += shape->postscript();

        if (i + 1 < size) {
            auto shapesWidth = box.width / 2;
            auto nextWidth = shapes[i+1]->getBoundingBox().width / 2;

            translation += (shapesWidth + nextWidth);
            shapes[i+1]->translate(-translation, 0);
        }
    }

    setBoundingBox(newBox);

    return total;
}


//***************************************** Vertical
std::string VerticalShape::getCompositeShapePS() {
    string total = "";
    auto translation = 0.;
    BoundingBox newBox(0., 0.);

    auto size = shapes.size();
    for (size_t i = 0; i < size; ++i) {
        auto shape = shapes[i];
        auto box = shape->getBoundingBox();

        newBox.height += box.height;
        newBox.width = max(newBox.width, box.width);

        total += shape->postscript();

        if (i + 1 < size) {
            auto shapesHeight = box.height / 2;
            auto nextHeight = shapes[i+1]->getBoundingBox().height / 2;

            translation += (shapesHeight + nextHeight);
            shapes[i+1]->translate(0, -translation);
        }
    }

    setBoundingBox(newBox);

    return total;
}


//************************************************* Custom
CompositePtr CustomShape::addShapes(CompositePtr & composite) {
    for (auto i = 10; i < 40; i += 10) {
        auto c = make_shared<Polygon>(i / 10 + 2, i);
        auto r = make_shared<Circle>(i);
        composite->add(c);
        composite->add(r);
    }

    composite->translate(200, 300);
    composite->rotate(-2);

    return composite;
}


CustomShape::CustomShape(): HorizontalShape() {
        CompositePtr stacked = make_shared<LayeredShape>();
        CompositePtr vertical = make_shared<VerticalShape>();
        CompositePtr horizontal = make_shared<HorizontalShape>();

        stacked = addShapes(stacked);
        vertical = addShapes(vertical);
        horizontal = addShapes(horizontal);

        add(stacked);
        add(vertical);
        add(horizontal);
}


//******************************************** PostScript
StringTemplate::StringTemplate(std::string t) : total(t) {}

std::string StringTemplate::get() {
    return total;
}


//*************************** Debug Operators
bool operator==(const BoundingBox & lhs, const BoundingBox & rhs) {
    return rhs.height == lhs.height && rhs.width == lhs.width;
}

ostream& operator <<(ostream& os, const BoundingBox & value) {
    os << value.toString();
    return os;
}

bool operator==(const Polygon & lhs, const Polygon & rhs) {
	return lhs.getNumOfSides() == rhs.getNumOfSides() &&
		lhs.getLenOfSides() == rhs.getLenOfSides() &&
		lhs.getBoundingBox() == rhs.getBoundingBox();
}
