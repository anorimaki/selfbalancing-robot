#ifndef UTIL_OPTIONAL_H_
#define UTIL_OPTIONAL_H_


template<typename T>
class Optional
{
public:
	Optional(): m_isValid(false) {}
	Optional( const T& o ): m_obj(o), m_isValid(true) {}

	const T& get() const { return m_obj; }

	const T& operator*() const { return m_obj; }

	operator bool() const { return m_isValid; }

	const T* operator->() const { return &m_obj; }

	T* operator->() { return &m_obj; }

private:
	T m_obj;
	bool m_isValid;
};


#endif /* UTIL_OPTIONAL_H_ */
