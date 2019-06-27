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

#include <netflex/routing/route_matcher.hpp>

namespace netflex {

namespace routing {

//!
//! ctor & dtor
//!
route_matcher::route_matcher(const std::string& path) {
  build_match_regex(path);
}


//!
//! build matching regex
//!
void
route_matcher::build_match_regex(const std::string& sPath) {
  //! match var1 in /abc/:var1/def
  std::regex regexFindURLParams("/:([a-zA-Z0-9_\\-]+)");
  std::smatch smatchData;

  auto posMatched       = std::sregex_iterator(sPath.cbegin(), sPath.cend(), regexFindURLParams);
  auto const& posEnd    = std::sregex_iterator();

  while (posMatched != posEnd) {
    auto param = posMatched->str();

    if (!std::regex_match(param, smatchData, regexFindURLParams))
      continue;

    //! sm is [/:var, var]
    m_vctURLParams.push_back(smatchData[1]);

    ++posMatched;
  }

  //! transform /abc/:var1/def into /abc/([a-zA-Z0-9]*)/def to match url params values
  //! also match trailing slash, get params and #comments
  //!
  //! regex_replace(path, reg, "/([a-zA-Z0-9]+)") ==> transform /abc/:var1/def into /abc/([a-zA-Z0-9]*)/def
  //! "/?((\\?([^=]+)=([^&\\#]*))(&([^=]+)=([^&\\#]*))*)?(\\#.*)?" ==> match trailing slash, get params and #comments
  //!  > /? ==> match trailing slash
  //!  > ((\\?([^=]+)=([^&\\#]*))(&([^=]+)=([^&\\#]*))*)? ==> match get params
  //!    > (\\?([^=]+)=([^&\\#]*)) ==> match first ?var=val
  //!    > (&([^=]+)=([^&\\#]*))*)?(\\#.*)? ==> match subsequent &var=val
  //!  > (\\#.*)? ==> match #comments
  m_strMatchRegex = std::regex_replace(sPath, regexFindURLParams,
      std::string("/([a-zA-Z0-9_\\-]+)")) + "/?((\\?([^=]+)=([^&\\#]*))(&([^=]+)=([^&\\#]*))*)?(\\#.*)*";
  m_regexMatch     = std::regex(m_strMatchRegex);
}


//!
//! matching
//!
bool
route_matcher::match(const std::string& sPath, params_t& mapParams) const {
  std::smatch smatchData;

  if (!std::regex_match(sPath, smatchData, m_regexMatch))
    return false;

  //! expected url params are in sm[1..m_url_params.size()]
  for (size_t i = 1; i <= m_vctURLParams.size(); ++i)
    mapParams[m_vctURLParams[i - 1]] = smatchData[i];

  //! sm[m_url_params.size() + 1] contains get_params
  match_get_params(smatchData[m_vctURLParams.size() + 1], mapParams);

  return true;
}

void
route_matcher::match_get_params(const std::string& sPath, params_t& mapParams) const {
  std::regex regexParams("[\\?&]([^=]+)=([^&]*)");
  std::smatch smatchData;

  auto posMatched       = std::sregex_iterator(sPath.cbegin(), sPath.cend(), regexParams);
  auto const& posEnd    = std::sregex_iterator();

  while (posMatched != posEnd) {
    auto sParam = posMatched->str();

    if (!std::regex_match(sParam, smatchData, regexParams))
      continue;

    //! sm is [?key1=val1, key1, val1]
    mapParams[smatchData[1]] = smatchData[2];

    ++posMatched;
  }
}

} // namespace routing

} // namespace netflex
