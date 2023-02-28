#pragma once
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#elif __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

struct Vector2
{
	consteval Vector2() noexcept
		:
		x(0.0f),
		y(0.0f)
	{}

	constexpr Vector2(
		const float value) noexcept
		:
		x(value),
		y(value)
	{}

	constexpr Vector2(
		const float value1,
		const float value2) noexcept
		:
		x(value1),
		y(value2)
	{}

	union
	{
		struct
		{
			float raw[2U];
		};

		struct
		{
			float x, y;
		};

		struct
		{
			float width, height;
		};
	};
};

struct Vector3
{
	consteval Vector3() noexcept
		:
		x(0.0f),
		y(0.0f),
		z(0.0f)
	{}

	constexpr Vector3(
		const float value) noexcept
		:
		x(value),
		y(value),
		z(value)
	{}

	constexpr Vector3(
		const float value1, 
		const float value2, 
		const float value3) noexcept
		:
		x(value1),
		y(value2),
		z(value3)
	{}

	union
	{
		struct
		{
			float raw[3U];
		};

		struct
		{
			float x, y, z;
		};

		struct
		{
			float r, g, b;
		};

		struct
		{
			float width, height, depth;
		};
	};
};

struct Vector4
{
	consteval Vector4() noexcept
		:
		x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{}

	constexpr Vector4(
		const float value) noexcept
		:
		x(value),
		y(value),
		z(value),
		w(value)
	{}

	constexpr Vector4(
		const float value1,
		const float value2,
		const float value3,
		const float value4) noexcept
		:
		x(value1),
		y(value2),
		z(value3),
		w(value4)
	{}

	union
	{
		struct
		{
			float raw[4U];
		};

		struct
		{
			float x, y, z, w;
		};

		struct
		{
			float r, g, b, a;
		};
	};
};

struct Matrix4x4
{
	consteval Matrix4x4() noexcept
		:
		m1(0.0f), m2(0.0f), m3(0.0f), m4(0.0f),
		m5(0.0f), m6(0.0f), m7(0.0f), m8(0.0f),
		m9(0.0f), m10(0.0f), m11(0.0f), m12(0.0f),
		m13(0.0f), m14(0.0f), m15(0.0f), m16(0.0f)
	{}

	constexpr Matrix4x4(const float value) noexcept
		:
		m1(value), m2(0.0f), m3(0.0f), m4(0.0f),
		m5(0.0f), m6(value), m7(0.0f), m8(0.0f),
		m9(0.0f), m10(0.0f), m11(value), m12(0.0f),
		m13(0.0f), m14(0.0f), m15(0.0f), m16(value)
	{}

	constexpr Matrix4x4(
		const float value1,
		const float value2,
		const float value3,
		const float value4,
		const float value5,
		const float value6,
		const float value7,
		const float value8,
		const float value9,
		const float value10,
		const float value11,
		const float value12,
		const float value13,
		const float value14,
		const float value15,
		const float value16) noexcept
		:
		m1(value1), m2(value2), m3(value3), m4(value4),
		m5(value5), m6(value6), m7(value7), m8(value8),
		m9(value9), m10(value10), m11(value11), m12(value12),
		m13(value13), m14(value14), m15(value15), m16(value16)
	{}

	inline void Multiply(const Matrix4x4& rhs)
	{
		Matrix4x4& lhs = *this;
		Matrix4x4 result;

		result.raw[0] = rhs.raw[0 + 0] * lhs.raw[0 + 0] + rhs.raw[0 + 1] * lhs.raw[4 + 0] + rhs.raw[0 + 2] * lhs.raw[8 + 0] + rhs.raw[0 + 3] * lhs.raw[12 + 0];
		result.raw[1] = rhs.raw[0 + 0] * lhs.raw[0 + 1] + rhs.raw[0 + 1] * lhs.raw[4 + 1] + rhs.raw[0 + 2] * lhs.raw[8 + 1] + rhs.raw[0 + 3] * lhs.raw[12 + 1];
		result.raw[2] = rhs.raw[0 + 0] * lhs.raw[0 + 2] + rhs.raw[0 + 1] * lhs.raw[4 + 2] + rhs.raw[0 + 2] * lhs.raw[8 + 2] + rhs.raw[0 + 3] * lhs.raw[12 + 2];
		result.raw[3] = rhs.raw[0 + 0] * lhs.raw[0 + 3] + rhs.raw[0 + 1] * lhs.raw[4 + 3] + rhs.raw[0 + 2] * lhs.raw[8 + 3] + rhs.raw[0 + 3] * lhs.raw[12 + 3];

		result.raw[4] = rhs.raw[4 + 0] * lhs.raw[0 + 0] + rhs.raw[4 + 1] * lhs.raw[4 + 0] + rhs.raw[4 + 2] * lhs.raw[8 + 0] + rhs.raw[4 + 3] * lhs.raw[12 + 0];
		result.raw[5] = rhs.raw[4 + 0] * lhs.raw[0 + 1] + rhs.raw[4 + 1] * lhs.raw[4 + 1] + rhs.raw[4 + 2] * lhs.raw[8 + 1] + rhs.raw[4 + 3] * lhs.raw[12 + 1];
		result.raw[6] = rhs.raw[4 + 0] * lhs.raw[0 + 2] + rhs.raw[4 + 1] * lhs.raw[4 + 2] + rhs.raw[4 + 2] * lhs.raw[8 + 2] + rhs.raw[4 + 3] * lhs.raw[12 + 2];
		result.raw[7] = rhs.raw[4 + 0] * lhs.raw[0 + 3] + rhs.raw[4 + 1] * lhs.raw[4 + 3] + rhs.raw[4 + 2] * lhs.raw[8 + 3] + rhs.raw[4 + 3] * lhs.raw[12 + 3];

		result.raw[8] = rhs.raw[8 + 0] * lhs.raw[0 + 0] + rhs.raw[8 + 1] * lhs.raw[4 + 0] + rhs.raw[8 + 2] * lhs.raw[8 + 0] + rhs.raw[8 + 3] * lhs.raw[12 + 0];
		result.raw[9] = rhs.raw[8 + 0] * lhs.raw[0 + 1] + rhs.raw[8 + 1] * lhs.raw[4 + 1] + rhs.raw[8 + 2] * lhs.raw[8 + 1] + rhs.raw[8 + 3] * lhs.raw[12 + 1];
		result.raw[10] = rhs.raw[8 + 0] * lhs.raw[0 + 2] + rhs.raw[8 + 1] * lhs.raw[4 + 2] + rhs.raw[8 + 2] * lhs.raw[8 + 2] + rhs.raw[8 + 3] * lhs.raw[12 + 2];
		result.raw[11] = rhs.raw[8 + 0] * lhs.raw[0 + 3] + rhs.raw[8 + 1] * lhs.raw[4 + 3] + rhs.raw[8 + 2] * lhs.raw[8 + 3] + rhs.raw[8 + 3] * lhs.raw[12 + 3];

		result.raw[12] = rhs.raw[12 + 0] * lhs.raw[0 + 0] + rhs.raw[12 + 1] * lhs.raw[4 + 0] + rhs.raw[12 + 2] * lhs.raw[8 + 0] + rhs.raw[12 + 3] * lhs.raw[12 + 0];
		result.raw[13] = rhs.raw[12 + 0] * lhs.raw[0 + 1] + rhs.raw[12 + 1] * lhs.raw[4 + 1] + rhs.raw[12 + 2] * lhs.raw[8 + 1] + rhs.raw[12 + 3] * lhs.raw[12 + 1];
		result.raw[14] = rhs.raw[12 + 0] * lhs.raw[0 + 2] + rhs.raw[12 + 1] * lhs.raw[4 + 2] + rhs.raw[12 + 2] * lhs.raw[8 + 2] + rhs.raw[12 + 3] * lhs.raw[12 + 2];
		result.raw[15] = rhs.raw[12 + 0] * lhs.raw[0 + 3] + rhs.raw[12 + 1] * lhs.raw[4 + 3] + rhs.raw[12 + 2] * lhs.raw[8 + 3] + rhs.raw[12 + 3] * lhs.raw[12 + 3];
		
		lhs = result;
	}

	constexpr float operator[](const size_t index) const noexcept
	{
		return raw[index];
	}

	constexpr float& operator[](const size_t index) noexcept
	{
		return raw[index];
	}

	union
	{
		struct
		{
			float raw[16U];
		};

		struct
		{
			float m1, m2, m3, m4;
			float m5, m6, m7, m8;
			float m9, m10, m11, m12;
			float m13, m14, m15, m16;
		};
	};
};

constexpr Vector2 operator+(const Vector2& lhs, const Vector2& rhs) noexcept
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y
	};
}

constexpr void operator+=(Vector2& lhs, const Vector2& rhs) noexcept
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
}

constexpr Vector2 operator*(const Vector2& lhs, const Vector2& rhs) noexcept
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y
	};
}

constexpr Vector2 operator*(const Vector2& lhs, const float rhs) noexcept
{
	return
	{
		lhs.x * rhs,
		lhs.y * rhs
	};
}

constexpr void operator*=(Vector2& lhs, const Vector2& rhs) noexcept
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
}

constexpr Vector3 operator+(const Vector3& lhs, const Vector3& rhs) noexcept
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	};
}

constexpr Vector3 operator*(const Vector3& lhs, const Vector3& rhs) noexcept
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	};
}

constexpr void operator*=(Vector3& lhs, const Vector3& rhs) noexcept
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
}

constexpr Vector4 operator+(const Vector4& lhs, const Vector4& rhs) noexcept
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w
	};
}

constexpr Vector4 operator*(const Vector4& lhs, const Vector4& rhs) noexcept
{
	return
	{
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z,
		lhs.w * rhs.w
	};
}

constexpr void operator*=(Vector4& lhs, const Vector4& rhs) noexcept
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	lhs.w *= rhs.w;
}

constexpr Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept
{
	Matrix4x4 result;

	result.raw[0] = rhs.raw[0 + 0] * lhs.raw[0 + 0] + rhs.raw[0 + 1] * lhs.raw[4 + 0] + rhs.raw[0 + 2] * lhs.raw[8 + 0] + rhs.raw[0 + 3] * lhs.raw[12 + 0];
	result.raw[1] = rhs.raw[0 + 0] * lhs.raw[0 + 1] + rhs.raw[0 + 1] * lhs.raw[4 + 1] + rhs.raw[0 + 2] * lhs.raw[8 + 1] + rhs.raw[0 + 3] * lhs.raw[12 + 1];
	result.raw[2] = rhs.raw[0 + 0] * lhs.raw[0 + 2] + rhs.raw[0 + 1] * lhs.raw[4 + 2] + rhs.raw[0 + 2] * lhs.raw[8 + 2] + rhs.raw[0 + 3] * lhs.raw[12 + 2];
	result.raw[3] = rhs.raw[0 + 0] * lhs.raw[0 + 3] + rhs.raw[0 + 1] * lhs.raw[4 + 3] + rhs.raw[0 + 2] * lhs.raw[8 + 3] + rhs.raw[0 + 3] * lhs.raw[12 + 3];

	result.raw[4] = rhs.raw[4 + 0] * lhs.raw[0 + 0] + rhs.raw[4 + 1] * lhs.raw[4 + 0] + rhs.raw[4 + 2] * lhs.raw[8 + 0] + rhs.raw[4 + 3] * lhs.raw[12 + 0];
	result.raw[5] = rhs.raw[4 + 0] * lhs.raw[0 + 1] + rhs.raw[4 + 1] * lhs.raw[4 + 1] + rhs.raw[4 + 2] * lhs.raw[8 + 1] + rhs.raw[4 + 3] * lhs.raw[12 + 1];
	result.raw[6] = rhs.raw[4 + 0] * lhs.raw[0 + 2] + rhs.raw[4 + 1] * lhs.raw[4 + 2] + rhs.raw[4 + 2] * lhs.raw[8 + 2] + rhs.raw[4 + 3] * lhs.raw[12 + 2];
	result.raw[7] = rhs.raw[4 + 0] * lhs.raw[0 + 3] + rhs.raw[4 + 1] * lhs.raw[4 + 3] + rhs.raw[4 + 2] * lhs.raw[8 + 3] + rhs.raw[4 + 3] * lhs.raw[12 + 3];

	result.raw[8] = rhs.raw[8 + 0] * lhs.raw[0 + 0] + rhs.raw[8 + 1] * lhs.raw[4 + 0] + rhs.raw[8 + 2] * lhs.raw[8 + 0] + rhs.raw[8 + 3] * lhs.raw[12 + 0];
	result.raw[9] = rhs.raw[8 + 0] * lhs.raw[0 + 1] + rhs.raw[8 + 1] * lhs.raw[4 + 1] + rhs.raw[8 + 2] * lhs.raw[8 + 1] + rhs.raw[8 + 3] * lhs.raw[12 + 1];
	result.raw[10] = rhs.raw[8 + 0] * lhs.raw[0 + 2] + rhs.raw[8 + 1] * lhs.raw[4 + 2] + rhs.raw[8 + 2] * lhs.raw[8 + 2] + rhs.raw[8 + 3] * lhs.raw[12 + 2];
	result.raw[11] = rhs.raw[8 + 0] * lhs.raw[0 + 3] + rhs.raw[8 + 1] * lhs.raw[4 + 3] + rhs.raw[8 + 2] * lhs.raw[8 + 3] + rhs.raw[8 + 3] * lhs.raw[12 + 3];

	result.raw[12] = rhs.raw[12 + 0] * lhs.raw[0 + 0] + rhs.raw[12 + 1] * lhs.raw[4 + 0] + rhs.raw[12 + 2] * lhs.raw[8 + 0] + rhs.raw[12 + 3] * lhs.raw[12 + 0];
	result.raw[13] = rhs.raw[12 + 0] * lhs.raw[0 + 1] + rhs.raw[12 + 1] * lhs.raw[4 + 1] + rhs.raw[12 + 2] * lhs.raw[8 + 1] + rhs.raw[12 + 3] * lhs.raw[12 + 1];
	result.raw[14] = rhs.raw[12 + 0] * lhs.raw[0 + 2] + rhs.raw[12 + 1] * lhs.raw[4 + 2] + rhs.raw[12 + 2] * lhs.raw[8 + 2] + rhs.raw[12 + 3] * lhs.raw[12 + 2];
	result.raw[15] = rhs.raw[12 + 0] * lhs.raw[0 + 3] + rhs.raw[12 + 1] * lhs.raw[4 + 3] + rhs.raw[12 + 2] * lhs.raw[8 + 3] + rhs.raw[12 + 3] * lhs.raw[12 + 3];
	
	return result;
}

constexpr Matrix4x4 OrthographicProjection(
	const float orthoLeft,
	const float orthoRight,
	const float orthoBottom,
	const float orthoTop) noexcept
{
	float matrix[4][4]
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	matrix[0][0] = 2.0f / (orthoRight - orthoLeft);
	matrix[1][1] = 2.0f / (orthoTop - orthoBottom);
	matrix[2][2] = -1.0f;
	matrix[3][0] = -(orthoRight + orthoLeft) / (orthoRight - orthoLeft);
	matrix[3][1] = -(orthoTop + orthoBottom) / (orthoTop - orthoBottom);
	
	Matrix4x4 result;
	for (size_t i{ 0U }; i < sizeof(Matrix4x4); ++i)
	{
		float* current{ reinterpret_cast<float*>(result.raw) + i };
		*current = *(reinterpret_cast<float*>(matrix) + i);
	}
	
	return result;
}

#if _MSC_VER
#pragma warning(pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#elif __clang__
#pragma clang diagnostic pop
#endif