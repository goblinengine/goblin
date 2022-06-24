/* 
Copyright (c) 2019-2021 Andrii Doroshenko.
Copyright (c) 2020-2021 Goost contributors (cf. AUTHORS.md).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "scene/main/node.h"

class Stopwatch : public Node {
	GDCLASS(Stopwatch, Node);

	bool autostart = false;
    bool running = false;

	double time_elapsed = 0.0;
    double time_start = 0.0;
    double time_stop = 0.0;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	enum ProcessMode {
		PROCESS_PHYSICS,
		PROCESS_IDLE,
	};
	void set_process_mode(ProcessMode p_mode);
	ProcessMode get_process_mode() const { return process_mode; };

	void set_autostart(bool p_enable) { autostart = p_enable; }
	bool has_autostart() const { return autostart; }

	void start();
	bool is_running() const { return running; }

	void stop();
	bool is_stopped() const { return !running; }

    void reset();

    double get_time_elapsed() { return time_elapsed; }

private:
	ProcessMode process_mode = PROCESS_IDLE;
	void _set_process(bool p_process);
};

VARIANT_ENUM_CAST(Stopwatch::ProcessMode);

