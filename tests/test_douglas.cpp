#include "cpp_example/geometry.hpp"
#include "cpp_example/naive_svg.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;
using namespace std::chrono;
using namespace cubao;

size_t unix_time();

vector<SVG::PointType> douglas(const vector<SVG::PointType> &points,
                               double thresh);

void update_svg(SVG &svg, const vector<SVG::PointType> &points,  //
                const SVG::Color &line_color, double line_width, //
                const SVG::Color &pt_color, double point_radius);

double rand01();

SVG::PointType next_random_point(const SVG::PointType &_p0,
                                 const SVG::PointType &_p1, double min_step,
                                 double madness);

void bbox(const vector<SVG::PointType> &points, //
          double &xmin, double &xmax, double &ymin, double &ymax);

void update_svg(SVG &svg, const vector<SVG::PointType> &points,  //
                const SVG::Color &line_color, double line_width, //
                const SVG::Color &pt_color, double point_radius)
{
    for (auto &pt : points) {
        svg.add_circle(pt, point_radius).stroke(pt_color);
    }
    svg.add_polyline(points).stroke(line_color).stroke_width(line_width);
}

vector<SVG::PointType> douglas(const vector<SVG::PointType> &points,
                               double thresh)
{
    if (points.size() <= 2) {
        return points;
    }
    Line line(points.front(), points.back());
    int max_index = 1;
    double max_dist = dist(points[max_index], line);
    for (size_t i = 2; i < points.size() - 1; i++) {
        double d = dist(points[i], line);
        if (d > max_dist) {
            max_dist = d;
            max_index = i;
        }
    }
    if (max_dist < thresh) {
        return {points.front(), points.back()};
    }
    auto lefts = douglas(
        vector<SVG::PointType>(points.begin(), points.begin() + max_index + 1),
        thresh);
    auto rights = douglas(
        vector<SVG::PointType>(points.begin() + max_index, points.end()),
        thresh);
    vector<SVG::PointType> ret;
    copy(lefts.begin(), lefts.end(), back_inserter(ret));
    copy(rights.begin() + 1, rights.end(), back_inserter(ret));
    return ret;
}

size_t unix_time()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

double rand01() { return rand() / (double)RAND_MAX; }

SVG::PointType next_random_point(const SVG::PointType &_p0,
                                 const SVG::PointType &_p1, double min_step,
                                 double madness)
{
    Point p0(_p0);
    Point p1(_p1);
    Point p01 = p1 - p0;
    double rad = rand01() * 360 / 180.0 * 3.14;
    double radius = madness * rand01() * norm(p01);
    Point delta = p01 + Point(cos(rad) * radius, sin(rad) * radius);
    double scale = max(min_step / norm(delta), 1.0);
    Point pp = p1 + delta * scale;
    return {pp.x, pp.y};
}

void bbox(const vector<SVG::PointType> &points, //
          double &xmin, double &xmax, double &ymin, double &ymax)
{
    for (auto &pt : points) {
        xmin = min(xmin, pt[0]);
        xmax = max(xmax, pt[0]);
        ymin = min(ymin, pt[1]);
        ymax = max(ymax, pt[1]);
    }
}

TEST_CASE("douglas")
{

    int n_points = 20;
    double madness = 0.5;
    double min_step = 50.0;
    double thresh = 20;
    cout << "n_points: " << n_points << endl;
    cout << "madness: " << madness << endl;
    cout << "min_step: " << min_step << endl;
    cout << "thresh: " << thresh << endl;

    SVG svg(10, 10);
    svg.grid_step(10);
    svg.grid_color(SVG::COLOR::GRAY);
    svg.background(SVG::COLOR::WHITE);

    vector<SVG::PointType> points{{0, 0}, {0, min_step}};
    for (int i = 0; i < n_points; i++) {
        points.push_back(next_random_point(points[points.size() - 2],
                                           points.back(), min_step, madness));
    }
    double xmin, xmax, ymin, ymax;
    xmin = xmax = points[0][0];
    ymin = ymax = points[0][1];
    bbox(points, xmin, xmax, ymin, ymax);

    auto points_daug = douglas(points, thresh);
    update_svg(svg, points, SVG::Color(), 1, SVG::COLOR::RED, 3);
    update_svg(svg, points_daug, SVG::Color(), 3, SVG::COLOR::GREEN, 5);

    double border_width = 20;
    xmin -= border_width;
    xmax += border_width;
    ymin -= border_width;
    ymax += border_width;

    stringstream ss;
    ss << "#points: " << points.size() << " -> " << points_daug.size();
    cout << ss.str() << endl;
    svg.add_text({xmin, ymax - 10}, ss.str(), 24).stroke(SVG::COLOR::RED);

    double width = xmax - xmin;
    double height = ymax - ymin;
    double ratio = height / width;
    if (width > height) {
        width = 1000;
        height = 1000 * ratio;
    } else {
        height = 1000;
        width = height / ratio;
    }
    svg.width(width).height(height);
    cout << "width: " << width << " height: " << height << endl;
    svg.fit_to_bbox(xmin, xmax, ymin, ymax);

    size_t epoch = unix_time();
    string path = "test_douglas_" + to_string(epoch) + ".svg";
    svg.save(path);
    cout << "wrote to '" << path << "'" << endl;
}