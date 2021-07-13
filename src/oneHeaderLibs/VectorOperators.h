#ifndef _VECTOR_OPERANDS
#define _VECTOR_OPERANDS

// Vector2:
inline Vector2 operator+(const Vector2& a, const Vector2& b) {
    return Vector2{ a.x + b.x, a.y + b.y }; 
}
inline Vector2& operator+=(Vector2& a, const Vector2& b) {
    a.x += b.x;
    a.y += b.y;
    return a; 
}

inline Vector2 operator-(const Vector2& a, const Vector2& b) {
    return Vector2{ a.x - b.x, a.y - b.y }; 
}
inline Vector2& operator-=(Vector2& a, const Vector2& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a; 
}

inline Vector2 operator*(const Vector2& a, const Vector2& b) {
    return Vector2{ a.x * b.x, a.y * b.y }; 
}
inline Vector2& operator*=(Vector2& a, const Vector2& b) {
    a.x *= b.x;
    a.y *= b.y;
    return a; 
}
inline Vector2 operator*(const float f, const Vector2& v) {
    return Vector2{ v.x * f, v.y * f }; 
}
inline Vector2 operator*(const Vector2& v, const float f) {
    return Vector2{ v.x * f, v.y * f }; 
}
inline Vector2& operator*=(Vector2& v, const float f) {
    v.x *= f;
    v.y *= f;
    return v; 
}

inline Vector2 operator/(const Vector2& a, const Vector2& b) {
    return Vector2{ a.x / b.x, a.y / b.y }; 
}
inline Vector2& operator/=(Vector2& a, const Vector2& b) {
    a.x /= b.x;
    a.y /= b.y;
    return a; 
}
inline Vector2 operator/(const float f, const Vector2& v) {
    return Vector2{ v.x / f, v.y / f }; 
}
inline Vector2 operator/(const Vector2& v, const float f) {
    return Vector2{ v.x / f, v.y / f }; 
}
inline Vector2& operator/=(Vector2& v, const float f) {
    v.x /= f;
    v.y /= f;
    return v; 
}

inline bool operator==(const Vector2& a, const Vector2& b) {
    return ((a.x == b.x) && (a.y == b.y));
}
inline bool operator!=(const Vector2& a, const Vector2& b) {
    return ((a.x != b.x) || (a.y != b.y));
}


// Vector3:
inline Vector3 operator+(const Vector3& a, const Vector3& b) {
    return Vector3{ a.x + b.x, a.y + b.y, a.z + b.z }; 
}
inline Vector3& operator+=(Vector3& a, const Vector3& b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a; 
}

inline Vector3 operator-(const Vector3& a, const Vector3& b) {
    return Vector3{ a.x - b.x, a.y - b.y, a.z - b.z }; 
}
inline Vector3& operator-=(Vector3& a, const Vector3& b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a; 
}

inline Vector3 operator*(const Vector3& a, const Vector3& b) {
    return Vector3{ a.x * b.x, a.y * b.y, a.z * b.z }; 
}
inline Vector3& operator*=(Vector3& a, const Vector3& b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    return a; 
}
inline Vector3 operator*(const float f, const Vector3& v) {
    return Vector3{ v.x * f, v.y * f, v.z * f }; 
}
inline Vector3 operator*(const Vector3& v, const float f) {
    return Vector3{ v.x * f, v.y * f, v.z * f }; 
}
inline Vector3& operator*=(Vector3& v, const float f) {
    v.x *= f;
    v.y *= f;
    v.z *= f;
    return v; 
}

inline Vector3 operator/(const Vector3& a, const Vector3& b) {
    return Vector3{ a.x / b.x, a.y / b.y, a.z / b.z }; 
}
inline Vector3& operator/=(Vector3& a, const Vector3& b) {
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    return a; 
}
inline Vector3 operator/(const float f, const Vector3& v) {
    return Vector3{ v.x / f, v.y / f, v.z / f }; 
}
inline Vector3 operator/(const Vector3& v, const float f) {
    return Vector3{ v.x / f, v.y / f, v.z / f }; 
}
inline Vector3& operator/=(Vector3& v, const float f) {
    v.x /= f;
    v.y /= f;
    v.z /= f;
    return v; 
}

inline bool operator==(const Vector3& a, const Vector3& b) {
    return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
}
inline bool operator!=(const Vector3& a, const Vector3& b) {
    return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
}


// Vector4:
inline Vector4 operator+(const Vector4& a, const Vector4& b) {
    return Vector4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; 
}
inline Vector4& operator+=(Vector4& a, const Vector4& b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a; 
}

inline Vector4 operator-(const Vector4& a, const Vector4& b) {
    return Vector4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; 
}
inline Vector4& operator-=(Vector4& a, const Vector4& b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a; 
}

inline Vector4 operator*(const Vector4& a, const Vector4& b) {
    return Vector4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; 
}
inline Vector4& operator*=(Vector4& a, const Vector4& b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    a.w *= b.w;
    return a; 
}
inline Vector4 operator*(const float f, const Vector4& v) {
    return Vector4{ v.x * f, v.y * f, v.z * f, v.w * f }; 
}
inline Vector4 operator*(const Vector4& v, const float f) {
    return Vector4{ v.x * f, v.y * f, v.z * f, v.w * f }; 
}
inline Vector4& operator*=(Vector4& v, const float f) {
    v.x *= f;
    v.y *= f;
    v.z *= f;
    v.w *= f;
    return v; 
}

inline Vector4 operator/(const Vector4& a, const Vector4& b) {
    return Vector4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; 
}
inline Vector4& operator/=(Vector4& a, const Vector4& b) {
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    a.w /= b.w;
    return a; 
}
inline Vector4 operator/(const float f, const Vector4& v) {
    return Vector4{ v.x / f, v.y / f, v.z / f, v.w / f }; 
}
inline Vector4 operator/(const Vector4& v, const float f) {
    return Vector4{ v.x / f, v.y / f, v.z / f, v.w / f }; 
}
inline Vector4& operator/=(Vector4& v, const float f) {
    v.x /= f;
    v.y /= f;
    v.z /= f;
    v.w /= f;
    return v; 
}

inline bool operator==(const Vector4& a, const Vector4& b) {
    return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w));
}
inline bool operator!=(const Vector4& a, const Vector4& b) {
    return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w));
}

#endif // _VECTOR_OPERANDS
