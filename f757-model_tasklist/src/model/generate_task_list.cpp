/*!
 * \file src/model/generate_task_list.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Generates task list
 */
#include <string>
#include <vector>
#include <algorithm>
#include "./model_manager.hpp"
#include "./task.hpp"

namespace flame { namespace model {

/*!
 * \brief Generates task list
 * \return The return code
 * Produces task list with state/data/communication dependencies.
 */
int ModelManager::generate_task_list() {
//    int rc;
    std::vector<XMachine*>::iterator agent;

    // Calculate dependencies
//    rc = calculate_dependencies(&tasks_);
    // Catalog data dependencies
//    rc = catalog_data_dependencies(&model_, &functionStateGraph);
    // Catalog data io dependencies
//    rc = catalog_dataio_dependencies(&model_, &functionStateGraph);
    // Calculate task list using dependencies
//    rc = calculate_task_list(&tasks_);

    /* For each agent */
    for (agent = model_.getAgents()->begin();
         agent != model_.getAgents()->end(); ++agent) {
        // Add condition vertices
        (*agent)->getFunctionDependencyGraph()->
                add_condition_vertices_to_graph();
        // Add init vertex
        (*agent)->add_init_vertex_to_graph();
        // Add variable verticies
        (*agent)->getFunctionDependencyGraph()->
                add_variable_vertices_to_graph((*agent)->getVariables());

        (*agent)->getFunctionDependencyGraph()->write_graphviz("test1.dot");

        // Contract variable vertices
        (*agent)->getFunctionDependencyGraph()->
                contract_variable_verticies_from_graph();

        (*agent)->getFunctionDependencyGraph()->write_graphviz("test2.dot");
    }

#ifdef TESTBUILD
    // Output function dependency graph to view via graphviz dot
    // functionStateGraph.write_dependency_graph("dgraph.dot");
//    functionStateGraph.write_graphviz();
#endif

    return 0;
}

int ModelManager::catalog_communication_dependencies_syncs(
        XModel * model, XGraph * graph) {
    std::vector<XMessage*>::iterator message;
    /* Add sync_start and sync_finish
     * for each message type */
    for (message = model->getMessages()->begin();
         message != model->getMessages()->end(); ++message) {
        /* Add sync start tasks to the task list */
        Task * syncStartTask = new Task;
        syncStartTask->setParentName((*message)->getName());
        syncStartTask->setName("sync_start");
        syncStartTask->setTaskType(Task::sync_start);
        syncStartTask->setPriorityLevel(10);
        graph->addVertex(syncStartTask);
        (*message)->setSyncStartTask(syncStartTask);
        /* Add sync finish tasks to the task list */
        Task * syncFinishTask = new Task;
        syncFinishTask->setParentName((*message)->getName());
        syncFinishTask->setName("sync_finish");
        syncFinishTask->setTaskType(Task::sync_finish);
        syncFinishTask->setPriorityLevel(1);
        graph->addVertex(syncFinishTask);
        (*message)->setSyncFinishTask(syncFinishTask);
        /* Add dependency between start and finish sync tasks */
        Dependency * d = new Dependency;
        d->setParentName((*message)->getName());
        d->setName((*message)->getName());
        d->setDependencyType(Dependency::communication);
        graph->addEdge(syncStartTask, syncFinishTask, d);
    }
    return 0;
}

int ModelManager::catalog_communication_dependencies_ioput(XModel * model,
        std::vector<XFunction*>::iterator function, XGraph * graph) {
    std::vector<XIOput*>::iterator ioput;
    std::vector<XMessage*>::iterator message;

    /* Find outputting functions */
    for (ioput = (*function)->getOutputs()->begin();
         ioput != (*function)->getOutputs()->end(); ++ioput) {
        /* Find associated messages */
        for (message = model->getMessages()->begin();
             message != model->getMessages()->end(); ++message) {
            if ((*message)->getName() == (*ioput)->getMessageName()) {
                /* Add communication dependency */
                Dependency * d = new Dependency;
                d->setName((*ioput)->getMessageName());
                d->setParentName((*ioput)->getMessageName());
                d->setDependencyType(Dependency::communication);
                graph->addEdge((*function)->getTask(),
                        (*message)->getSyncStartTask(), d);
            }
        }
    }

    /* Find inputting functions */
    for (ioput = (*function)->getInputs()->begin();
         ioput != (*function)->getInputs()->end(); ++ioput) {
        /* Find associated messages */
        for (message = model->getMessages()->begin();
                message != model->getMessages()->end(); ++message) {
            if ((*message)->getName() == (*ioput)->getMessageName()) {
                /* Add communication dependency */
                Dependency * d = new Dependency;
                d->setName((*ioput)->getMessageName());
                d->setParentName((*ioput)->getMessageName());
                d->setDependencyType(Dependency::communication);
                graph->addEdge((*message)->getSyncFinishTask(),
                        (*function)->getTask(), d);
            }
        }
    }

    return 0;
}

int ModelManager::catalog_communication_dependencies(XModel * model,
        XGraph * graph) {
    std::vector<XMachine*>::iterator agent;
    std::vector<XFunction*>::iterator function;

    /* Remove unused messages or
     * messages not read or
     * messages not sent and give warning. */

    catalog_communication_dependencies_syncs(model, graph);

    /* Find dependencies */
    /* For each agent */
    for (agent = model->getAgents()->begin();
         agent != model->getAgents()->end(); ++agent) {
        /* For each function */
        for (function = (*agent)->getFunctions()->begin();
             function != (*agent)->getFunctions()->end(); ++function) {
            catalog_communication_dependencies_ioput(model, function, graph);
        }
    }

    return 0;
}

int ModelManager::catalog_dataio_dependencies_variable(
        std::vector<XMachine*>::iterator agent,
        std::vector<XVariable*>::iterator variable,
        XGraph * graph) {
    std::vector<XFunction*>::iterator function;
    std::vector<XVariable*>::iterator variableFind;
    /* Add variable to disk task */
    Task * dataTask = new Task;
    dataTask->setParentName((*agent)->getName());
    dataTask->setName((*variable)->getName());
    dataTask->setTaskType(Task::io_pop_write);
    dataTask->setPriorityLevel(0);
    graph->addVertex(dataTask);
    /* Add dependency parents to task */
    /* Find the last function that writes each variable */
    XFunction * lastFunction = 0;
    for (function = (*agent)->getFunctions()->begin();
            function != (*agent)->getFunctions()->end(); ++function) {
        variableFind = std::find(
                (*function)->getReadWriteVariables()->begin(),
                (*function)->getReadWriteVariables()->end(), (*variable));
        if (variableFind != (*function)->getReadWriteVariables()->end()
                || lastFunction == 0) lastFunction = (*function);
    }
    /* Add data dependency */
    Dependency * d = new Dependency;
    d->setName((*variable)->getName());
    d->setParentName((*agent)->getName());
    d->setDependencyType(Dependency::data);
    graph->addEdge(lastFunction->getTask(), dataTask, d);
    /* Give function higher level */
    dataTask->setLevel(lastFunction->getTask()->getLevel()+1);

    return 0;
}

/*!
 * \brief Catalogs data dependencies
 * \ingroup FLAME_MODEL
 * \param[in] model The FLAME model
 * \param[out] tasks The task list
 * \return Return error code
 *
 * For each agent memory variable add a task for writing the variable to disk.
 */
int ModelManager::catalog_dataio_dependencies(XModel * model,
        XGraph * graph) {
    std::vector<XMachine*>::iterator agent;
    std::vector<XVariable*>::iterator variable;
    std::vector<XVariable*>::iterator variableFind;
    std::vector<XFunction*>::iterator function;

    /* For each agent */
    for (agent = model->getAgents()->begin();
         agent != model->getAgents()->end(); ++agent) {
        for (variable = (*agent)->getVariables()->begin();
                variable != (*agent)->getVariables()->end(); ++variable) {
            catalog_dataio_dependencies_variable(agent, variable, graph);
        }
    }

    return 0;
}

std::string ModelManager::taskTypeToString(Task::TaskType t) {
    /* Convert Task task to printable string */
    if (t == Task::io_pop_write) return "disk";
    else if (t == Task::sync_finish) return "comm";
    else if (t == Task::sync_start) return "comm";
    else if (t == Task::xfunction) return "func";
    /* If task not recognised return empty string */
    else
        return "";
}

void ModelManager::printTaskList(std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;

    fprintf(stdout, "Level\tPriority\tType\tName\n");
    fprintf(stdout, "-----\t--------\t----\t----\n");
    for (task = tasks->begin(); task != tasks->end(); ++task) {
        fprintf(stdout, "%u\t%u\t\t%s\t%s\n",
                static_cast<unsigned int>((*task)->getLevel()),
                static_cast<unsigned int>((*task)->getPriorityLevel()),
                taskTypeToString((*task)->getTaskType()).c_str(),
                (*task)->getFullName().c_str());
    }
}

int ModelManager::calculate_dependencies(std::vector<Task*> * tasks) {
    std::vector<Task*>::iterator task;
//    size_t ii;

    /* Initialise task levels to be zero */
    for (task = tasks->begin(); task != tasks->end(); ++task)
        (*task)->setLevel(0);

    /* Calculate layers of dependency graph */
    /* This is achieved by finding functions with no dependencies */
    /* giving them a layer no, taking those functions away and doing
     * the operation again. */
    /* Boolean to track if all tasks have been leveled */
    bool finished = false;
    /* The current level that is being populated */
    size_t currentLevel = 1;
    /* Loop while tasks still being leveled */
    while (!finished) {
        finished = true;    /* Set finished to be true, until unleveled task */
        /* For every task */
        for (task = tasks->begin(); task != tasks->end(); ++task) {
            /* If task is not leveled */
            if ((*task)->getLevel() == 0) {
                /* Boolean to track if any dependencies
                 * still need to be leveled */
                bool unleveled_dependency = false;
                /* For each dependency */
                /* Didn't use iterator here as caused valgrind errors */
/*                for (ii = 0; ii < (*task)->getParents().size(); ii++) {
                    Dependency * dependency = (*task)->getParents().at(ii);
                    // If the dependency is not leveled or just been leveled
                    // at the current level that is being populated
                    if ((dependency)->getTask()->getLevel() == 0 ||
                        (dependency)->getTask()->getLevel() == currentLevel)
                        // Set that current task has an unleveled dependency
                        unleveled_dependency = true;
                }*/
                /* If no unleveled dependencies */
                if (!unleveled_dependency)
                    /* Add task to current level */
                    (*task)->setLevel(currentLevel);
                else
                    /* Else leveling has not finished */
                    finished = false;
            }
        }
        /* Increment current level */
        currentLevel++;
    }

    return 0;
}

bool compare_task_levels(Task * i, Task * j) {
    /* If same level then use priority level */
    if (i->getLevel() == j->getLevel()) {
        return (i->getPriorityLevel() > j->getPriorityLevel());
    } else {
        return (i->getLevel() < j->getLevel());
    }
}

int ModelManager::calculate_task_list(std::vector<Task*> * tasks) {
    /* Sort the task list by level */
    sort(tasks->begin(), tasks->end(), compare_task_levels);

    printTaskList(tasks);

    return 0;
}

}}  // namespace flame::model
