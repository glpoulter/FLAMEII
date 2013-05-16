/*!
 * \file flame2/exe/scheduler.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Declaration of Scheduler
 */
#ifndef EXE__SCHEDULER_HPP_
#define EXE__SCHEDULER_HPP_
#include <map>
#include <queue>
#include <vector>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "task_interface.hpp"
#include "task_queue_interface.hpp"

namespace flame { namespace exe {

/*!
 * \brief Acts as a front for managing task scheduling
 *
 * The actual scheduling of task exection is done by task queues attached to
 * the Scheduler instance. Multiple queues can be created to handle different
 * types of tasks.
 */
class Scheduler {
  public:
    typedef size_t QueueId;
    typedef std::map<Task::TaskType, QueueId> RouteMap;

    //! Constructor. Initialises iter_count to 1
    Scheduler() : iter_count_(1) {}

    //! Constructor. Allows for custom value for initial iter_count.
    explicit Scheduler(size_t iter_count) : iter_count_(iter_count) {}

    /*!
     * \brief Creates a task queue of the given type and returns its id
     * \param[in] slots The number of slots to allocated for the queue
     * \return Id used to identify the newly created queue
     *
     * The type of queue determines the task allocation algorithm, while the
     * number of slots determines the number of concurrent tasks that can be
     * handled by the queue.
     *
     */
    template <typename T>
    QueueId CreateQueue(size_t slots) {
      TaskQueue* q = new T(slots);
      q->SetCallback(boost::bind(&Scheduler::TaskDoneCallback, this, _1));
      queues_.push_back(q);
      return (queues_.size() - 1);
    }

    /*!
     * \brief assigns a task type to the given queue
     * \param[in] qid Queue ID to assign the task type to
     * \param[in] type The task type to assign to the queue
     *
     * Throws flame::exceptions:invalid_argument if the queue id is invalid
     * or if the type has already been assigned.
     */
    void AssignType(QueueId qid, Task::TaskType type);

    /*!
     * \brief Specity tasks that can be split
     * \param[in] type Task type that should be split
     *
     * Throws flame::exceptions::invalid_argument if type has not yet been assigned
     * to a queue.
     */
    void SetSplittable(Task::TaskType type);

    /*!
     * \brief Specifies the maximum number of subtask a created per split
     * \param[in] type Task type
     * \param[in] max_tasks_per_split max tasks per split
     *
     * Throws flame::exceptions::invalid_argument if type has not yet been assigned
     * to a queue.
     */
    void SetMaxTasksPerSplit(Task::TaskType type, size_t max_tasks_per_split);

    /*!
     * \brief Returns the maximum number of subtask a created per split
     * \param[in] type Task type
     *
     * Throws flame::exceptions::invalid_argument if type has not yet been assigned
     * to a queue.
     */
    size_t GetMaxTasksPerSplit(Task::TaskType type) const;

    /*!
     * \brief Specifies the minimum vector size to maintain when splitting task
     * \param[in] type Task type
     * \param[in] min_vector_size min vector size
     *
     * Throws flame::exceptions::invalid_argument if type has not yet been assigned
     * to a queue.
     */
    void SetMinVectorSize(Task::TaskType type, size_t min_vector_size);

    /*!
     * \brief Returns the minimum vector size to maintain when splitting task
     * \param[in] type Task type
     *
     * Throws flame::exceptions::invalid_argument if type has not yet been assigned
     * to a queue.
     */
    size_t GetMinVectorSize(Task::TaskType type) const;

    /*! 
     * \brief Callback function used to indicate that a task is completed
     *
     * Assigned to each associated task queue for reverse communication
     */
    void TaskDoneCallback(Task::id_type task_id);

    /*!
     * \brief Runs a single iteration
     *
     * Tasks are retrieved from the Task Manager and all ready tasks (no
     * dependencies) are enqueued so they can be processed by the queues.
     *
     * A call to this method is blocking and returns once all tasks have been
     * completed.
     */
    void RunIteration();

  private:
    /*!
      * \brief Equeues task within the associated queue
      *
      * The queue is selected based on the task type and contents of
      * route_ map which is populated by AssignType().
      *
      * Throws flame::exceptions::invalid_type if a tasks with an unregistered
      * type is encountered.
      */
    void EnqueueTask(Task::id_type task_id);

    //! \brief Returns true if the given id is a valid queue id
    bool IsValidQueueId(QueueId id);

    boost::mutex doneq_mutex_;  //! mutex to control access to done queue
    boost::condition_variable doneq_cond_;  //! condition variable for done Q
    boost::ptr_vector<TaskQueue> queues_;  //! Collection of task queues
    RouteMap route_;  //! Map which associates task type to task queue

    /*!
     * \brief Done queue
     *
     * Temporary staging area for tasks that have been completed
     */
    std::vector<Task::id_type> doneq_;

    size_t iter_count_;
};

}}  // namespace flame::exe
#endif  // EXE__SCHEDULER_HPP_