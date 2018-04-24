#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <fstream>
using std::ofstream;
#include <vector>
using std::vector;
#include <memory>
using std::make_shared;

#include "catch.hpp"
#include "cps_crtp.hpp"
#include "cps_crtp.hpp"
#include "cps_crtp.hpp"

// ********************************* Basic Shape
TEST_CASE("base shape") {
    auto basicCircle = Circle(1);
    REQUIRE_NOTHROW(basicCircle.getBoundingBox() == basicCircle.getBoundingBox());
}

// ********************************* Circle
TEST_CASE("basic circle") {
    auto basicCircle = Circle(1);

    SECTION("test boundingBox is correct size") {
        auto circleBox = basicCircle.getBoundingBox();
        auto correctBox = BoundingBox(2, 2);

        REQUIRE(circleBox == correctBox);
    }

    SECTION("basic post script output") {
        std::string output = "";
        REQUIRE_NOTHROW(output = basicCircle.postscript());
		ofstream of("ps-example/test-circle.ps");
		if (of.is_open()) {
			of << output << std::endl;
			of.close();
		}
    }
}


// ********************************* Polygons
bool resetOKFlag(bool & ok) {
	return ok = true;
}

TEST_CASE("Polygons: basic polygons") {
	// These shapes should be fine since number of sides is > 5 or default
	auto pentagon = Polygon(5,1);
	auto hexagon = Polygon(6,1);
	auto septagon = Polygon(7,1);
	auto octogon = Polygon(8,1);
	auto nonagon = Polygon(9,1);

	SECTION("test shapes have proper amount of sides") {
		REQUIRE(pentagon.getNumOfSides() == 5);
		REQUIRE(hexagon.getNumOfSides() == 6);
		REQUIRE(septagon.getNumOfSides() == 7);
		REQUIRE(octogon.getNumOfSides() == 8);
		REQUIRE(nonagon.getNumOfSides() == 9);
	}

	SECTION("illegal polygons") {
        REQUIRE_THROWS(Polygon(1, 1));
	}
}

int polyHeight(const int & n, const int & e) {
	if (n%2==1)
		return (e*(1+cos(M_PI/n)))/(2*sin(M_PI/n));
	else if (n%4==0)
		return e*(cos(M_PI/n))/(sin(M_PI/n));
	else if (n%2==0 && n%4!=0)
		return e*(cos(M_PI/n))/(sin(M_PI/n));
}

int polyWidth(const int & n, const int & e) {
	if (n%2==1)
		return (e*sin(M_PI*(n-1)/(2*n))/(sin(M_PI/n)));
	else if (n%4==0)
		return (e*cos(M_PI/n))/(sin(M_PI/n));
	else if (n%2==0 && n%4!=0)
		return e/(sin(M_PI/n));
}

TEST_CASE("Polygons: bounding box correctness") {
	//auto pentagon = Polygon(5,1);
	//auto hexagon = Polygon(6,1);
	//auto septagon = Polygon(7,1);
	//auto octogon = Polygon(8,1);
	//auto nonagon = Polygon(9,1);

	//auto testPentaBox = BoundingBox(polyHeight(5,1), polyWidth(5,1));
	//auto testHexaBox = BoundingBox(polyHeight(6,1), polyWidth(6,1));
	//auto testSeptaBox = BoundingBox(polyHeight(7,1), polyWidth(7,1));
	//auto testOctoBox = BoundingBox(polyHeight(8,1), polyWidth(8,1));
	//auto testNonaBox = BoundingBox(polyHeight(9,1), polyWidth(9,1));

	//SECTION("test shapes have proper bounding boxes") {
        //REQUIRE(pentagon.getBoundingBox() == testPentaBox);
        //REQUIRE(hexagon.getBoundingBox() == testHexaBox);
        //REQUIRE(septagon.getBoundingBox() == testSeptaBox);
        //REQUIRE(octogon.getBoundingBox() == testOctoBox);
        //REQUIRE(nonagon.getBoundingBox() == testNonaBox);
	//}
}

TEST_CASE("Polygons: copiable") {
	auto pentagon = Polygon(5,1);
	auto hexagon = Polygon(6,1);
	auto septagon = Polygon(7,1);
	auto octogon = Polygon(8,1);
	auto nonagon = Polygon(9,1);

	auto pentaCopy = pentagon;
	auto hexaCopy = hexagon;
	auto septaCopy(septagon);
	auto octoCopy(octogon);
	auto nonaCopy = (nonagon);

	SECTION("test shapes are proper copies of shapes") {
		REQUIRE(pentagon == pentaCopy);
		REQUIRE(hexagon == hexaCopy);
		REQUIRE(septagon == septaCopy);
		REQUIRE(octogon == octoCopy);
		REQUIRE(nonagon == nonaCopy);
	}
}

TEST_CASE("Polygons: Draw to PostScript") {
	auto octogon = Polygon(8,20);
    octogon.translate(200, 200);

	string output = "";
	SECTION("Can write output") {
		REQUIRE_NOTHROW(output = octogon.postscript());
        ofstream of("ps-example/test-polygon.ps");
		if (of.is_open()) {
			of << output << std::endl;
			of.close();
		}
	}
}

// ********************************* Rectangles
TEST_CASE("rectangle") {
    vector<Rectangle> rects = {
        {1, 1}, {1, 2}, {1, 3},
        {2, 1}, {2, 2}, {2, 3},
        {3, 1}, {3, 2}, {3, 3}
    };

    vector<BoundingBox> boxes = {
        {1, 1}, {1, 2}, {1, 3},
        {2, 1}, {2, 2}, {2, 3},
        {3, 1}, {3, 2}, {3, 3}
    };

    SECTION("test that the bounding box is correct") {
        for (size_t i = 0; i < boxes.size(); ++i) {
            //REQUIRE(rects[i].getBoundingBox() == boxes[i]);
        }
    }
}

TEST_CASE("Rectangles: Draw to PostScript") {
	auto rectangle = Rectangle(50,70);
    rectangle.translate(200, 200);

    string correct = R"ps(
        gsave

        200 200 translate


        newpath
        -50 2 div -70 2 div
         50 70 rectstroke

        grestore
    )ps";

	SECTION("Can write output") {
        string output;

		REQUIRE_NOTHROW(output = rectangle.postscript());
        REQUIRE(output == correct);

		ofstream of("ps-example/test-rectangle.ps");
		if (of.is_open()) {
			of << output << std::endl;
			of.close();
		}
	}
}

// ********************************* Triangles
TEST_CASE("Triangles")	{
	auto triangle1 = Triangle(1);

	SECTION("Bounding box test for triangle") {
	}
}

TEST_CASE("Triangles: Draw to PostScript") {
	auto triangle = Triangle(5);
	string output = ""; // could do "/inch {72 mul} def\n\n\n\n" but we're not using doubles...
	SECTION("Can write output") {
		REQUIRE_NOTHROW(output = triangle.postscript());
		ofstream of("../ps-example/test-triangle.ps");
		if (of.is_open()) {
			of << "Is this printing" << std::endl;
			of.close();
		}
	}
}

// ********************************* Transforms
TEST_CASE("transformations") {
    auto circle = Circle(1);
    auto correctScale = "1 1 scale\n";
    auto correctTranslate = "1 1 translate\n";

    SECTION("test scale") {
        circle.scale(1., 1.);

        REQUIRE(circle.getTransform() == correctScale);
    }

    SECTION("test translate") {
        circle.translate(1., 1.);

        REQUIRE(circle.getTransform() == correctTranslate);
    }

    SECTION("test rotation") {
        for (int i = -4; i < 5; ++i) {
            auto c = Circle(1);
            c.rotate(i);
            auto correct = std::to_string(90 * i) + " rotate\n";

            REQUIRE(c.getTransform() == correct);
        }
    }

    SECTION("transformation adds to overall postscript output") {
        auto before = circle.postscript();

        circle.translate(200, 200);
        circle.rotate(2);
        circle.scale(10, 10);

        auto after = circle.postscript();

        REQUIRE(before.length() < after.length());

        ofstream of("ps-example/test-transforms.ps");
		if (of.is_open()) {
			of << after << std::endl;
			of.close();
		}
    }
}

// ********************************* Composite shapes

void toFile(const std::string & fname, const std::string & output) {
        ofstream of(fname);
        if (of.is_open()) {
            of << output << std::endl;
            of.close();
        }
}

CompositePtr addShapes(CompositePtr & composite) {
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

TEST_CASE("test composites") {
    auto c = make_shared<Circle>(5);
    auto r = make_shared<Rectangle>(20, 20);

    SECTION("layered") {
        CompositePtr ss = make_shared<LayeredShape>();
        SECTION("bounding box test") {
            ss->add(c);
            ss->add(r);
            ss->postscript();

            auto correct = BoundingBox(20, 20);

            REQUIRE(correct == ss->getBoundingBox());
        }

        SECTION("visual test") {
            ss = addShapes(ss);

            std::string out = "";
            REQUIRE_NOTHROW(out = ss->postscript());

            toFile("ps-example/test-stacked.ps", out);
        }
    }

    SECTION("horizontal") {
        CompositePtr hs = make_shared<HorizontalShape>();

        SECTION("bounding box test") {
            hs->add(c);
            hs->add(r);
            hs->postscript();

            auto correct = BoundingBox(20, 30);

            REQUIRE(correct == hs->getBoundingBox());
        }

        SECTION("visual test") {
            hs = addShapes(hs);
            std::string out = "";

            REQUIRE_NOTHROW(out = hs->postscript());

            toFile("ps-example/test-horizontal.ps", out);
        }
    }

    SECTION("Vertical") {
        CompositePtr ss = make_shared<VerticalShape>();
        SECTION("bounding box test") {
            ss->add(c);
            ss->add(r);
            ss->postscript();

            auto correct = BoundingBox(30, 20);

            REQUIRE(correct == ss->getBoundingBox());
        }

        SECTION("visual test") {
            ss = addShapes(ss);

            std::string out = "";
            REQUIRE_NOTHROW(out = ss->postscript());

            toFile("ps-example/test-vertical.ps", out);
        }
    }

    SECTION("custom composite composite") {
        CompositePtr combined = make_shared<CustomShape>();

        std::string out = "";
        REQUIRE_NOTHROW(out = combined->postscript());

        toFile("ps-example/test-combined.ps", out);
    }

}
