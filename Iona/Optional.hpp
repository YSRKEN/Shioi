/**
* @file Optional.hpp
* ~japanese	@brief �e�L�g�[�Ɏg����optional����
* ~japanese	@brief �\�[�X�Fhttp://qiita.com/okdshin/items/8afb5e3e6044798e162c
* ~english	@brief Definition of "optional" like boost::optional
* ~english	@brief source : http://qiita.com/okdshin/items/8afb5e3e6044798e162c
* @author (Original code by okdshin)
* @date 2016/04/26
*/
#pragma once
template<typename T>
class optional {
public:
	optional() : is_engaged_(false), value_() {}

	explicit optional(T const& value) :
		is_engaged_(true), value_(value) {}

	optional& operator=(T const& value) {
		value_ = value;
		is_engaged_ = true;
		return *this;
	}

	// bool�^�ւ̈Öٕϊ�
	operator bool() const { return is_engaged_; }

	// �����I�u�W�F�N�g�ւ̃A�N�Z�b�T
	T const& operator*() const { return value_; }
	T& operator*() { return value_; }
private:
	bool is_engaged_;
	T value_;
};
