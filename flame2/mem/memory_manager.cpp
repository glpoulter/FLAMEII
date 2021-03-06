/*!
 * \file flame2/mem/memory_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MemoryManager: management and storage class for agent data
 */
#include <utility>
#include <string>
#include "flame2/config.hpp"
#include "flame2/exceptions/mem.hpp"
#include "memory_manager.hpp"
#include "agent_shadow.hpp"

namespace flame { namespace mem {

namespace exc = flame::exceptions;

void MemoryManager::RegisterAgent(std::string agent_name) {
  std::pair<AgentMap::iterator, bool> ret;
  ret = agent_map_.insert(agent_name, new AgentMemory(agent_name));
  if (!ret.second) {
    throw exc::logic_error("Agent with that name already exists");
  }
}

VectorWrapperBase* MemoryManager::GetVectorWrapper(
    const std::string& agent_name,
    const std::string& var_name) {
  return GetAgentMemory(agent_name).GetVectorWrapper(var_name);
}

void MemoryManager::HintPopulationSize(const std::string& agent_name,
                                       unsigned int size_hint) {
  GetAgentMemory(agent_name).HintPopulationSize(size_hint);
}

AgentMemory& MemoryManager::GetAgentMemory(std::string agent_name) {
  try {
    return agent_map_.at(agent_name);
  }
  catch(const boost::bad_ptr_container_operation& E) {
    throw exc::invalid_agent("unknown agent name");
  }
}

size_t MemoryManager::GetAgentCount() const {
  return agent_map_.size();
}

bool MemoryManager::IsRegisteredAgent(const std::string& agent_name) const {
  return (agent_map_.find(agent_name) != agent_map_.end());
}

void MemoryManager::AssertVarRegistered(const std::string& agent_name,
                                    const std::string& var_name)  const {
  try {
    if (!agent_map_.at(agent_name).IsRegistered(var_name)) {
      throw exc::invalid_variable("unknown memory variable name");
    }
  }
  catch(const boost::bad_ptr_container_operation& E) {
    throw exc::invalid_agent("unknown agent name");
  }
}

AgentShadowPtr MemoryManager::GetAgentShadow(const std::string& agent_name) {
  return AgentShadowPtr(new AgentShadow(&GetAgentMemory(agent_name)));
}

#ifdef TESTBUILD
void MemoryManager::Reset() {
  agent_map_.clear();
}
#endif

}}  // namespace flame::mem
