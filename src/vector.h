#ifndef VECTOR_H
#define VECTOR_H

template<typename T> class Vector2
{
public:
    Vector2() :
        x(0), y(0)
    {}

    Vector2(T x, T y) :
        x(x), y(y)
    {}

    ~Vector2() = default;
public:
    T x, y;
};

template<typename T> class Vector3
{
public:
    Vector3() :
        x(0), y(0), z(0)
    {}

    Vector3(T x, T y, T z) :
        x(x), y(y), z(z)
    {}

    ~Vector3() = default;
public:
    union { T x, r; };
    union { T y, g; };
    union { T z, b; };
};

static bool operator==(Vector2<int> first, Vector2<int> second) { return first.x == second.x && first.y == second.y; }
static bool operator!=(Vector2<int> first, Vector2<int> second) { return first.x != second.x || first.y != second.y; }

static bool operator==(Vector3<int> first, Vector3<int> second) 
{ 
    return first.x == second.x && first.y == second.y && first.z == second.z;
}

static bool operator!=(Vector3<int> first, Vector3<int> second) 
{ 
    return first.x != second.x || first.y != second.y || first.z != second.z;
}

#endif