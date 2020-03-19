#ifndef __VECTOR3_H__
#define __VECTOR3_H__

template<class TYPE>
class Vector3
{
public:

	Vector3(TYPE _x = 0, TYPE _y = 0, TYPE _z = 0) : x(_x), y(_y), z(_z) {}

	TYPE x, y, z;

	inline void Set(TYPE val)
	{
		x = y = z = val;
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