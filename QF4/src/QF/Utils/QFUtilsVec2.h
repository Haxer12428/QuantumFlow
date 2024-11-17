#pragma once
// Vec2.h
#pragma once

#include <string>
#include <format>
#include <windows.h>
#include "imgui.h"
#include <vector>

namespace QF
{
  namespace Utils
  {
    class Vec2
    {
    public:
      /* Constructors */
      ~Vec2() = default;
      Vec2();
      Vec2(float _x);
      Vec2(float _x, float _y);
      Vec2(const std::vector<float>& _Value);
      Vec2(const std::vector<float>& _Value, size_t _IndexX, size_t _IndexY);

      /* Operators */
      inline Vec2 operator+(const Vec2& _Other);
      inline Vec2 operator-(const Vec2& _Other);
      inline Vec2 operator*(const Vec2& _Other);
      inline Vec2 operator*(const float& _Other);
      inline Vec2 operator/(const Vec2& _Other);
      inline Vec2 operator/(const float& _Other);
      Vec2& operator=(const Vec2& _Other);

      const bool operator==(const Vec2& _Other);

      inline friend Vec2 operator+(const Vec2& _Self, const Vec2& _Other) {
        return { _Self.x + _Other.x, _Self.y + _Other.y };
      }

      inline friend Vec2 operator*(const float& _Self, const Vec2& _Other)
      {
        return { _Self * _Other.x, _Self * _Other.y };
      }

      inline friend Vec2 operator-(const Vec2& _Self, const Vec2& _Other) {
        return { _Self.x - _Other.x, _Self.y - _Other.y };
      }

      inline friend Vec2 operator/(const Vec2& _Self, float _Other) {
        if (_Other == 0.0f) return _Self;
        return { _Self.x / _Other, _Self.y / _Other };
      }

      inline operator ImVec2() const { return ImVec2{ x, y }; };

      /* Transformations */
      const std::string g_String() const;
      
      static const Vec2 g_PositionFromRECT(const RECT& _Rect);
      static const Vec2 g_SizeFromRECT(const RECT& _Rect);

      const std::vector<float> g_DataVector() const;
      /* Checks */
      const bool is_InBounds(const Vec2& _First, const Vec2& _Size) const; 

      /* Public variables */
      float x;
      float y;
    };
  }
}
