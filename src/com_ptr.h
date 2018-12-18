#pragma once

template <typename T>
class ComPtr {
  private:
	T* _obj = nullptr;

  public:
	ComPtr()
	    : _obj(nullptr)
	{
	}

	ComPtr(T* obj)
	    : _obj(obj)
	{
	}

	~ComPtr()
	{
		if (_obj != nullptr)
			_obj->Release();
	}

	T* get() const
	{
		return _obj;
	}

	ULONG release()
	{
		if (_obj == nullptr)
			return 0;

		ULONG ref = _obj->Release();
		_obj      = nullptr;

		return ref;
	}

	T** operator&()
	{
		return &_obj;
	}

	operator T*() const
	{
		return _obj;
	}

	T* operator->() const
	{
		return _obj;
	}

	bool operator!() const
	{
		return _obj != nullptr;
	}
};
