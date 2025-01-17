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

#pragma once

#include <functional>
#include <string>

#include <netflex/http/method.hpp>
#include <netflex/http/request.hpp>
#include <netflex/http/response.hpp>
#include <netflex/routing/route_matcher.hpp>

namespace netflex {

namespace routing {

//!
//! define a route for the server
//! specify path, method and callback to be called
//! path can be regex and contains params (like /articles/:id)
//!
class route {
public:
  //!
  //! callback associated to the route, to be called on dispatch in case of match
  //! takes as parameter the request (const) and the response (to be modified)
  //!
  typedef std::function<void(const http::request&, http::response&)> route_callback_t;

public:
  //!
  //! ctor
  //!
  //! \param m HTTP verb of the route
  //! \param path path of the route
  //! \param callback callback to be called on dispatch in case of match
  //!
  route(http::method m, const std::string& path, const route_callback_t& callback);

  //! default dtor
  ~route(void) = default;

  //! copy ctor
  route(const route&) = default;
  //! assignment operator
  route& operator=(const route&) = default;

public:
  //!
  //! match the given http request with the underlying route to check if the requested route is this one
  //!
  //! \return true if match, false otherwise
  //!
  bool match(http::request& request) const;

public:
  //!
  //! dispatch the request (and the response) to the pre-defined route callback
  //!
  //! \param request the http request
  //! \param response the http response to return to the client
  //!
  void dispatch(const http::request& request, http::response& response) const;

private:
  //!
  //! http method of the route
  //!
  http::method          m_eHttpMethod;

  //!
  //! path of the route
  //!
  std::string           m_sPath;

  //!
  //! callback to be called on match/dispatch
  //!
  route_callback_t      m_callbackRoute;

  //!
  //! used to match a route with a requested path
  //!
  route_matcher         m_routeMatcher;
};

} // namespace routing

} // namespace netflex
