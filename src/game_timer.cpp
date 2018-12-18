#include "game_timer.h"
#include <Windows.h>

GameTimer::GameTimer()
    : _seconds_per_count(0.0)
    , _delta_time(-1.0)
    , _base_time(0)
    , _paused_time(0)
    , _stop_time(0)
    , _prev_time(0)
    , _curr_time(0)
    , _stopped(false)
{
	int64_t counts_per_sec;

	QueryPerformanceFrequency((LARGE_INTEGER*)&counts_per_sec);
	_seconds_per_count = 1.0 / (double)counts_per_sec;
}

void GameTimer::tick()
{
	if (_stopped) {
		_delta_time = 0.0f;
		return;
	}

	int64_t curr_time;
	QueryPerformanceCounter((LARGE_INTEGER*)&curr_time);
	_curr_time = curr_time;

	_delta_time = (_curr_time - _prev_time) * _seconds_per_count;

	_prev_time = _curr_time;

	// Force non-negative. The DXSDK's CDXUTimer mentions that if the processor
	// goes into a power save mode or we get shuffled to another processor,
	// then _delta_time can be negative.
	if (_delta_time < 0.0f)
		_delta_time = 0.0;
}

float GameTimer::delta_time() const
{
	return static_cast<float>(_delta_time);
}

void GameTimer::reset()
{
	int64_t curr_time;
	QueryPerformanceCounter((LARGE_INTEGER*)&curr_time);

	_base_time = curr_time;
	_prev_time = curr_time;
	_stop_time = 0;
	_stopped   = false;
}

void GameTimer::stop()
{
	if (!_stopped) {
		int64_t curr_time;
		QueryPerformanceCounter((LARGE_INTEGER*)&curr_time);

		_stopped   = true;
		_stop_time = curr_time;
	}
}

void GameTimer::start()
{
	int64_t start_time;
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);

	if (_stopped) {
		_paused_time += start_time - _stop_time;
		_prev_time = start_time;
		_stop_time = 0;
		_stopped   = false;
	}
}

float GameTimer::total_time() const
{
	int64_t total_time;

	if (_stopped) {
		total_time = (_stop_time - _paused_time) - _base_time;
		return static_cast<float>(total_time * _seconds_per_count);
	} else {
		total_time = (_curr_time - _paused_time) - _base_time;
		return static_cast<float>(total_time * _seconds_per_count);
	}
}
