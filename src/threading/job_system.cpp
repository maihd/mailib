#include <threading/job_system.h>
#include <threading/thread_functions.h>

constexpr int32_t JOB_SYSTEM_MAX_WORKERS    = 16;
constexpr int32_t JOB_SYSTEM_MAX_JOBS       = 32;

inline int32_t int32_min(int32_t x, int32_t y)
{
	return x < y ? x : y;
}

inline int32_t int32_max(int32_t x, int32_t y)
{
	return x > y ? x : y;
}

/// This data structure only use for JobSystem
struct Job
{
    JobFunc*        func;
    void*           data;
};

/// The job system for parallel computing. The workers donot steal job.
/// The JobSystem must be alive long enough before worker threads done,
/// should allocate JobSystem's memory in global/static scope, main() function, or heap.
struct JobSystemState
{
    ThreadDesc      worker_threads[JOB_SYSTEM_MAX_WORKERS]              = {};   // Workers' thread description

    Job             jobs[JOB_SYSTEM_MAX_WORKERS][JOB_SYSTEM_MAX_JOBS]   = {};   // Jobs ring buffer
    int32_t         queue_index[JOB_SYSTEM_MAX_WORKERS]                 = {};   // Safe index to queue a new job
    int32_t         running_index[JOB_SYSTEM_MAX_WORKERS]               = {};   // The current running job
    int32_t         current_worker_index                                = 0;    // Find out which worker receive the job

    ThreadMutex     mutex                                               = {};
    ThreadSignal    idle_signal                                         = {};
    ThreadSignal    queue_signal                                        = {};

    int32_t         worker_count                                        = 0;
    int32_t         idle_worker_count                                   = 0;

    volatile bool   running                                             = false;

    static int thread_func(void* data)
    {
        JobSystemState* jobSystem = (JobSystemState*)data;

        jobSystem->main_loop();

        return 0;
    }

    void init(int32_t requestWorkers)
    {
        const int32_t thread_count = int32_max(1, threading_get_cpu_cores());
        const int32_t worker_count = requestWorkers <= 0 ? thread_count : int32_min(thread_count, int32_min(requestWorkers, JOB_SYSTEM_MAX_WORKERS));

        thread_mutex_init(&this->mutex);
        thread_signal_init(&this->idle_signal);
        thread_signal_init(&this->queue_signal);

        this->running = true;

        this->idle_worker_count = 0;

        this->worker_count = worker_count;
        for (int i = 0; i < worker_count; i++)
        {
            this->queue_index[i] = 0;
            this->running_index[i] = 0;
            thread_run(this->worker_threads[i].func, this, &this->worker_threads[i]);
        }

        this->wait_idle();
    }

    void deinit()
    {
        thread_mutex_lock(&this->mutex);
        this->running = false;
		thread_mutex_unlock(&this->mutex);

        thread_signal_broadcast(&this->queue_signal);

        for (int i = 0, n = this->worker_count; i < n; i++)
        {
            this->queue_index[i] = 0;
            this->running_index[i] = 0;
            thread_join(&this->worker_threads[i]);
        }

        thread_signal_release(&this->queue_signal);
		thread_signal_release(&this->idle_signal);
		thread_mutex_release(&this->mutex);
    }

    bool is_idle_non_locking()
    {
        bool all_workers_idle = true;
        for (int i = 0, n = this->worker_count; i < n; i++)
        {
            if (this->queue_index[i] != this->running_index[i])
            {
                all_workers_idle = false;
                break;
            }
        }

        return (all_workers_idle && (this->idle_worker_count == this->worker_count)) || !this->running;
    }

    bool is_idle()
    {
		thread_mutex_lock(&this->mutex);

        const bool is_idle = is_idle_non_locking();

		thread_mutex_unlock(&this->mutex);
        return is_idle;
    }

    void wait_idle()
    {
		thread_mutex_lock(&this->mutex);

        while (!is_idle_non_locking())
        {
			thread_signal_wait(&this->idle_signal, &this->mutex);
        }

		thread_mutex_unlock(&this->mutex);
    }

    void queue_job(JobFunc* func, void* items)
    {
		thread_mutex_lock(&this->mutex);

        const int worker_index = this->current_worker_index;

        Job* jobs = this->jobs[worker_index];
        int* queue_index = &this->queue_index[worker_index];

        jobs[*queue_index] = Job{ func, items };
        *queue_index = (*queue_index + 1) % JOB_SYSTEM_MAX_JOBS;

        this->current_worker_index = (worker_index + 1) % this->worker_count;


		thread_mutex_unlock(&this->mutex);
		thread_signal_broadcast(&this->queue_signal);
    }

    void main_loop()
    {
		thread_mutex_lock(&this->mutex);

        const int worker_index = this->current_worker_index;

        Job* jobs = this->jobs[worker_index];
        int* queue_index = &this->queue_index[worker_index];
        int* running_index = &this->running_index[worker_index];

        this->current_worker_index = (this->current_worker_index + 1) % this->worker_count;
		thread_mutex_unlock(&this->mutex);

        while (this->running)
        {
			thread_mutex_lock(&this->mutex);

            this->idle_worker_count++;
            while (this->running && *queue_index == *running_index)
            {
				thread_signal_broadcast(&this->idle_signal);
				thread_signal_wait(&this->queue_signal, &this->mutex);
            }
            this->idle_worker_count--;
			thread_mutex_unlock(&this->mutex);

            while (*queue_index != *running_index)
            {
				thread_mutex_lock(&this->mutex);
                Job job = jobs[*running_index];

                *running_index = (*running_index + 1) % JOB_SYSTEM_MAX_JOBS;

				thread_mutex_unlock(&this->mutex);

                job.func(job.data);
            }
        }

		thread_mutex_lock(&this->mutex);

        this->idle_worker_count++;
		thread_signal_broadcast(&this->idle_signal);

		thread_mutex_unlock(&this->mutex);
    }
};

static JobSystemState g_job_system_state;

void job_system_setup(void)
{
    g_job_system_state.init(-1);
}

void job_system_shutdown(void)
{
    g_job_system_state.deinit();
}

bool job_system_is_idle(void)
{
    return g_job_system_state.is_idle();
}

void job_system_wait_idle(void)
{
    g_job_system_state.wait_idle();
}

void job_system_queue(JobFunc* func, void* items)
{
    g_job_system_state.queue_job(func, items);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
