#include "register_types.h"

/*

Copyright (c) 2020-2022 Péter Magyar

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

#include "core/version.h"

#if VERSION_MAJOR > 3
#include "core/config/engine.h"
#else
#include "core/engine.h"
#endif

#include "thread_pool.h"
#include "thread_pool_execute_job.h"
#include "thread_pool_job.h"

static ThreadPool *thread_pool = NULL;

void register_thread_pool_types() {

	ClassDB::register_class<ThreadPoolJob>();
	ClassDB::register_class<ThreadPoolExecuteJob>();

	thread_pool = memnew(ThreadPool);
	ClassDB::register_class<ThreadPool>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("ThreadPool", ThreadPool::get_singleton()));
}

void unregister_thread_pool_types() {
	if (thread_pool) {
		memdelete(thread_pool);
	}
}
