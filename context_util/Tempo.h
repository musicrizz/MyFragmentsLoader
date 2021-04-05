//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * Tempo.h
 *
 *  Created on: Nov 29, 2020
 *      Author: gio
 */
#pragma once
#ifndef OPENGL_CONTEXT_UTIL_TEMPO_H_
#define OPENGL_CONTEXT_UTIL_TEMPO_H_

#include <chrono>

/*Simple Class to create, start, update a time point
 * and get the elapsed time
 */
class Tempo {
private:

	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _current;

	Tempo(Tempo &&other) = delete;
	Tempo& operator=(Tempo &&other) = delete;

public:
	Tempo() {
		_start = std::chrono::steady_clock::now();
		_current = std::chrono::steady_clock::now();
	}
	virtual ~Tempo() {};

	Tempo(const Tempo &other) {
		this->_start = other._start;
		this->_current = other._current;
	}

	Tempo& operator=(const Tempo &other)   {
		if(this != &other)  {
			this->_start = other._start;
			this->_current = other._current;
		}
		return *this;
	}

	std::chrono::steady_clock::time_point getStart() {
		return this->_start;
	}
	std::chrono::steady_clock::time_point getCurrent() {
		return this->_current;
	}

	void updateStart() {
		this->_start = std::chrono::steady_clock::now();
	}

	void updateCurrent()  {
		this->_current = std::chrono::steady_clock::now();;
	}

	void reset()  {
		_start = std::chrono::steady_clock::now();
		_current = std::chrono::steady_clock::now();
	}

	long long int getElapsedNano()   {
		updateCurrent();
		return std::chrono::duration_cast<std::chrono::nanoseconds>(this->_current - this->_start).count();
	}

	long long int getElapsedMicro() {
		updateCurrent();
		return std::chrono::duration_cast<std::chrono::microseconds>(this->_current - this->_start).count();
	}
	long long int getElapsedMill() {
		updateCurrent();
		return std::chrono::duration_cast<std::chrono::milliseconds>(this->_current - this->_start).count();
	}
	long long int getElapsedSeconds() {
		updateCurrent();
		return std::chrono::duration_cast<std::chrono::seconds>(this->_current - this->_start).count();
	}

};

#endif /* OPENGL_CONTEXT_UTIL_TEMPO_H_ */
