#ifndef __CVAR__
#define __CVAR__

#include "Vector3.h"
#include "Collider.h"
#include <vector>

class Cvar // Global Value Container
{
public:

	Cvar();
	Cvar(const Cvar& copy);
	Cvar(bool bool_v);
	Cvar(int int_v);
	Cvar(unsigned int uint_v);
	Cvar(long long int int64_v);
	Cvar(unsigned long long int uint64_v);
	Cvar(double double_v);
	Cvar(float float_v);
	Cvar(const char* char_p_v);
	Cvar(vec vec_v);
	Cvar(Collider coll_v);
	Cvar(std::vector<int>& vector_i_v);
	Cvar(std::vector<float>& vector_f_v);

public:

	enum VAR_TYPE : unsigned int
	{
		UNDEFINED,
		BOOL,
		INT,
		UINT,
		INT64,
		UINT64,
		DOUBLE,
		FLOAT,
		CHAR_P,
		VEC,
		COLLIDER,
		VECTOR_INT,
		VECTOR_FLOAT
	};

protected:

	VAR_TYPE type;

	union VAR_data
	{
		bool bool_v;
		int int_v;
		unsigned int uint_v;
		long long int int64_v;
		unsigned long long int uint64_v;
		double double_v;
		float float_v;
		const char* char_p_v;
		vec vec_v;
		Collider coll_v;

		std::vector<int> vector_i_v;
		std::vector<float> vector_f_v;

		VAR_data() { }
		~VAR_data() { }
	} value;

public:

	bool SetValue(bool bool_v, bool force_type = false);
	bool SetValue(int int_v, bool force_type = false);
	bool SetValue(unsigned int uint_v, bool force_type = false);
	bool SetValue(long long int int64_v, bool force_type = false);
	bool SetValue(unsigned long long int uint64_v, bool force_type = false);
	bool SetValue(double double_v, bool force_type = false);
	bool SetValue(float float_v, bool force_type = false);
	bool SetValue(const char* char_p_v, bool force_type = false);
	bool SetValue(vec vec_v, bool force_type = false);
	bool SetValue(Collider coll_v, bool force_type = false);
	bool SetValue(std::vector<int>& vector_i_v, bool force_type = false);
	bool SetValue(std::vector<float>& vector_f_v, bool force_type = false);

	VAR_TYPE				 GetType() const;
	bool					 AsBool() const;
	int						 AsInt() const;
	unsigned int			 AsUInt() const;
	long long int			 AsInt64() const;
	unsigned long long int	 AsUInt64() const;
	double					 AsDouble() const;
	float					 AsFloat() const;
	const char*				 AsCharP() const;
	vec						 AsVec() const;
	Collider				 AsCollider() const;
	const std::vector<int>	 AsIntVector() const;
	const std::vector<float> AsFloatVector() const;
};

#endif // !__CVAR__