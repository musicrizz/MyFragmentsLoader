//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * TempoMap.cpp
 *
 *  Created on: Nov 29, 2020
 *      Author: gio
 */

#include "TempoMap.h"
#include <iostream>
#include <chrono>
#include <algorithm>

std::unordered_map<std::string, Tempo*> TempoMap::tempo_map;
std::unordered_map<std::string, Timer*> TempoMap::timer_map;

std::thread TempoMap::_thread_timers;
std::mutex TempoMap::_mutex_timers;
std::mutex TempoMap::_mutex_tempo;
std::condition_variable TempoMap::_cond_var;

bool TempoMap::init_flag = false;

TempoMap::TempoMap() {
}


TempoMap::~TempoMap() {
}

//-----TEMPO ----
void TempoMap::create(std::string name)  {
	std::lock_guard<std::mutex> lk(_mutex_tempo);
	tempo_map.insert(std::pair<std::string, Tempo*>(name, new Tempo()));
}

/*
 * if @name does not exist , a new Tempo is created associated with @name
 */
inline Tempo* TempoMap::get(std::string name) {
	std::lock_guard<std::mutex> lk(_mutex_tempo);
	std::unordered_map<std::string, Tempo*>::const_iterator it = tempo_map.find(name);
	if (it != tempo_map.end()) {
		return it->second;
	}else{
		Tempo* tmp = new Tempo();
		tempo_map.insert(std::pair<std::string, Tempo*>(name, tmp));
		return tmp;
	}
}

void TempoMap::updateStart(std::string name)   {
	get(name)->updateStart();
}

void TempoMap::updateCurrent(std::string name)   {
	return get(name)->updateCurrent();
}

long long int TempoMap::getElapsedNano(std::string name) {
	return get(name)->getElapsedNano();
}

long long int TempoMap::getElapsedMicro(std::string name) {
	return get(name)->getElapsedMicro();
}

long long int TempoMap::getElapsedMill(std::string name) {
	return get(name)->getElapsedMill();
}

long long int TempoMap::getElapsedSeconds(std::string name) {
	return get(name)->getElapsedSeconds();
}

double TempoMap::getElapsedMillToSec(std::string name) {
	return double(getElapsedMill(name)) / 3600.0;
}

void TempoMap::erase(std::string name) {
	std::lock_guard<std::mutex> lk(_mutex_tempo);
	std::unordered_map<std::string, Tempo*>::const_iterator it = tempo_map.find(name);
	if (it != tempo_map.end()) {
		if(it->second)delete it->second;
		tempo_map.erase(it);
	}
}

//-------------------------------------
//--- TIMER ---

void TempoMap::init()  {
	_thread_timers = std::thread(&TempoMap::routine);
	_thread_timers.detach();
	init_flag = true;
}

void TempoMap::routine()  {
	 for(;;){
		std::unique_lock<std::mutex> lk(_mutex_timers, std::defer_lock);

		if (lk.try_lock()) {
			_cond_var.wait(lk, [&] {return !timer_map.empty();});

			std::for_each(timer_map.begin(), timer_map.end(), [](std::pair<std::string, Timer*> elm){

				if(elm.second->isStarted() && elm.second->checkElapsed() && !elm.second->checkTimeOut())  {
//					_thread_executions = std::thread( [&]()->void{ ((void (*)())elm.second->getFun())();} );
//					_thread_executions.join();
					((void (*)())elm.second->getFun())();
				}

			});
			lk.unlock();
		}

		std::this_thread::sleep_for(std::chrono::microseconds(73));
	}
}

void TempoMap::createTimer(std::string name, void (*pf)(), long long int interval, long long int timeout, bool started) {
	std::lock_guard<std::mutex> lk(_mutex_timers);
	if(timer_map.insert(std::pair<std::string, Timer*>(name, new Timer(pf,interval,timeout,started))).second) {
		if(!init_flag){ init();}
		_cond_var.notify_one();
	}
}

inline Timer* TempoMap::findTimer(std::string name)  {
	std::unordered_map<std::string, Timer*>::const_iterator it = timer_map.find(name);
	if (it != timer_map.end()) {
		return it->second;
	}
	return nullptr;
}

bool TempoMap::startTimer(std::string name) {
	std::lock_guard<std::mutex> lk(_mutex_timers);
	Timer* t = findTimer(name);
	if(t) {
		t->start();
		return true;
	}
	return false;
}

bool TempoMap::stopTimer(std::string name)  {
	std::lock_guard<std::mutex> lk(_mutex_timers);
	Timer* t = findTimer(name);
	if(t) {
		t->stop();
		return true;
	}
	return false;
}

bool TempoMap::deleteTimer(std::string name) {
	std::lock_guard<std::mutex> lk(_mutex_timers);
	return timer_map.erase(name);
}



