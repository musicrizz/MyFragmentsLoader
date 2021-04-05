/*
 * Timer.h
 *
 *  Created on: Mar 21, 2021
 *      Author: gio
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "Tempo.h"

class Timer {

private:

	//puntatore a funzione da eseguire a timer
	void (*pf)();

	//
	long long int _interval;//milliseconds

	//if timeout is negative (default) , timer is infinite
	long long int _timeout;//milliseconds
	long long int _timeout_counter;

	//flag started
	bool _started,
		 _infinite;

	Tempo* _tempo;

	Timer(const Timer &other) = delete;
	Timer& operator=(const Timer &other) = delete;
	Timer& operator=(Timer &&other) = delete;

public:

	Timer(void (*function)(), long long int interval, long long int timeout = -1, bool started = true) :
			pf(function), _interval(interval), _timeout(timeout), _started(started) {
		timeout <= 0 ? _infinite = true : _infinite = false;
		started ? _tempo = new Tempo() : _tempo = nullptr;
		_timeout_counter=0;
	}

	virtual ~Timer() {
		pf = 0;
		if(_tempo != nullptr) delete _tempo;
	};

	Timer(Timer &&other) {
		this->pf = other.pf;
		this->_interval = other._interval;
		this->_started = other._started;
		this->_timeout = other._timeout;
		this->_infinite = other._infinite;
		if (this->_tempo != nullptr)delete this->_tempo;
		this->_tempo = other._tempo;
		other._tempo = nullptr;
		_timeout_counter = other._timeout_counter;
	}

	void* getFun() {
		return (void*)pf;
	}

	bool isStarted()  {
		return _started;
	}

	void start() {
		_timeout <= 0 ? _infinite = true : _infinite = false;
		_timeout_counter = 0;
		if (_tempo != nullptr) {
			_tempo->reset();
		} else {
			_tempo = new Tempo();
		}
		_started = true;
	}

	void stop()  {
		_started = false;
	}

	bool checkElapsed()  {
		if(_tempo != nullptr) {
			if(_tempo->getElapsedMill() >= _interval) {
				if(!_infinite)_timeout_counter+=_tempo->getElapsedMill();
				_tempo->updateStart();
				return true;
			}else{
				return false;
			}
		}
		return false;
	}

	bool checkTimeOut() {
		if (!_infinite) {
			if (_timeout <= _timeout_counter) {
				_infinite = false;
				_started = false;
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

};

#endif /* TIMER_H_ */
