#ifndef __VECTOR3_H__
#define __VECTOR3_H__

template<class TYPE>
class Vector3
{
public:

	Vector3(TYPE val = 0) : x(val), y(val), z(val) {}
	Vector3(TYPE _x, TYPE _y, TYPE _z) : x(_x), y(_y), z(_z) {}

	TYPE x, y, z;

	inline void Set(TYPE val)
	{
		x = y = z = val;
	}

	inline float Distance(const Vector3& v) const
	{
		return sqrt((((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y)) + ((z - v.z) * (z - v.z))));
	}

	inline float DistanceSquared(const Vector3& v) const
	{
		return (((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y)) + ((z - v.z) * (z - v.z)));
	}


	Vector3 operator -(const Vector3 &v) const
	{
		Vector3 r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		return(r);
	}

	Vector3 operator + (const Vector3 &v) const
	{
		Vector3 r;

		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;

		return(r);
	}

	Vector3 operator * (const Vector3 &v) const
	{
		Vector3 r;

		r.x = x * v.x;
		r.y = y * v.y;
		r.z = z * v.z;

		return(r);
	}

	Vector3 operator / (const Vector3 &v) const
	{
		Vector3 r;

		r.x = x / v.x;
		r.y = y / v.y;
		r.z = z / v.z;

		return(r);
	}

	const Vector3& operator -=(const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return(*this);
	}

	const Vector3& operator +=(const Vector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return(*this);
	}

	bool operator ==(const Vector3& v) const
	{
		return (x == v.x && y == v.y && z == v.z);
	}

	bool operator !=(const Vector3& v) const
	{
		return (x != v.x || y != v.y || z == v.z);
	}
};

typedef Vector3<float> vec;
typedef Vector3<int> Ivec;

#endif // __VECTOR3_H__