#pragma once
// nIndex is a arg because we really want to end up using a offset array or whatever
template <class T, class ... Types>
__forceinline T CallVirtualFunction(void* _this, int nIndex,  Types ... vals) {
	return ((*reinterpret_cast<T(__thiscall***)(void*, Types...)>(_this))[nIndex])(_this, vals...);
}


template <class T, class ... Types>
__forceinline T StdCallVirtualFunction(void* _this, int nIndex, Types ... vals) {
	return ((*reinterpret_cast<T(__stdcall***)(void*, Types...)>(_this))[nIndex])(_this, vals...);
}

template <class T, class ... Types>
__forceinline T CdeclCallVirtualFunction(void* _this, int nIndex, Types ... vals) {
	return ((*reinterpret_cast<T(__stdcall***)(void*, Types...)>(_this))[nIndex])(_this, vals...);
}