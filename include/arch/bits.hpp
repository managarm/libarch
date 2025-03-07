#ifndef LIBARCH_BITS_HPP
#define LIBARCH_BITS_HPP

namespace arch {

template<typename B>
struct bit_mask {
	explicit bit_mask(B bits)
	: _bits(bits) { }

	explicit operator B () {
		return _bits;
	}

private:
	B _bits;
};

// represents a fixed size vector of bits with a value mask.
template<typename B>
struct masked_bit_value {
	explicit masked_bit_value(B bits, B mask)
	: _bits(bits), _mask(mask) { }

	explicit operator B () {
		return _bits;
	}

	B mask() const {
		return _mask;
	}

	B bits() const {
		return _bits;
	}

	// allow building a value from multiple bit vectors.
	masked_bit_value operator| (masked_bit_value other) const {
		return masked_bit_value(_bits | other.bits(), _mask | other.mask());
	}
	masked_bit_value &operator|= (masked_bit_value other) {
		*this = *this | other;
		return *this;
	}

	// allow masking out individual bits.
	masked_bit_value operator& (bit_mask<B> other) const {
		return masked_bit_value(_bits & static_cast<B>(other), _mask & ~static_cast<B>(other));
	}
	masked_bit_value &operator&= (bit_mask<B> other) {
		*this = *this & other;
		return *this;
	}

	masked_bit_value operator/ (masked_bit_value other) const {
		return masked_bit_value((_bits & ~other.mask()) | other.bits(), _mask);
	}
	masked_bit_value &operator/= (masked_bit_value other) {
		*this = *this / other;
		return *this;
	}

private:
	B _bits;
	B _mask;
};

// represents a fixed size vector of bits.
template<typename B>
struct bit_value {
	explicit bit_value(B bits)
	: _bits(bits) { }

	explicit operator B () {
		return _bits;
	}

	bit_value(masked_bit_value<B> v)
	: _bits{v.bits()} { }

	// allow building a value from multiple bit vectors.
	bit_value operator| (bit_value other) const {
		return bit_value(_bits | other._bits);
	}
	bit_value &operator|= (bit_value other) {
		*this = *this | other;
		return *this;
	}

	// allow building a value from multiple bit vectors.
	bit_value operator| (masked_bit_value<B> other) const {
		return bit_value(_bits | other.bits());
	}
	bit_value &operator|= (masked_bit_value<B> other) {
		*this = *this | other;
		return *this;
	}

	// allow masking out individual bits.
	bit_value operator& (bit_mask<B> other) const {
		return bit_value(_bits & static_cast<B>(other));
	}
	bit_value &operator&= (bit_mask<B> other) {
		*this = *this & other;
		return *this;
	}

	// combined masking out and building values
	bit_value operator/ (masked_bit_value<B> other) const {
		return bit_value((_bits & ~other.mask()) | other.bits());
	}
	bit_value &operator/= (masked_bit_value<B> other) {
		*this = *this / other;
		return *this;
	}

	bit_value operator~ () const {
		return bit_value(~_bits);
	}

private:
	B _bits;
};

template<typename B, typename T>
struct field {
	// allow extraction of individual fields from bit vectors.
	friend T operator& (bit_value<B> bv, field f) {
		return static_cast<T>((static_cast<B>(bv) >> f._shift) & f._mask);
	}

	explicit constexpr field(int shift, int num_bits)
	: _shift(shift), _mask((B(1) << num_bits) - 1) { }

	masked_bit_value<B> operator() (T value) const {
		return masked_bit_value<B>((static_cast<B>(value) & _mask) << _shift, _mask << _shift);
	}

	// allow inversion of this field to a bit mask.
	bit_mask<B> operator~ () const {
		return bit_mask<B>(~(_mask << _shift));
	}

private:
	int _shift;
	B _mask;
};

} // namespace arch

#endif // LIBARCH_BITS_HPP
