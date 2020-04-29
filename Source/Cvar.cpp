#include "Cvar.h"

Cvar::Cvar() : type(UNDEFINED) { value.int_v = 0; }

Cvar::Cvar(const Cvar & copy) : type(copy.type)
{
	switch (copy.type)
	{
	case BOOL: value.bool_v = copy.value.bool_v; break;
	case INT: value.int_v = copy.value.int_v; break;
	case UINT: value.uint_v = copy.value.uint_v; break;
	case INT64: value.int64_v = copy.value.int64_v; break;
	case UINT64: value.uint64_v = copy.value.uint64_v; break;
	case DOUBLE: value.double_v = copy.value.double_v; break;
	case FLOAT: value.float_v = copy.value.float_v; break;
	case CHAR_P: value.char_p_v = copy.value.char_p_v; break;
	case VEC: value.vec_v = copy.value.vec_v; break;
	case COLLIDER: value.coll_v = copy.value.coll_v; break;
	}
}

Cvar::Cvar(bool bool_v) : type(BOOL) { value.bool_v = bool_v; }

Cvar::Cvar(int int_v) : type(INT) { value.int_v = int_v; }

Cvar::Cvar(unsigned int uint_v) : type(UINT) { value.uint_v = uint_v; }

Cvar::Cvar(long long int int64_v) : type(INT64) { value.int64_v = int64_v; }

Cvar::Cvar(unsigned long long int uint64_v) : type(UINT64) { value.uint64_v = uint64_v; }

Cvar::Cvar(double double_v) : type(DOUBLE) { value.double_v = double_v; }

Cvar::Cvar(float float_v) : type(FLOAT) { value.float_v = float_v; }

Cvar::Cvar(const char * char_p_v) : type(CHAR_P) { value.char_p_v = char_p_v; }

Cvar::Cvar(vec vec_v) : type(VEC) { value.vec_v = vec_v; }

Cvar::Cvar(Collider coll_v) : type(COLLIDER) { value.coll_v = coll_v; }

Cvar::Cvar(std::vector<int>& vector_i_v) : type(VECTOR_INT) { (value.vector_i_v = vector_i_v).shrink_to_fit(); }

Cvar::Cvar(std::vector<float>& vector_f_v) : type(VECTOR_FLOAT) { (value.vector_f_v = vector_f_v).shrink_to_fit(); }

bool Cvar::SetValue(bool bool_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = BOOL;

	if (ret = (type == BOOL))
		value.bool_v = bool_v;

	return ret;
}

bool Cvar::SetValue(int int_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = INT;

	if (ret = (type == INT))
		value.int_v = int_v;
	 
	return ret;
}

bool Cvar::SetValue(unsigned int uint_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = UINT;

	if (ret = (type == UINT))
		value.uint_v = uint_v;

	return ret;
}

bool Cvar::SetValue(long long int int64_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = INT64;

	if (ret = (type == INT64))
		value.int64_v = int64_v;

	return ret;
}

bool Cvar::SetValue(unsigned long long int uint64_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = UINT64;

	if (ret = (type == UINT64))
		value.uint64_v = uint64_v;

	return ret;
}

bool Cvar::SetValue(double double_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = DOUBLE;

	if (ret = (type == DOUBLE))
		value.double_v = double_v;

	return ret;
}

bool Cvar::SetValue(float float_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = FLOAT;

	if (ret = (type == FLOAT))
		value.float_v = float_v;

	return ret;
}

bool Cvar::SetValue(const char * char_p_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = CHAR_P;

	if (ret = (type == CHAR_P))
		value.char_p_v = char_p_v;

	return ret;
}

bool Cvar::SetValue(vec vec_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = VEC;

	if (ret = (type == VEC))
		value.vec_v = vec_v;

	return ret;
}

bool Cvar::SetValue(Collider coll_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = COLLIDER;

	if (ret = (type == COLLIDER))
		value.coll_v = coll_v;

	return ret;
}

bool Cvar::SetValue(std::vector<int>& vector_i_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = VECTOR_INT;

	if (ret = (type == VECTOR_INT))
		value.vector_i_v = vector_i_v;

	return ret;
}

bool Cvar::SetValue(std::vector<float>& vector_f_v, bool force_type)
{
	bool ret = false;

	if (force_type)
		type = VECTOR_FLOAT;

	if (ret = (type == VECTOR_FLOAT))
		value.vector_f_v = vector_f_v;

	return ret;
}

Cvar::VAR_TYPE Cvar::GetType() const { return type; }

bool Cvar::AsBool() const { return value.bool_v; }

int Cvar::AsInt() const { return value.int_v; }

unsigned int Cvar::AsUInt() const { return value.uint_v; }

long long int Cvar::AsInt64() const { return value.int64_v; }

unsigned long long int Cvar::AsUInt64() const { return value.uint64_v; }

double Cvar::AsDouble() const { return value.double_v; }

float Cvar::AsFloat() const { return value.float_v; }

const char * Cvar::AsCharP() const { return value.char_p_v; }

vec Cvar::AsVec() const { return value.vec_v; }

Collider Cvar::AsCollider() const { return value.coll_v; }

const std::vector<int> Cvar::AsIntVector() const { return value.vector_i_v; }
const std::vector<float> Cvar::AsFloatVector() const { return value.vector_f_v; }
