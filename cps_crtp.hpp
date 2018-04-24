#ifndef CPS_CRTP_HPP_INCLUDED
#define CPS_CRTP_HPP_INCLUDED

#include <memory>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>


// ********************* Member Structs
struct Point {
    double x, y;

    Point(): x(0.0), y(0.0) {}
    Point(double x, double y) : x(x), y(y) {}
};

struct BoundingBox {
    double height, width;

    BoundingBox(): height(0.0), width(0.0)  {}
    BoundingBox(double h, double w): height(h), width(w) {}

    std::string toString() const;

    void scale(double x, double y) {
        width *= x;
        height *= y;
    }
};

// ************************ Base Shape Class
class Shape {
    private:
        Point _center;
        BoundingBox _boundingBox;

        std::string transformation;

    public:
        Shape()=default;
        virtual ~Shape() = default;

        virtual std::string postscript() = 0;

        std::string getTransform() const;

        BoundingBox getBoundingBox() const;
        void setBoundingBox(BoundingBox b);

        void scale(double x, double y);
        void translate(double x, double y);
        void rotate(int ticks);
};
using ShapePtr = std::shared_ptr<Shape>;
using ShapePtrs = std::vector<ShapePtr>;


// ************************ Circle Class
class Circle : public Shape {
    private:
        double _radius;

    public:
        Circle(double radius);
        ~Circle() override = default;

        std::string postscript() override;
        const BoundingBox makeBoundingBox() const;
};


// ************************ Polygon Class
class Polygon : public Shape {
	private:
		using size_type = double;
		const size_type _numSides, _sideLen;
		BoundingBox _boundBox;

	public:
		Polygon() = delete;
		Polygon(const size_type & numSides, const size_type & sideLen);
		~Polygon() override = default;

		const size_type getNumOfSides() const;
		const size_type getLenOfSides() const;

        BoundingBox makeBoundingBox();

		std::string postscript() override;
        std::string getPolyPath() const;
};


// ************************ Rectangle Class
class Rectangle : public Shape {
	BoundingBox _boundingBox;

    public:
        Rectangle(double width, double height);
        ~Rectangle() override = default;

		virtual std::string postscript() override;

        BoundingBox makeBoundingBox(double width, double height);
};


// ************************ Spacer Class
class Spacer : public Rectangle {
    public:
        Spacer(double width, double height);
        ~Spacer() override = default;

		std::string postscript() override;
};


// ************************ Triangle Class
class Triangle: public Polygon	{
public:
	Triangle(double sideLength);
	~Triangle() = default;
};


// ************************ Composite Shape Base Class
class CompositeShape : public Shape {
    protected:
        ShapePtrs shapes;

    public:
        CompositeShape() = default;
        std::string postscript() override;

        void add(ShapePtr shape);

    private:
        virtual std::string getCompositeShapePS() = 0;
};


// ************************ Layered Shape Class
class LayeredShape : public CompositeShape {
    public:
        std::string getCompositeShapePS() override;
};


// ************************ Horizontal Shape Class
class HorizontalShape : public CompositeShape {
    public:
        std::string getCompositeShapePS() override;
};


// ************************ Vertical Shape Class
class VerticalShape : public CompositeShape {
    public:
        std::string getCompositeShapePS() override;
};


// ************************ Custom Shape Class
using CompositePtr = std::shared_ptr<CompositeShape>;
class CustomShape : public HorizontalShape {
    public:
        CustomShape();
    private:

    CompositePtr addShapes(CompositePtr & composite);
};


// ************************ PostScript Functionality
class StringTemplate {
    private:
        std::string total;
    public:
        StringTemplate(std::string t);

        std::string get();

        template <typename ToStringType>
        StringTemplate& replace(const std::string & token, const ToStringType & value);
};

template <typename ToStringType>
StringTemplate& StringTemplate::replace(const std::string & token, const ToStringType & value) {
    auto formattedToken = "${" + token + "}";

    std::ostringstream os;
    os << value;
    std::string valueString = os.str();

    std::size_t pos = total.find(formattedToken);

    while (pos != std::string::npos) {
        total = total.replace(pos, formattedToken.length(), valueString);
        pos = total.find(formattedToken);
    }

    return *this;
}


//************************* Debug Operators
bool operator==(const BoundingBox & lhs, const BoundingBox & rhs);
std::ostream& operator <<(std::ostream& os, const BoundingBox & value);
bool operator==(const Polygon & lhs, const Polygon & rhs);

#endif //CPS_CRTP_HPP_INCLUDED