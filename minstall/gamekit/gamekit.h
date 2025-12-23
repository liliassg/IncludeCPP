#pragma once
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

namespace includecpp {

// ============================================================================
// Vector2D - 2D Vector Math
// ============================================================================

struct Vector2D {
    float x, y;

    Vector2D() : x(0), y(0) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(float scalar) const;
    Vector2D operator/(float scalar) const;

    float length() const;
    float lengthSquared() const;
    Vector2D normalized() const;
    float dot(const Vector2D& other) const;
    float distance(const Vector2D& other) const;
    float angle() const;
    Vector2D rotate(float radians) const;
    Vector2D lerp(const Vector2D& target, float t) const;
};

// ============================================================================
// Rect - Axis-Aligned Bounding Box
// ============================================================================

struct Rect {
    float x, y, width, height;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}

    float left() const { return x; }
    float right() const { return x + width; }
    float top() const { return y; }
    float bottom() const { return y + height; }
    Vector2D center() const { return Vector2D(x + width/2, y + height/2); }

    bool contains(const Vector2D& point) const;
    bool intersects(const Rect& other) const;
    Rect intersection(const Rect& other) const;
    Rect union_with(const Rect& other) const;
    void move(float dx, float dy);
    void moveto(float x, float y);
};

// ============================================================================
// Color - RGBA Color
// ============================================================================

struct Color {
    uint8_t r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    static Color fromHex(uint32_t hex);
    uint32_t toHex() const;
    Color lerp(const Color& target, float t) const;

    static Color Black()   { return Color(0, 0, 0); }
    static Color White()   { return Color(255, 255, 255); }
    static Color Red()     { return Color(255, 0, 0); }
    static Color Green()   { return Color(0, 255, 0); }
    static Color Blue()    { return Color(0, 0, 255); }
    static Color Yellow()  { return Color(255, 255, 0); }
    static Color Cyan()    { return Color(0, 255, 255); }
    static Color Magenta() { return Color(255, 0, 255); }
};

// ============================================================================
// Circle - Circle Collision Shape
// ============================================================================

struct Circle {
    float x, y, radius;

    Circle() : x(0), y(0), radius(0) {}
    Circle(float x, float y, float radius) : x(x), y(y), radius(radius) {}

    Vector2D center() const { return Vector2D(x, y); }
    bool contains(const Vector2D& point) const;
    bool intersects(const Circle& other) const;
    bool intersects(const Rect& rect) const;
    void move(float dx, float dy);
};

// ============================================================================
// Collision - Collision Detection Utilities
// ============================================================================

class Collision {
public:
    static bool pointInRect(const Vector2D& point, const Rect& rect);
    static bool pointInCircle(const Vector2D& point, const Circle& circle);
    static bool rectRect(const Rect& a, const Rect& b);
    static bool circleCircle(const Circle& a, const Circle& b);
    static bool rectCircle(const Rect& rect, const Circle& circle);
    static Vector2D closestPoint(const Rect& rect, const Vector2D& point);
};

// ============================================================================
// Timer - Game Timer & Delta Time
// ============================================================================

class Timer {
private:
    double start_time;
    double pause_time;
    bool paused;
    double time_scale;

public:
    Timer();

    void reset();
    void pause();
    void resume();
    double elapsed() const;
    double delta(double last_time) const;
    void setTimeScale(float scale);
    float getTimeScale() const;
    bool isPaused() const;

    static double now();
};

// ============================================================================
// Animation - Sprite Animation Controller
// ============================================================================

class Animation {
private:
    std::vector<int> frames;
    double frame_duration;
    bool looping;
    int current_frame;
    double elapsed_time;
    bool finished;

public:
    Animation();
    Animation(const std::vector<int>& frames, double frame_duration, bool loop = true);

    void update(double delta);
    int getCurrentFrame() const;
    void reset();
    void setLooping(bool loop);
    bool isFinished() const;
    void setFrameDuration(double duration);
};

// ============================================================================
// Input - Input State Manager
// ============================================================================

class Input {
private:
    static std::map<int, bool> keys_down;
    static std::map<int, bool> keys_pressed;
    static std::map<int, bool> keys_released;
    static std::map<int, bool> mouse_buttons;
    static Vector2D mouse_pos;

public:
    static void setKeyDown(int key, bool down);
    static void setMouseButton(int button, bool down);
    static void setMousePos(float x, float y);
    static void update();

    static bool isKeyDown(int key);
    static bool isKeyPressed(int key);
    static bool isKeyReleased(int key);
    static bool isMouseButtonDown(int button);
    static Vector2D getMousePos();
};

// ============================================================================
// Transform - 2D Transform (Position, Rotation, Scale)
// ============================================================================

struct Transform {
    Vector2D position;
    float rotation;
    Vector2D scale;

    Transform();
    Transform(const Vector2D& pos, float rot = 0.0f, const Vector2D& scale = Vector2D(1, 1));

    void translate(const Vector2D& offset);
    void rotate(float radians);
    void scaleBy(float factor);
    Vector2D transformPoint(const Vector2D& point) const;
};

// ============================================================================
// Sprite - Sprite with Transform and Animation
// ============================================================================

class Sprite {
public:
    Transform transform;
    Rect source_rect;
    Color tint;
    bool visible;
    int texture_id;

    Sprite();
    Sprite(int texture_id, const Rect& source);

    void setPosition(const Vector2D& pos);
    void setRotation(float rot);
    void setScale(const Vector2D& scale);
    void move(const Vector2D& offset);
    Rect getBounds() const;
};

// ============================================================================
// Camera2D - 2D Camera with Zoom and Follow
// ============================================================================

class Camera2D {
private:
    Vector2D position;
    Vector2D viewport_size;
    float zoom;
    Vector2D target_position;
    float follow_speed;

public:
    Camera2D();
    Camera2D(float width, float height);

    void setPosition(const Vector2D& pos);
    void setZoom(float z);
    void setViewportSize(float width, float height);
    void follow(const Vector2D& target, float speed = 1.0f);
    void update(double delta);

    Vector2D getPosition() const;
    float getZoom() const;
    Vector2D worldToScreen(const Vector2D& world_pos) const;
    Vector2D screenToWorld(const Vector2D& screen_pos) const;
    Rect getViewBounds() const;
};

// ============================================================================
// Grid - 2D Grid Utilities (Tilemap helpers)
// ============================================================================

class Grid {
public:
    static Vector2D worldToGrid(const Vector2D& world_pos, int tile_size);
    static Vector2D gridToWorld(int grid_x, int grid_y, int tile_size);
    static int getIndex(int x, int y, int width);
    static Vector2D getCoords(int index, int width);
    static std::vector<Vector2D> getNeighbors(int x, int y, int width, int height, bool diagonals = false);
};

} // namespace includecpp
