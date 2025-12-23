SOURCE(gamekit.cpp) && HEADER(gamekit.h) gamekit

PUBLIC(
    gamekit STRUCT(Vector2D) {
        FIELD(float, x)
        FIELD(float, y)
        METHOD(length)
        METHOD(lengthSquared)
        METHOD(normalized)
        METHOD(dot)
        METHOD(distance)
        METHOD(angle)
        METHOD(rotate)
        METHOD(lerp)
    }

    gamekit STRUCT(Rect) {
        FIELD(float, x)
        FIELD(float, y)
        FIELD(float, width)
        FIELD(float, height)
        METHOD(left)
        METHOD(right)
        METHOD(top)
        METHOD(bottom)
        METHOD(center)
        METHOD(contains)
        METHOD(intersects)
        METHOD(intersection)
        METHOD(union_with)
        METHOD(move)
        METHOD(moveto)
    }

    gamekit STRUCT(Color) {
        FIELD(uint8_t, r)
        FIELD(uint8_t, g)
        FIELD(uint8_t, b)
        FIELD(uint8_t, a)
        METHOD(fromHex)
        METHOD(toHex)
        METHOD(lerp)
        METHOD(Black)
        METHOD(White)
        METHOD(Red)
        METHOD(Green)
        METHOD(Blue)
        METHOD(Yellow)
        METHOD(Cyan)
        METHOD(Magenta)
    }

    gamekit STRUCT(Circle) {
        FIELD(float, x)
        FIELD(float, y)
        FIELD(float, radius)
        METHOD(center)
        METHOD(contains)
        METHOD(intersects)
        METHOD(move)
    }

    gamekit CLASS(Collision) {
        METHOD(pointInRect)
        METHOD(pointInCircle)
        METHOD(rectRect)
        METHOD(circleCircle)
        METHOD(rectCircle)
        METHOD(closestPoint)
    }

    gamekit CLASS(Timer) {
        CONSTRUCTOR()
        METHOD(reset)
        METHOD(pause)
        METHOD(resume)
        METHOD(elapsed)
        METHOD(delta)
        METHOD(setTimeScale)
        METHOD(getTimeScale)
        METHOD(isPaused)
        METHOD(now)
    }

    gamekit CLASS(Animation) {
        CONSTRUCTOR()
        METHOD(update)
        METHOD(getCurrentFrame)
        METHOD(reset)
        METHOD(setLooping)
        METHOD(isFinished)
        METHOD(setFrameDuration)
    }

    gamekit CLASS(Input) {
        METHOD(setKeyDown)
        METHOD(setMouseButton)
        METHOD(setMousePos)
        METHOD(update)
        METHOD(isKeyDown)
        METHOD(isKeyPressed)
        METHOD(isKeyReleased)
        METHOD(isMouseButtonDown)
        METHOD(getMousePos)
    }

    gamekit STRUCT(Transform) {
        FIELD(Vector2D, position)
        FIELD(float, rotation)
        FIELD(Vector2D, scale)
        METHOD(translate)
        METHOD(rotate)
        METHOD(scaleBy)
        METHOD(transformPoint)
    }

    gamekit CLASS(Sprite) {
        CONSTRUCTOR()
        FIELD(Transform, transform)
        FIELD(Rect, source_rect)
        FIELD(Color, tint)
        FIELD(bool, visible)
        FIELD(int, texture_id)
        METHOD(setPosition)
        METHOD(setRotation)
        METHOD(setScale)
        METHOD(move)
        METHOD(getBounds)
    }

    gamekit CLASS(Camera2D) {
        CONSTRUCTOR()
        METHOD(setPosition)
        METHOD(setZoom)
        METHOD(setViewportSize)
        METHOD(follow)
        METHOD(update)
        METHOD(getPosition)
        METHOD(getZoom)
        METHOD(worldToScreen)
        METHOD(screenToWorld)
        METHOD(getViewBounds)
    }

    gamekit CLASS(Grid) {
        METHOD(worldToGrid)
        METHOD(gridToWorld)
        METHOD(getIndex)
        METHOD(getCoords)
        METHOD(getNeighbors)
    }
)
