#ifndef CUBAO_NAIVE_SVG_HPP
#define CUBAO_NAIVE_SVG_HPP

// upstream
// https://github.com/cubao/headers/tree/main/include/cubao/naive_svg.hpp
// migrated from https://github.com/cubao/naive-svg/blob/master/svg.hpp

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

namespace cubao
{
// https://en.wikipedia.org/wiki/Fluent_interface
// https://rapidjson.org/md_doc_tutorial.html
// https://www.tutorialspoint.com/what-is-function-chaining-in-javascript
#ifndef SETUP_FLUENT_API
#define SETUP_FLUENT_API(Klass, VarType, VarName)                              \
    Klass &VarName(const VarType &v)                                           \
    {                                                                          \
        VarName##_ = v;                                                        \
        return *this;                                                          \
    }                                                                          \
    VarType &VarName() { return VarName##_; }                                  \
    const VarType &VarName() const { return VarName##_; }
#endif

#ifndef SETUP_FLUENT_API_FOR_SVG_ELEMENT
#define SETUP_FLUENT_API_FOR_SVG_ELEMENT(KlassType)                            \
    SETUP_FLUENT_API(KlassType, Color, stroke)                                 \
    SETUP_FLUENT_API(KlassType, Color, fill)                                   \
    SETUP_FLUENT_API(KlassType, double, stroke_width)
#endif

struct SVG
{
    using PointType = std::array<double, 2>;
    enum COLOR
    {
        RED = 0xFF0000,
        GREEN = 0x00FF00, // not css green
        BLUE = 0x0000FF,
        YELLOW = 0xFFFF00,
        WHITE = 0xFFFFFF,
        GRAY = 0x9B9B9B,
        BLACK = 0x000000,
        NONE = -1,
    };

    struct Color
    {
        Color(int rgb = -1)
        {
            if (rgb > 0) {
                r_ = (rgb >> 16) & 0xFF;
                g_ = (rgb >> 8) & 0xFF;
                b_ = rgb & 0xFF;
            }
        }
        Color(int r, int g, int b, float a = -1.f) : r_(r), g_(g), b_(b), a_(a)
        {
        }
        SETUP_FLUENT_API(Color, int, r)
        SETUP_FLUENT_API(Color, int, g)
        SETUP_FLUENT_API(Color, int, b)
        SETUP_FLUENT_API(Color, float, a)
        bool invalid() const
        {
            return r_ < 0 || g_ < 0 || b_ < 0 || r_ > 255 || g_ > 255 ||
                   b_ > 255;
        }
        friend std::ostream &operator<<(std::ostream &out, const Color &c);

      private:
        int r_{-1}, g_{-1}, b_{-1};
        float a_{-1.f};
    };

    enum ELEMENT
    {
        POLYLINE,
        POLYGON,
        CIRCLE,
        TEXT
    };

    struct Element
    {
      protected:
        std::vector<PointType> points_;
        Color stroke_{COLOR::BLACK};
        double stroke_width_{1.0};
        Color fill_{COLOR::NONE};
    };

    struct Polyline : Element
    {
        Polyline(const std::vector<PointType> &points) { points_ = points; }
        SETUP_FLUENT_API(Polyline, std::vector<SVG::PointType>, points)
        SETUP_FLUENT_API_FOR_SVG_ELEMENT(Polyline)
        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Polyline &p);
    };

    struct Polygon : Element
    {
        Polygon(const std::vector<PointType> &points) { points_ = points; }
        SETUP_FLUENT_API(Polygon, std::vector<SVG::PointType>, points)
        SETUP_FLUENT_API_FOR_SVG_ELEMENT(Polygon)
        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Polygon &p);
    };

    struct Circle : Element
    {
        Circle(const PointType &p, double r = 1.0)
        {
            points_ = {p};
            r_ = r;
        }
        Circle &center(const PointType &p)
        {
            points_[0] = p;
            return *this;
        }
        PointType &center() { return points_[0]; }
        const PointType &center() const { return points_[0]; }
        Circle &x(double x)
        {
            points_[0][0] = x;
            return *this;
        }
        double &x() { return points_[0][0]; }
        const double &x() const { return points_[0][0]; }
        Circle &y(double y)
        {
            points_[0][0] = y;
            return *this;
        }
        double &y() { return points_[0][0]; }
        const double &y() const { return points_[0][0]; }

        SETUP_FLUENT_API(Circle, double, r)
        SETUP_FLUENT_API_FOR_SVG_ELEMENT(Circle)
        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Circle &c);

      protected:
        double r_{1.0};
    };

    struct Text : Element
    {
        Text(const PointType &p, const std::string &text, int fontsize = 10.0)
        {
            points_ = {p};
            text_ = text;
            fontsize_ = fontsize;
        }
        Text &position(const PointType &p)
        {
            points_[0] = p;
            return *this;
        }
        PointType &position() { return points_[0]; }
        const PointType &position() const { return points_[0]; }

        Text &x(double x)
        {
            points_[0][0] = x;
            return *this;
        }
        double &x() { return points_[0][0]; }
        const double &x() const { return points_[0][0]; }
        Text &y(double y)
        {
            points_[0][0] = y;
            return *this;
        }
        double &y() { return points_[0][0]; }
        const double &y() const { return points_[0][0]; }

        SETUP_FLUENT_API(Text, std::string, text)
        SETUP_FLUENT_API(Text, std::vector<std::string>, lines)
        SETUP_FLUENT_API(Text, double, fontsize)
        SETUP_FLUENT_API_FOR_SVG_ELEMENT(Text)

        friend std::ostream &operator<<(std::ostream &out, const SVG::Text &t);

      protected:
        std::string text_;
        std::vector<std::string> lines_;
        double fontsize_{10.0};
    };

    SVG(double width, double height) : width_(width), height_(height) {}
    SETUP_FLUENT_API(SVG, double, width)
    SETUP_FLUENT_API(SVG, double, height)
    SETUP_FLUENT_API(SVG, double, grid_step)
    SETUP_FLUENT_API(SVG, std::vector<double>, grid_x)
    SETUP_FLUENT_API(SVG, std::vector<double>, grid_y)
    SETUP_FLUENT_API(SVG, Color, grid_color)
    SETUP_FLUENT_API(SVG, Color, background)

    void save(std::string path) const
    {
        std::ofstream file(path);
        file << *this;
        file.close();
    }

    void fit_to_bbox(double xmin, double xmax, double ymin, double ymax)
    {
        // for (auto &p : polygons) {
        //     interp(p.points, xmin, xmax, ymin, ymax, width, height);
        // }
        // for (auto &p : polylines) {
        //     interp(p.points, xmin, xmax, ymin, ymax, width, height);
        // }
        // for (auto &c : circles) {
        //     interp(c.points, xmin, xmax, ymin, ymax, width, height);
        // }
        // for (auto &t : texts) {
        //     interp(t.points, xmin, xmax, ymin, ymax, width, height);
        // }
    }

    friend std::ostream &operator<<(std::ostream &out, const SVG &s);

  private:
    // size
    double width_, height_;
    // grid
    double grid_step_{-1.0};
    std::vector<double> grid_x_, grid_y_; // low, high, step
    Color grid_color_{COLOR::GRAY};
    // background
    Color background_{COLOR::NONE};
    // elements
    std::vector<std::pair<ELEMENT, void *>> elements_;
};

inline std::ostream &operator<<(std::ostream &out, const SVG::Color &c)
{
    if (c.invalid()) {
        out << "none";
        return out;
    }
    if (0.0 <= c.a() && c.a() <= 1.0) {
        out << "rgba(" << c.r() << "," << c.g() << "," << c.b() << "," << c.a()
            << ")";
    } else {
        out << "rgb(" << c.r() << "," << c.g() << "," << c.b() << ")";
    }
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SVG::Polyline &p)
{
    out << "<polyline";
    out << " style='stroke:" << p.stroke()      //
        << ";stroke-width:" << p.stroke_width() //
        << ";fill:" << p.fill()                 //
        << "'";
    out << " points='";
    for (auto &pt : p.points()) {
        out << pt[0] << "," << pt[1] << " ";
    }
    out << "'";
    out << " />";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SVG::Polygon &p)
{
    out << "<polygon";
    out << " style='stroke:" << p.stroke()      //
        << ";stroke-width:" << p.stroke_width() //
        << ";fill:" << p.fill()                 //
        << "'";
    out << " points='";
    for (auto &pt : p.points()) {
        out << pt[0] << "," << pt[1] << " ";
    }
    out << "'";
    out << " />";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SVG::Circle &c)
{
    out << "<circle r='" << c.r() << "'"                //
        << " cx='" << c.x() << "' cy='" << c.y() << "'" //
        << " style='stroke:" << c.stroke()              //
        << ";stroke-width:" << c.stroke_width()         //
        << ";fill:" << c.fill() << "'"                  //
        << " />";
    return out;
}

inline std::string html_escape(const std::string &text)
{
    const static std::vector<std::string> escapes = {"&amp;", "&quot;",
                                                     "&apos;", "&lt;", "&gt;"};
    std::map<int, int> replace;
    for (size_t pos = 0; pos != text.size(); ++pos) {
        const char c = text[pos];
        if (c == '&') {
            replace[pos] = 0;
        } else if (c == '\"') {
            replace[pos] = 1;
        } else if (c == '\'') {
            replace[pos] = 2;
        } else if (c == '<') {
            replace[pos] = 3;
        } else if (c == '>') {
            replace[pos] = 4;
        }
    }
    if (replace.empty()) {
        return text;
    }
    std::string buffer;
    buffer.reserve(text.size() + 6 * replace.size());
    // TODO
    for (size_t pos = 0; pos != text.size(); ++pos) {
        auto itr = replace.find(text[pos]);
        if (itr == replace.end()) {
            buffer.append(&text[pos], 1);
        } else {
            buffer.append(escapes[itr->second]);
        }
    }
    return buffer;
}

inline std::ostream &operator<<(std::ostream &out, const SVG::Text &t)
{
    out << "<text"                                    //
        << " x='" << t.x() << "' y='" << t.y() << "'" //
        << " fill='" << t.fill() << "'"               //
        << " font-size='" << t.fontsize() << "'"      //
        << " font-family='monospace'"                 //
        << ">" << t.text();
    if (!t.lines().empty()) {
        double fontsize = t.fontsize() / 5.0;
        for (auto &line : t.lines()) {
            out << "<tspan xml:space='preserve' x='" << t.x() //
                << "' dy='" << fontsize                       //
                << "' fill='" << t.fill()                     //
                << "' font-size='" << fontsize                //
                << "' font-family='monospace'>"               //
                << html_escape(line) << "</tspan>";
        }
    }
    out << "</text>";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SVG &s)
{
    out << "<svg width='" << s.width() << "' height='" << s.height() << "'"
        << " xmlns='http://www.w3.org/2000/svg' "
           "xmlns:xlink='http://www.w3.org/1999/xlink'>";
    if (!s.background().invalid()) {
        out << "\n\t<rect width='100%' height='100%' fill='" //
            << s.background()                                //
            << "'/>";
    }
    double xmin = 0, xmax = s.width(), xstep = s.grid_step();
    double ymin = 0, ymax = s.height(), xstep = s.grid_step();
    if (s.grid_x_.size() == 3 && s.grid_y_.size() == 3) {
    }
    // if (s.grid_step > 0) {
    //     SVG::Color grid_color = SVG::COLOR::GRAY;
    //     if (!s.grid_color.invalid()) {
    //         grid_color = s.grid_color;
    //     }
    //     for (double i = 0; i < s.height; i += s.grid_step) {
    //         out << "\n\t"
    //             << SVG::Polyline({{0, i}, {s.width, i}}).stroke(grid_color);
    //     }
    //     for (double j = 0; j < s.width; j += s.grid_step) {
    //         out << "\n\t"
    //             << SVG::Polyline({{j, 0}, {j, s.height}}).stroke(grid_color);
    //     }
    // }
    // for (auto &p : s.polygons) {
    //     out << "\n\t" << p;
    // }
    // for (auto &p : s.polylines) {
    //     out << "\n\t" << p;
    // }
    // for (auto &c : s.circles) {
    //     out << "\n\t" << c;
    // }
    // for (auto &t : s.texts) {
    //     out << "\n\t" << t;
    // }
    out << "\n</svg>";
    return out;
}

inline void SVG::save(std::string path) const {}

inline void interp(std::vector<std::vector<double>> &points,           //
                   double xmin, double xmax, double ymin, double ymax, //
                   double width, double height)
{
    double xspan = xmax - xmin;
    double yspan = ymax - ymin;
    for (auto &pt : points) {
        pt[0] = (pt[0] - xmin) / xspan * width;
        pt[1] = (pt[1] - ymin) / yspan * height;
    }
}

inline void SVG::fit_to_bbox(double xmin, double xmax, double ymin, double ymax)
{
}
} // namespace cubao

#endif
