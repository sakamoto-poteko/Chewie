#ifndef _Threader_h_
#define _Threader_h_ 

#include <vector>

class TThread;
class ThreadArgs;

class Threader
{
 public:
  Threader(int nOfThreads = 1);
  virtual ~Threader();
  static void* thread0(void* arg); // function running as threads

  virtual int execute(int threadNumber);// user functions called from thread0

  virtual int startThreads(void);// launch all threads
  virtual int stopThreads (void);// stop all threads
  virtual int join        (void);// wait for threads to be done
 
  void setNumberOfThreads (int numberOfThreads);
  int  getNumberOfThreads (void               ){return nOfThreads_;}

  bool getThreadsRun      (void               ){return threadsRun_;}

 protected:
  int  nOfThreads_;//WARNING: You can't change this name since it used in the MACROS defined in ThreadedUtilities!!!!!
  bool threadsRun_;// flags for quick abort of loops within threads
                   // may also use different flags for each thread

 private:
  std::vector<TThread*>    pThreads_;    // Thread pointer
  std::vector<ThreadArgs*> pThreadArgs_; // Thread arguments
};

///////////////////////////////////////////////////////////////////////////////////
class ThreadArgs{
 public:
  ThreadArgs(Threader* threader, int threadNumber) : theThreader_ (threader)
                                                   , threadNumber_(threadNumber)
                                                   , funcRunning_ (false)
						   {;}
  ~ThreadArgs(){;}
  Threader* getThreader    (void        ){return theThreader_;}
  int	    getThreadNumber(void        ){return threadNumber_;}
  void	    setFuncRunning (bool running){funcRunning_ = running;}
  bool	    getFuncRunning (void        ){return funcRunning_;}
 private:	
  Threader* theThreader_;
  int	    threadNumber_;
  bool	    funcRunning_;
};

#endif
