#ifndef LIBARCH_DMA_HPP
#define LIBARCH_DMA_HPP

#include <assert.h>
#include <cstddef>
#include <new>
#include <optional>
#include <stdint.h>
#include <utility>

namespace arch {

// ----------------------------------------------------------------------------
// DMA pool infrastructure.
// ----------------------------------------------------------------------------

struct dma_pool;

// Memory region suitable for DMA.
// DMA buffers/objects/arrays are subsets of a dma_region.
struct dma_region {
	constexpr dma_region(dma_pool *pool)
	: pool_{pool} { }

	dma_region(const dma_region &) = delete;

	dma_region &operator= (const dma_region &) = delete;

	dma_pool *pool() const {
		return pool_;
	}

	// If true, then all offsets in this region correspond to non-null pointers.
	// Otherwise, only offset zero is allowed and it corresponds to a null pointer.
	bool is_valid() const {
		return valid;
	}

	bool has_va() const {
		return static_cast<bool>(base_va);
	}

	// Allows DMA objects to access the region (if it is mapped).
	uintptr_t get_base_va() const {
		assert(base_va);
		return *base_va;
	}

	template<typename T = void>
	T *get_raw_ptr(size_t offset) const {
		return reinterpret_cast<T *>(get_base_va() + offset);
	}

protected:
	// Whether this region is valid. Only null_dma_region is invalid.
	bool valid{true};
	// Virtual address where the region is mapped (if it is mapped).
	std::optional<uintptr_t> base_va;

private:
	dma_pool *pool_;
};

// dma_region that corresponds to a nullptr (i.e., that returns false from is_valid()).
struct null_dma_region_impl : dma_region {
	constexpr null_dma_region_impl()
	: dma_region{nullptr} {
		valid = false;
		// Set base_va to zero such that get_raw_ptr() works.
		base_va = 0;
	}
};

// dma_region for pointers constructed from the host virtual address space.
struct host_dma_region_impl : dma_region {
	constexpr host_dma_region_impl()
	: dma_region{nullptr} {
		base_va = 0;
	}
};

inline constinit null_dma_region_impl null_dma_region;
inline constinit host_dma_region_impl host_dma_region;

// Fat pointer that points to a single DMA buffer/object/array inside a dma_region.
struct dma_ptr {
	dma_ptr() = default;

	dma_ptr(dma_region *region, size_t offset)
	: _region{region}, _offset{offset} { }

	explicit operator bool () {
		return _region->is_valid();
	}

	dma_pool *pool() {
		return _region->pool();
	}

	dma_region *region() {
		return _region;
	}

	size_t offset() {
		return _offset;
	}

	template<typename T = void>
	T *get_raw_ptr() const {
		return _region->get_raw_ptr<T>(_offset);
	}

	dma_ptr offset_by(size_t off) const {
		return {_region, _offset + off};
	}

private:
	dma_region *_region{&null_dma_region};
	size_t _offset{0};
};

inline dma_ptr make_host_dma_ptr(void *p) {
	if (!p)
		return {};
	return {&host_dma_region, reinterpret_cast<uintptr_t>(p)};
}

struct dma_pool {
	virtual ~dma_pool() = default;

	virtual dma_ptr allocate(size_t size, size_t count, size_t align) = 0;
	virtual void deallocate(dma_ptr ptr, size_t size, size_t count, size_t align) = 0;
};

// ----------------------------------------------------------------------------
// View classes.
// ----------------------------------------------------------------------------

struct dma_buffer_view {
	dma_buffer_view()
	: _size{0} { }

	explicit dma_buffer_view(dma_ptr ptr, size_t size)
	: _ptr{ptr}, _size{size} { }

	// For backwards compatibility: previous API took a dma_pool pointer.
	explicit dma_buffer_view(std::nullptr_t, void *data, size_t size)
	: _ptr{make_host_dma_ptr(data)}, _size{size} { }

	size_t size() const {
		return _size;
	}

	void *data() const {
		return _ptr.get_raw_ptr();
	}

	std::byte *byte_data() {
		return static_cast<std::byte *>(data());
	}

	dma_ptr get_dma_ptr() const {
		return _ptr;
	}

	dma_buffer_view subview(size_t offset, size_t chunk) const {
		assert(offset <= _size);
		assert(offset + chunk <= _size);
		return dma_buffer_view{_ptr.offset_by(offset), chunk};
	}

	dma_buffer_view subview(size_t offset) const {
		assert(offset <= _size);
		return dma_buffer_view{_ptr.offset_by(offset), _size - offset};
	}

private:
	dma_ptr _ptr;
	size_t _size;
};

template<typename T>
struct dma_object_view {
	dma_object_view() = default;

	explicit dma_object_view(dma_ptr ptr)
	: _ptr{ptr} { }

	T *data() const {
		return _ptr.get_raw_ptr<T>();
	}

	T &operator* () const {
		return *data();
	}

	T *operator-> () const {
		return data();
	}

	dma_ptr get_dma_ptr() const {
		return _ptr;
	}

private:
	dma_ptr _ptr;
};

template<typename T>
struct dma_array_view {
	dma_array_view()
	: _size{0} { }

	explicit dma_array_view(dma_ptr ptr, size_t size)
	: _ptr{ptr}, _size{size} { }

	size_t size() const {
		return _size;
	}

	T *data() const {
		return _ptr.get_raw_ptr<T>();
	}

	T &operator[] (size_t n) const {
		return data()[n];
	}

	dma_ptr get_dma_ptr() const {
		return _ptr;
	}

private:
	dma_ptr _ptr;
	size_t _size;
};

// ----------------------------------------------------------------------------
// Actual storage classes.
// ----------------------------------------------------------------------------

struct dma_buffer {
	friend void swap(dma_buffer &a, dma_buffer &b) {
		using std::swap;
		swap(a._ptr, b._ptr);
		swap(a._size, b._size);
	}

	dma_buffer()
	: _size{0} { }

	dma_buffer(dma_buffer &&other)
	: dma_buffer() {
		swap(*this, other);
	}

	explicit dma_buffer(dma_pool *pool, size_t size)
	: _size{size} {
		if(pool) {
			_ptr = pool->allocate(_size, 1, 1);
		}else{
			void *p = operator new(_size);
			_ptr = make_host_dma_ptr(p);
		}
	}

	~dma_buffer() {
		if (!_ptr)
			return;
		if(_ptr.pool()) {
			_ptr.pool()->deallocate(_ptr, _size, 1, 1);
		}else{
			operator delete(data(), _size);
		}
	}

	dma_buffer &operator= (dma_buffer other) {
		swap(*this, other);
		return *this;
	}

	operator dma_buffer_view () {
		return dma_buffer_view{_ptr, _size};
	}

	size_t size() {
		return _size;
	}

	void *data() const {
		return _ptr.get_raw_ptr();
	}

	dma_buffer_view subview(size_t offset, size_t chunk) {
		return dma_buffer_view{_ptr.offset_by(offset), chunk};
	}

	dma_buffer_view subview(size_t offset) {
		return dma_buffer_view{_ptr.offset_by(offset), _size - offset};
	}

	dma_ptr get_dma_ptr() const {
		return _ptr;
	}

private:
	dma_ptr _ptr;
	size_t _size;
};

template<typename T>
struct dma_object {
	friend void swap(dma_object &a, dma_object &b) {
		using std::swap;
		swap(a._ptr, b._ptr);
	}

	dma_object() = default;

	dma_object(dma_object &&other)
	: dma_object() {
		swap(*this, other);
	}

	template<typename... Args>
	explicit dma_object(dma_pool *pool, Args &&... args) {
		if(pool) {
			_ptr = pool->allocate(sizeof(T), 1, alignof(T));
		}else{
			auto p = operator new(sizeof(T), std::align_val_t(alignof(T)));
			_ptr = make_host_dma_ptr(p);
		}
		new (_ptr.get_raw_ptr()) T{std::forward<Args>(args)...};
	}

	~dma_object() {
		if (!_ptr)
			return;
		data()->~T();
		if(_ptr.pool()) {
			_ptr.pool()->deallocate(_ptr, sizeof(T), 1, alignof(T));
		}else{
			operator delete(data(), sizeof(T), std::align_val_t(alignof(T)));
		}
	}

	dma_object &operator= (dma_object other) {
		swap(*this, other);
		return *this;
	}

	operator dma_object_view<T> () {
		return dma_object_view<T>{_ptr};
	}

	T *data() {
		return _ptr.get_raw_ptr<T>();
	}

	T &operator* () {
		return *data();
	}

	T *operator-> () {
		return data();
	}

	dma_buffer_view view_buffer() {
		return dma_buffer_view{_ptr, sizeof(T)};
	}

	dma_ptr get_dma_ptr() const {
		return _ptr;
	}

private:
	dma_ptr _ptr;
};

template<typename T>
struct dma_array {
	friend void swap(dma_array &a, dma_array &b) {
		using std::swap;
		swap(a._ptr, b._ptr);
		swap(a._size, b._size);
	}

	dma_array()
	: _size(0) { }

	dma_array(dma_array &&other)
	: dma_array() {
		swap(*this, other);
	}

	explicit dma_array(dma_pool *pool, size_t size)
	: _size{size} {
		if(pool) {
			_ptr = pool->allocate(sizeof(T), _size, alignof(T));
		}else{
			// TODO: Check for overflow.
			auto p = operator new(sizeof(T) * _size, std::align_val_t(alignof(T)));
			_ptr = make_host_dma_ptr(p);
		}
		new (_ptr.get_raw_ptr()) T[_size];
	}

	~dma_array() {
		if (!_ptr)
			return;
		for(size_t i = 0; i < _size; ++i)
			data()[i].~T();
		if(_ptr.pool()) {
			_ptr.pool()->deallocate(_ptr, sizeof(T), _size, alignof(T));
		}else{
			// TODO: Check for overflow.
			operator delete(data(), sizeof(T) * _size, std::align_val_t(alignof(T)));
		}
	}

	dma_array &operator= (dma_array other) {
		swap(*this, other);
		return *this;
	}

	operator dma_array_view<T> () {
		return dma_array_view<T>{_ptr, _size};
	}

	size_t size() {
		return _size;
	}

	T *data() {
		return _ptr.get_raw_ptr<T>();
	}

	T &operator[] (size_t n) {
		return data()[n];
	}

	dma_buffer_view view_buffer() {
		return dma_buffer_view{_ptr, sizeof(T) * _size};
	}

	dma_ptr get_dma_ptr() const {
		return _ptr;
	}

private:
	dma_ptr _ptr;
	size_t _size;
};

} // namespace arch

#endif // LIBARCH_DMA_HPP
