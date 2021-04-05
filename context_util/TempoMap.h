//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * TempoMap.h
 *
 *  Created on: Nov 29, 2020
 *      Author: gio
 */
#pragma once
#ifndef OPENGL_CONTEXT_UTIL_TEMPOMAP_H_
#define OPENGL_CONTEXT_UTIL_TEMPOMAP_H_

#include <string>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Tempo.h"
#include "Timer.h"

/*
 * Map of time point Tempo
 */
class TempoMap {
private:

	static std::unordered_map<std::string, Tempo*> tempo_map;
	static std::mutex _mutex_tempo;

	static std::unordered_map<std::string, Timer*> timer_map;
	static std::thread _thread_timers;
	static std::mutex _mutex_timers;
	static std::condition_variable _cond_var;

	static bool init_flag;

	TempoMap();
	TempoMap(const TempoMap &other) = delete;
	TempoMap(TempoMap &&other) = delete;
	TempoMap& operator=(const TempoMap &other) = delete;
	TempoMap& operator=(TempoMap &&other) = delete;

	static void init();
	static void routine();
	static Timer* findTimer(std::string);

public:

	virtual ~TempoMap();

	//Create and manage Tempo

	static void create(std::string);

	static Tempo* get(std::string);

	static void erase(std::string name);

	static void updateStart(std::string);

	static void updateCurrent(std::string);

	static long long int getElapsedNano(std::string);

	static long long int getElapsedMicro(std::string);

	static long long int getElapsedMill(std::string);

	static long long int getElapsedSeconds(std::string);

	static double getElapsedMillToSec(std::string);

	//---------------------------------------

	//Createand manage Timer

	static void createTimer(std::string name,
			void (*pf)(), long long int interval, long long int timeout = -1, bool started = true);

	static bool deleteTimer(std::string name);

	static bool startTimer(std::string name);

	static bool stopTimer(std::string name);

};

#endif /* OPENGL_CONTEXT_UTIL_TEMPOMAP_H_ */
