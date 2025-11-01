#ifndef JOB_SYSTEM_DECLARATIONS
#define JOB_SYSTEM_DECLARATIONS

// https://stackoverflow.com/questions/15297270/problems-with-running-exe-file-built-with-visual-studio-on-another-computer

#include<vector>
#include<thread>
#include<mutex>

namespace Jaguar
{
	class Job		// Job-system job
	{
	public:
		void* Parameters;
		void (*Job_Function)(void*);

		Job() {}

		Job(void* Parametersp, void (*Job_Functionp)(void*))
		{
			Parameters = Parametersp;
			Job_Function = Job_Functionp;
		}
	};

	class Worker	// job-system worker
	{
	public:
		std::vector<Job> Job_Stack;
		std::mutex Job_Stack_Lock;
		bool Finished;					// This flag lets the job system know if this worker has finished working or not
		bool Working;					// This flag lets the worker know whether or not the job system wants them to finish working (uses local stack lock for access)
		Worker() {}

		//Worker(const Jaguar::Worker& Other)	// copy value initialiser (necessary for std::vector)
		//{
		//
		//}
	};

	struct Job_System
	{
		std::vector<Worker> Workers;
		std::vector<std::thread> Worker_Threads;

		uint8_t Last_Submitted_Job_Index;
		std::mutex Job_Submission_Mutex;

		bool Working;
	};

	void Wait_For_Job_System_Completion(Job_System* Target_System);

	void Terminate_Job_System(Job_System* Target_System);

	void Submit_Job(Job_System* System, Job Task);

	void Initialise_Job_System(Job_System* Target_System, size_t Workers);	// only required if you want to create a new job system 
}

#endif