#pragma once

#include <cstdint>

class GameTimer {
public:
	GameTimer();

	float game_time() const;
	float delta_time() const;
	float total_time() const;

	void reset();
	void start();
	void stop();
	void tick();

private:
	double _seconds_per_count;
	double _delta_time;

	int64_t _base_time;
	int64_t _paused_time;
	int64_t _stop_time;
	int64_t _prev_time;
	int64_t _curr_time;

	bool _stopped;
};
