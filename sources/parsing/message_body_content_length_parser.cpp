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

#include <algorithm>
#include <cstdlib>

#include <netflex/parsing/message_body_content_length_parser.hpp>

namespace netflex {

namespace parsing {

//!
//! ctor
//!
message_body_content_length_parser::message_body_content_length_parser(http::request& request)
: parser_iface(request)
, m_uContentLength(fetch_content_length()) {}


//!
//! parser_iface impl
//!
parser_iface&
message_body_content_length_parser::operator<<(std::string& buffer) {
  fetch_body(buffer);

  return *this;
}

bool
message_body_content_length_parser::is_done(void) const {
  return m_sBody.length() == m_uContentLength;
}


//!
//! fetch body
//!
void
message_body_content_length_parser::fetch_body(std::string& sBuffer) {
  if (is_done()) {
    return;
  }

  std::size_t uRemaining        = m_uContentLength - m_sBody.length();
  std::size_t uNbBytesToRead    = std::min(uRemaining, sBuffer.length());

  //! fetch iterator to last byte to read in buffer
  auto posBufferLastByte        = sBuffer.begin();
  std::advance(posBufferLastByte, uNbBytesToRead);

  //! fetch bytes from buffer
  m_sBody.insert(m_sBody.end(), sBuffer.begin(), posBufferLastByte);
  //! erase bytes from buffer
  sBuffer.erase(sBuffer.begin(), posBufferLastByte);

  if (is_done()) {
    //! store body in request
    m_request.set_body(m_sBody);
  }
}


//!
//! fetch content length
//!
unsigned int
message_body_content_length_parser::fetch_content_length(void) const {
  if (m_request.has_header("Content-Length")) {
    return std::strtoul(m_request.get_header("Content-Length").c_str(), nullptr, 10);
  }

  return 0;
}

} // namespace parsing

} // namespace netflex
