/*!
 * \file src/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef EXE__TASK_MANAGER_HPP_
#define EXE__TASK_MANAGER_HPP_
#include <string>
#include "boost/ptr_container/ptr_map.hpp"
#include "include/flame.h"
#include "task.hpp"

namespace flame { namespace exe {

typedef boost::ptr_map<std::string, Task> TaskMap;

//! Task Manager object.
//! This is a singleton class - only one instance should exist throughtout
//! the simulation. Instances are accessed using TaskManager::GetInstance().
//! Apart from the GetTask methods, all others should be called during the
//! initialisation stage before threads are spawned, or guarded by mutexes
class TaskManager {
  public:
    //! Returns instance of singleton object
    //!  When used in a multithreaded environment, this should be called
    //!  at lease once before threads are spawned.
    static TaskManager& GetInstance() {
      static TaskManager instance;
      return instance;
    }

    //! Registers and returns a new Task
    Task& CreateTask(std::string task_name,
                     std::string agent_name,
                     AgentFuncPtr func_ptr);

    //! Returns a registered Task
    Task& GetTask(std::string task_name);

#ifdef TESTBUILD
    //! Delete all tasks
    void Reset();
#endif

  private:
    //! This is a singleton class. Disable manual instantiation
    TaskManager() {}
    //! This is a singleton class. Disable copy constructor
    TaskManager(const TaskManager&);
    //! This is a singleton class. Disable assignment operation
    void operator=(const TaskManager&);

    TaskMap task_map_;
};

}}  // namespace flame::exe
#endif  // EXE__TASK_MANAGER_HPP_
