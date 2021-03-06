/*!
 * \file flame2/exceptions/io.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by io managers
 */
#ifndef EXCEPTIONS__IO_HPP_
#define EXCEPTIONS__IO_HPP_
#include <string>
#include "base.hpp"

namespace flame { namespace exceptions {

class flame_io_exception : public flame_exception {
  public:
    explicit flame_io_exception(const std::string& msg)
        : flame_exception(msg) {}
};

class invalid_file_type : public flame_io_exception {
  public:
    explicit invalid_file_type(const std::string& msg)
        : flame_io_exception(msg) {}
};

class inaccessable_file : public flame_io_exception {
  public:
    explicit inaccessable_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

class unparseable_file : public flame_io_exception {
  public:
    explicit unparseable_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

class invalid_model_file : public flame_io_exception {
  public:
    explicit invalid_model_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

class invalid_pop_file : public flame_io_exception {
  public:
    explicit invalid_pop_file(const std::string& msg)
        : flame_io_exception(msg) {}
};

}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__IO_HPP_
