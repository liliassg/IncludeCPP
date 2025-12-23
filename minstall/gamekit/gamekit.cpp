#include "gamekit.h"
#include <chrono>
#include <cmath>

namespace includecpp {

// Static member initialization
std::map<int, bool> Input::keys_down;
std::map<int, bool> Input::keys_pressed;
std::map<int, bool> Input::keys_released;
std::map<int, bool> Input::mouse_buttons;
Vector2D Input::mouse_pos;

// ============================================================================
// Vector2D Implementation
// ============================================================================

Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(float scalar) const {
    return Vector2D(x / scalar, y / scalar);
}

float Vector2D::length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2D::lengthSquared() const {
    return x * x + y * y;
}

Vector2D Vector2D::normalized() const {
    float len = length();
    return (len > 0) ? (*this / len) : Vector2D(0, 0);
}

float Vector2D::dot(const Vector2D& other) const {
    return x * other.x + y * other.y;
}

float Vector2D::distance(const Vector2D& other) const {
    return (*this - other).length();
}

float Vector2D::angle() const {
    return std::atan2(y, x);
}

Vector2D Vector2D::rotate(float radians) const {
    float cos_a = std::cos(radians);
    float sin_a = std::sin(radians);
    return Vector2D(
        x * cos_a - y * sin_a,
        x * sin_a + y * cos_a
    );
}

Vector2D Vector2D::lerp(const Vector2D& target, float t) const {
    return Vector2D(
        x + (target.x - x) * t,
        y + (target.y - y) * t
    );
}

// ============================================================================
// Rect Implementation
// ============================================================================

bool Rect::contains(const Vector2D& point) const {
    return point.x >= x && point.x <= x + width &&
           point.y >= y && point.y <= y + height;
}

bool Rect::intersects(const Rect& other) const {
    return !(right() < other.left() || left() > other.right() ||
             bottom() < other.top() || top() > other.bottom());
}

Rect Rect::intersection(const Rect& other) const {
    float left = std::max(this->left(), other.left());
    float right = std::min(this->right(), other.right());
    float top = std::max(this->top(), other.top());
    float bottom = std::min(this->bottom(), other.bottom());

    if (right < left || bottom < top) {
        return Rect(0, 0, 0, 0);
    }

    return Rect(left, top, right - left, bottom - top);
}

Rect Rect::union_with(const Rect& other) const {
    float left = std::min(this->left(), other.left());
    float right = std::max(this->right(), other.right());
    float top = std::min(this->top(), other.top());
    float bottom = std::max(this->bottom(), other.bottom());

    return Rect(left, top, right - left, bottom - top);
}

void Rect::move(float dx, float dy) {
    x += dx;
    y += dy;
}

void Rect::moveto(float new_x, float new_y) {
    x = new_x;
    y = new_y;
}

// ============================================================================
// Color Implementation
// ============================================================================

Color Color::fromHex(uint32_t hex) {
    return Color(
        (hex >> 16) & 0xFF,
        (hex >> 8) & 0xFF,
        hex & 0xFF,
        (hex >> 24) & 0xFF
    );
}

uint32_t Color::toHex() const {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

Color Color::lerp(const Color& target, float t) const {
    return Color(
        static_cast<uint8_t>(r + (target.r - r) * t),
        static_cast<uint8_t>(g + (target.g - g) * t),
        static_cast<uint8_t>(b + (target.b - b) * t),
        static_cast<uint8_t>(a + (target.a - a) * t)
    );
}

// ============================================================================
// Circle Implementation
// ============================================================================

bool Circle::contains(const Vector2D& point) const {
    float dx = point.x - x;
    float dy = point.y - y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

bool Circle::intersects(const Circle& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float distance_sq = dx * dx + dy * dy;
    float radius_sum = radius + other.radius;
    return distance_sq <= (radius_sum * radius_sum);
}

bool Circle::intersects(const Rect& rect) const {
    Vector2D closest = Collision::closestPoint(rect, Vector2D(x, y));
    return contains(closest);
}

void Circle::move(float dx, float dy) {
    x += dx;
    y += dy;
}

// ============================================================================
// Collision Implementation
// ============================================================================

bool Collision::pointInRect(const Vector2D& point, const Rect& rect) {
    return rect.contains(point);
}

bool Collision::pointInCircle(const Vector2D& point, const Circle& circle) {
    return circle.contains(point);
}

bool Collision::rectRect(const Rect& a, const Rect& b) {
    return a.intersects(b);
}

bool Collision::circleCircle(const Circle& a, const Circle& b) {
    return a.intersects(b);
}

bool Collision::rectCircle(const Rect& rect, const Circle& circle) {
    return circle.intersects(rect);
}

Vector2D Collision::closestPoint(const Rect& rect, const Vector2D& point) {
    return Vector2D(
        std::max(rect.left(), std::min(point.x, rect.right())),
        std::max(rect.top(), std::min(point.y, rect.bottom()))
    );
}

// ============================================================================
// Timer Implementation
// ============================================================================

Timer::Timer() : start_time(now()), pause_time(0), paused(false), time_scale(1.0) {}

void Timer::reset() {
    start_time = now();
    pause_time = 0;
    paused = false;
}

void Timer::pause() {
    if (!paused) {
        pause_time = now();
        paused = true;
    }
}

void Timer::resume() {
    if (paused) {
        start_time += (now() - pause_time);
        paused = false;
    }
}

double Timer::elapsed() const {
    if (paused) {
        return (pause_time - start_time) * time_scale;
    }
    return (now() - start_time) * time_scale;
}

double Timer::delta(double last_time) const {
    return (elapsed() - last_time) * time_scale;
}

void Timer::setTimeScale(float scale) {
    time_scale = scale;
}

float Timer::getTimeScale() const {
    return static_cast<float>(time_scale);
}

bool Timer::isPaused() const {
    return paused;
}

double Timer::now() {
    return std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

// ============================================================================
// Animation Implementation
// ============================================================================

Animation::Animation()
    : frame_duration(0.1), looping(true), current_frame(0),
      elapsed_time(0), finished(false) {}

Animation::Animation(const std::vector<int>& frames, double frame_duration, bool loop)
    : frames(frames), frame_duration(frame_duration), looping(loop),
      current_frame(0), elapsed_time(0), finished(false) {}

void Animation::update(double delta) {
    if (finished && !looping) return;
    if (frames.empty()) return;

    elapsed_time += delta;

    if (elapsed_time >= frame_duration) {
        elapsed_time = 0;
        current_frame++;

        if (current_frame >= static_cast<int>(frames.size())) {
            if (looping) {
                current_frame = 0;
            } else {
                current_frame = static_cast<int>(frames.size()) - 1;
                finished = true;
            }
        }
    }
}

int Animation::getCurrentFrame() const {
    if (frames.empty()) return 0;
    return frames[current_frame];
}

void Animation::reset() {
    current_frame = 0;
    elapsed_time = 0;
    finished = false;
}

void Animation::setLooping(bool loop) {
    looping = loop;
}

bool Animation::isFinished() const {
    return finished;
}

void Animation::setFrameDuration(double duration) {
    frame_duration = duration;
}

// ============================================================================
// Input Implementation
// ============================================================================

void Input::setKeyDown(int key, bool down) {
    bool was_down = keys_down[key];
    keys_down[key] = down;

    if (down && !was_down) {
        keys_pressed[key] = true;
    } else if (!down && was_down) {
        keys_released[key] = true;
    }
}

void Input::setMouseButton(int button, bool down) {
    mouse_buttons[button] = down;
}

void Input::setMousePos(float x, float y) {
    mouse_pos = Vector2D(x, y);
}

void Input::update() {
    keys_pressed.clear();
    keys_released.clear();
}

bool Input::isKeyDown(int key) {
    return keys_down[key];
}

bool Input::isKeyPressed(int key) {
    return keys_pressed[key];
}

bool Input::isKeyReleased(int key) {
    return keys_released[key];
}

bool Input::isMouseButtonDown(int button) {
    return mouse_buttons[button];
}

Vector2D Input::getMousePos() {
    return mouse_pos;
}

// ============================================================================
// Transform Implementation
// ============================================================================

Transform::Transform() : position(0, 0), rotation(0), scale(1, 1) {}

Transform::Transform(const Vector2D& pos, float rot, const Vector2D& s)
    : position(pos), rotation(rot), scale(s) {}

void Transform::translate(const Vector2D& offset) {
    position = position + offset;
}

void Transform::rotate(float radians) {
    rotation += radians;
}

void Transform::scaleBy(float factor) {
    scale = scale * factor;
}

Vector2D Transform::transformPoint(const Vector2D& point) const {
    Vector2D scaled = Vector2D(point.x * scale.x, point.y * scale.y);
    Vector2D rotated = scaled.rotate(rotation);
    return rotated + position;
}

// ============================================================================
// Sprite Implementation
// ============================================================================

Sprite::Sprite()
    : source_rect(0, 0, 0, 0), tint(Color::White()),
      visible(true), texture_id(0) {}

Sprite::Sprite(int texture_id, const Rect& source)
    : source_rect(source), tint(Color::White()),
      visible(true), texture_id(texture_id) {}

void Sprite::setPosition(const Vector2D& pos) {
    transform.position = pos;
}

void Sprite::setRotation(float rot) {
    transform.rotation = rot;
}

void Sprite::setScale(const Vector2D& scale) {
    transform.scale = scale;
}

void Sprite::move(const Vector2D& offset) {
    transform.translate(offset);
}

Rect Sprite::getBounds() const {
    return Rect(
        transform.position.x,
        transform.position.y,
        source_rect.width * transform.scale.x,
        source_rect.height * transform.scale.y
    );
}

// ============================================================================
// Camera2D Implementation
// ============================================================================

Camera2D::Camera2D()
    : position(0, 0), viewport_size(800, 600),
      zoom(1.0f), target_position(0, 0), follow_speed(1.0f) {}

Camera2D::Camera2D(float width, float height)
    : position(0, 0), viewport_size(width, height),
      zoom(1.0f), target_position(0, 0), follow_speed(1.0f) {}

void Camera2D::setPosition(const Vector2D& pos) {
    position = pos;
}

void Camera2D::setZoom(float z) {
    zoom = std::max(0.1f, z);
}

void Camera2D::setViewportSize(float width, float height) {
    viewport_size = Vector2D(width, height);
}

void Camera2D::follow(const Vector2D& target, float speed) {
    target_position = target;
    follow_speed = speed;
}

void Camera2D::update(double delta) {
    position = position.lerp(target_position, static_cast<float>(follow_speed * delta));
}

Vector2D Camera2D::getPosition() const {
    return position;
}

float Camera2D::getZoom() const {
    return zoom;
}

Vector2D Camera2D::worldToScreen(const Vector2D& world_pos) const {
    Vector2D offset = world_pos - position;
    return Vector2D(
        offset.x * zoom + viewport_size.x / 2,
        offset.y * zoom + viewport_size.y / 2
    );
}

Vector2D Camera2D::screenToWorld(const Vector2D& screen_pos) const {
    Vector2D centered = Vector2D(
        screen_pos.x - viewport_size.x / 2,
        screen_pos.y - viewport_size.y / 2
    );
    return position + (centered / zoom);
}

Rect Camera2D::getViewBounds() const {
    float half_width = (viewport_size.x / 2) / zoom;
    float half_height = (viewport_size.y / 2) / zoom;
    return Rect(
        position.x - half_width,
        position.y - half_height,
        half_width * 2,
        half_height * 2
    );
}

// ============================================================================
// Grid Implementation
// ============================================================================

Vector2D Grid::worldToGrid(const Vector2D& world_pos, int tile_size) {
    return Vector2D(
        std::floor(world_pos.x / tile_size),
        std::floor(world_pos.y / tile_size)
    );
}

Vector2D Grid::gridToWorld(int grid_x, int grid_y, int tile_size) {
    return Vector2D(
        static_cast<float>(grid_x * tile_size),
        static_cast<float>(grid_y * tile_size)
    );
}

int Grid::getIndex(int x, int y, int width) {
    return y * width + x;
}

Vector2D Grid::getCoords(int index, int width) {
    return Vector2D(
        static_cast<float>(index % width),
        static_cast<float>(index / width)
    );
}

std::vector<Vector2D> Grid::getNeighbors(int x, int y, int width, int height, bool diagonals) {
    std::vector<Vector2D> neighbors;

    int offsets[][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    int count = diagonals ? 8 : 4;

    for (int i = 0; i < count; ++i) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            neighbors.push_back(Vector2D(static_cast<float>(nx), static_cast<float>(ny)));
        }
    }

    return neighbors;
}

} // namespace includecpp
