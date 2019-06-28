// The MIT License (MIT)
//
// Copyright (c) 2015-2017 Simon Ninon <simon.ninon@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <netflex/misc/error.hpp>
#include <netflex/parsing/request_parser.hpp>

namespace netflex {

namespace parsing {

//!
//! ctor & dtor
//!
request_parser::request_parser(void)
: m_eParsingStageCurrent(parsing_stage::start_line)
, m_ptrParserCurrent(create_parser(m_eParsingStageCurrent, m_requestCurrent)) {}


//!
//! add data to the parser
//!
request_parser&
request_parser::operator<<(const std::string& data) {
  m_sBuffer += data;

  while (build_request()) {
  }

  return *this;
}


//!
//! build request
//!
bool
request_parser::build_request(void) {
  //! feed current parser
  *m_ptrParserCurrent << m_sBuffer;

  if (m_ptrParserCurrent->is_done()) {
    //! request fully built
    if (m_eParsingStageCurrent == parsing_stage::message_body) {
      //! store request as available
      m_dequeAvailableRequests.push_back(m_requestCurrent);
      //! reset current request
      m_requestCurrent = {};
    }

    //! switch to next stage
    m_ptrParserCurrent = switch_to_next_stage(m_eParsingStageCurrent, m_requestCurrent);

    return true;
  }

  return false;
}


//!
//! get request
//!
void
request_parser::operator>>(http::request& request) {
  request = get_front();
}

const http::request&
request_parser::get_front(void) const {
  if (!request_available())
    __NETFLEX_THROW(error, "No available request");

  return m_dequeAvailableRequests.front();
}

void
request_parser::pop_front(void) {
  if (!request_available())
    __NETFLEX_THROW(error, "No available request");

  m_dequeAvailableRequests.pop_front();
}


//!
//! get incomplete request currently being parsed
//!
const http::request&
request_parser::get_currently_parsed_request(void) const {
  return m_requestCurrent;
}


//!
//! returns whether a request is available
//!
bool
request_parser::request_available(void) const {
  return !m_dequeAvailableRequests.empty();
}

} // namespace parsing

} // namespace netflex
