#include <Threading/JobSystem.h>
#include <Threading/ThreadFunctions.h>

constexpr int32_t JOB_SYSTEM_MAX_WORKERS    = 16;
constexpr int32_t JOB_SYSTEM_MAX_JOBS       = 32;

inline int32_t min(int32_t x, int32_t y)
{
	return x < y ? x : y;
}

inline int32_t max(int32_t x, int32_t y)
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
    ThreadDesc      workerThreads[JOB_SYSTEM_MAX_WORKERS]               = {};   // Workers' thread description

    Job             jobs[JOB_SYSTEM_MAX_WORKERS][JOB_SYSTEM_MAX_JOBS]   = {};   // Jobs ring buffer
    int32_t         queueIndex[JOB_SYSTEM_MAX_WORKERS]                  = {};   // Safe index to queue a new job
    int32_t         runningIndex[JOB_SYSTEM_MAX_WORKERS]                = {};   // The current running job
    int32_t         currentWorkerIndex                                  = 0;    // Find out which worker receive the job

    ThreadMutex     mutex                                               = {};
    ThreadSignal    idleSignal                                          = {};
    ThreadSignal    queueSignal                                         = {};

    int32_t         workerCount                                         = 0;
    int32_t         idleWorkerCount                                     = 0;

    volatile bool   running                                             = false;

    static int ThreadFunc(void* data)
    {
        JobSystemState* jobSystem = (JobSystemState*)data;

        jobSystem->MainLoop();

        return 0;
    }

    void Create(int32_t requestWorkers)
    {
        const int32_t threadCount = max(1, Threading_GetCpuCores());
        const int32_t workerCount = requestWorkers <= 0 ? threadCount : min(threadCount, min(requestWorkers, JOB_SYSTEM_MAX_WORKERS));

        ThreadMutex_Init(&this->mutex);
        ThreadSignal_Init(&this->idleSignal);
        ThreadSignal_Init(&this->queueSignal);

        this->running = true;

        this->idleWorkerCount = 0;

        this->workerCount = workerCount;
        for (int i = 0; i < workerCount; i++)
        {
            this->queueIndex[i] = 0;
            this->runningIndex[i] = 0;
            Thread_Run(this->workerThreads[i].func, this, &this->workerThreads[i]);
        }

        this->WaitIdle();
    }

    void Destroy()
    {
        ThreadMutex_Lock(&this->mutex);
        this->running = false;
		ThreadMutex_Unlock(&this->mutex);

        ThreadSignal_Broadcast(&this->queueSignal);

        for (int i = 0, n = this->workerCount; i < n; i++)
        {
            this->queueIndex[i] = 0;
            this->runningIndex[i] = 0;
            Thread_Join(&this->workerThreads[i]);
        }

        ThreadSignal_Release(&this->queueSignal);
		ThreadSignal_Release(&this->idleSignal);
		ThreadMutex_Release(&this->mutex);
    }

    bool IsIdle_NonLocking()
    {
        bool allWorkersIdle = true;
        for (int i = 0, n = this->workerCount; i < n; i++)
        {
            if (this->queueIndex[i] != this->runningIndex[i])
            {
                allWorkersIdle = false;
                break;
            }
        }

        return (allWorkersIdle && (this->idleWorkerCount == this->workerCount)) || !this->running;
    }

    bool IsIdle()
    {
		ThreadMutex_Lock(&this->mutex);

        const bool isIdle = IsIdle_NonLocking();

		ThreadMutex_Unlock(&this->mutex);
        return isIdle;
    }

    void WaitIdle()
    {
		ThreadMutex_Lock(&this->mutex);

        while (!IsIdle_NonLocking())
        {
			ThreadSignal_Wait(&this->idleSignal, &this->mutex);
        }

		ThreadMutex_Unlock(&this->mutex);
    }

    void QueueJob(JobFunc* func, void* items)
    {
		ThreadMutex_Lock(&this->mutex);

        const int workerIndex = this->currentWorkerIndex;

        Job* jobs = this->jobs[workerIndex];
        int* queueIndex = &this->queueIndex[workerIndex];

        jobs[*queueIndex] = Job{ func, items };
        *queueIndex = (*queueIndex + 1) % JOB_SYSTEM_MAX_JOBS;

        this->currentWorkerIndex = (workerIndex + 1) % this->workerCount;


		ThreadMutex_Unlock(&this->mutex);
		ThreadSignal_Broadcast(&this->queueSignal);
    }

    void MainLoop()
    {
		ThreadMutex_Lock(&this->mutex);

        const int workerIndex = this->currentWorkerIndex;

        Job* jobs = this->jobs[workerIndex];
        int* queueIndex = &this->queueIndex[workerIndex];
        int* runningIndex = &this->runningIndex[workerIndex];

        this->currentWorkerIndex = (this->currentWorkerIndex + 1) % this->workerCount;
		ThreadMutex_Unlock(&this->mutex);

        while (this->running)
        {
			ThreadMutex_Lock(&this->mutex);

            this->idleWorkerCount++;
            while (this->running && *queueIndex == *runningIndex)
            {
				ThreadSignal_Broadcast(&this->idleSignal);
				ThreadSignal_Wait(&this->queueSignal, &this->mutex);
            }
            this->idleWorkerCount--;
			ThreadMutex_Unlock(&this->mutex);

            while (*queueIndex != *runningIndex)
            {
				ThreadMutex_Lock(&this->mutex);
                Job job = jobs[*runningIndex];

                *runningIndex = (*runningIndex + 1) % JOB_SYSTEM_MAX_JOBS;

				ThreadMutex_Unlock(&this->mutex);

                job.func(job.data);
            }
        }

		ThreadMutex_Lock(&this->mutex);

        this->idleWorkerCount++;
		ThreadSignal_Broadcast(&this->idleSignal);

		ThreadMutex_Unlock(&this->mutex);
    }
};

static JobSystemState gJobSystem;

void JobSystem_Setup(void)
{
    gJobSystem.Create(-1);
}

void JobSystem_Shutdown(void)
{
    gJobSystem.Destroy();
}

bool JobSystem_IsIdle(void)
{
    return gJobSystem.IsIdle();
}

void JobSystem_WaitIdle(void)
{
    gJobSystem.WaitIdle();
}

void JobSystem_Queue(JobFunc* func, void* items)
{
    gJobSystem.QueueJob(func, items);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
