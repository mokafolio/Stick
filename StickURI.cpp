// #include <Stick/StickURI.hpp>
// #include <ctype.h>

// namespace stick
// {
//     String URI::s_reservedPath = "?#";
//     String URI::s_reservedQuery = "#";
//     String URI::s_reservedFragment = "";

//     URI::URI() :
//         m_port(0)
//     {

//     }

//     Error URI::parse(const String & _str)
//     {
//         Error ret;
//         parse(_str, ret);
//         return ret;
//     }

//     bool URI::operator == (const URI & _other) const
//     {
//         return (m_scheme == _other.m_scheme
//                 && m_userInfo == _other.m_userInfo
//                 && m_host == _other.m_host
//                 && m_port == _other.m_port
//                 && m_path == _other.m_path
//                 && m_query == _other.m_query
//                 && m_fragment == _other.m_fragment);
//     }

//     bool URI::operator != (const URI & _other) const
//     {
//         return !(*this == _other);
//     }

//     void URI::resolve(const URI & _relURI)
//     {
//         if (!_relURI.scheme().isEmpty())
//         {
//             m_scheme = _relURI.m_scheme;
//             m_userInfo = _relURI.m_userInfo;
//             m_host = _relURI.m_host;
//             m_port = _relURI.m_port;
//             m_path = _relURI.m_path;
//             m_query = _relURI.m_query;
//             normalize();
//         }
//         else
//         {
//             if (!_relURI.host().isEmpty())
//             {
//                 m_userInfo = _relURI.m_userInfo;
//                 m_host     = _relURI.m_host;
//                 m_port     = _relURI.m_port;
//                 m_path     = _relURI.m_path;
//                 m_query    = _relURI.m_query;
//                 normalize();
//             }
//             else
//             {
//                 if (_relURI.path().isEmpty())
//                 {
//                     if (!_relURI.query().isEmpty())
//                         m_query = _relURI.query();
//                 }
//                 else
//                 {
//                     if (_relURI.m_path[0] == '/')
//                     {
//                         m_path = _relURI.path();
//                         normalize();
//                     }
//                     else
//                     {
//                         if (!(*(m_path.rbegin()) == '/'))
//                         {
//                             // StringArray tSegs = path::segments(m_path);
//                             // tSegs.pop_back();
//                             // m_path = path::join(path::fromSegments(tSegs, true), _relURI.path());
//                         }
//                         else
//                         {
//                             //m_path = path::join(m_path, _relURI.path());
//                         }
//                         normalize();
//                     }
//                     m_query = _relURI.query();
//                 }
//             }
//         }
//         m_fragment = _relURI.fragment();

//         if (m_port == 0)
//             m_port = defaultPortForScheme(m_scheme);
//     }

//     void URI::normalize()
//     {
//         //m_path = path::normalize(m_path, !isRelative());
//     }

//     void URI::parse(const String & _uri, Error & _error)
//     {
//         if (_uri.isEmpty())
//             return;

//         String::ConstIter URIEnd = _uri.end();

//         String::ConstIter schemeBegin = _uri.begin();
//         String::ConstIter schemeEnd = schemeBegin;

//         //scheme must begin with an alphabetic char
//         if (isalpha(*schemeBegin))
//         {
//             schemeEnd = find(schemeBegin, URIEnd, ':');
//             //find scheme
//             if (schemeEnd != _uri.end())
//             {
//                 m_scheme = String(schemeBegin, schemeEnd);
//             }
//             else
//                 schemeEnd = _uri.begin();
//         }

//         //authority
//         String::ConstIter authorityBegin = schemeEnd;
//         String::ConstIter authorityEnd = schemeEnd;
//         if (!m_scheme.isEmpty())
//             authorityBegin += 1; //skip colon

//         String slashSlash(authorityBegin, authorityBegin + 2);

//         //check if there is an authority (indicate by a double slash
//         if (slashSlash == "//")
//         {
//             authorityBegin += 2; //skip authority begin ("//")

//             //there is an authority part
//             authorityEnd = findIf(authorityBegin, URIEnd, isAuthorityEnd);
//             parseAuthority(authorityBegin, authorityEnd, _error);
//             if (_error)
//                 return;
//         }
//         else
//             authorityEnd = authorityBegin;

//         //path
//         String::ConstIter pathBegin = authorityEnd;
//         String::ConstIter pathEnd = findIf(schemeEnd, URIEnd, isPathEnd);

//         m_path = decode(String(pathBegin, pathEnd), _error);

//         if (_error)
//             return;

//         //query
//         String::ConstIter queryEnd = pathEnd;
//         if (*pathEnd == '?')
//         {
//             queryEnd = find(pathEnd, URIEnd, '#');
//             m_query = decode(String(pathEnd + 1, queryEnd), _error);

//             if (_error)
//                 return;
//         }

//         //fragment
//         if (*queryEnd == '#')
//         {
//             m_fragment = decode(String(queryEnd + 1, URIEnd), _error);

//             if (_error)
//                 return;
//         }

//         normalize();
//     }

//     void URI::parseAuthority(String::ConstIter _begin, String::ConstIter _end, Error & _error)
//     {
//         //find user info
//         String::ConstIter userInfoEnd = find(_begin, _end, '@');
//         if (userInfoEnd != _end)
//         {
//             m_userInfo = String(_begin, userInfoEnd);
//             userInfoEnd++; //skip the @
//         }
//         else
//             userInfoEnd = _begin;

//         //find host
//         String::ConstIter hostBegin = userInfoEnd;
//         String::ConstIter hostEnd = _end;
//         String::ConstIter portStart;

//         //IP6
//         if (*hostBegin == '[')
//         {
//             hostBegin++;
//             String::ConstIter IP6end = find(hostBegin, _end, ']');

//             if (IP6end == _end)
//             {
//                 _error = Error(ec::BadURI, "Unterminated IP6 host!", STICK_FILE, STICK_LINE);
//                 return;
//             }

//             hostEnd = IP6end;
//             m_host = String(hostBegin, hostEnd);
//             portStart = find(hostEnd + 1, _end, ':');
//         }
//         else
//         {
//             hostEnd = find(hostBegin, _end, ':');
//             m_host = String(hostBegin, hostEnd);
//             portStart = hostEnd;
//         }

//         //we have a port
//         if (portStart != _end)
//         {
//             String port(portStart + 1, _end);
//             m_port = atoi(port.cString());
//         }
//         else
//         {
//             m_port = defaultPortForScheme(m_scheme);
//         }
//     }

//     bool URI::isAuthorityEnd(char _c)
//     {
//         if (_c == '/' || _c == '?' || _c == '#')
//             return true;
//         return false;
//     }

//     bool URI::isPathEnd(char _c)
//     {
//         if (_c == '?' || _c == '#')
//             return true;
//         return false;
//     }

//     bool URI::isEmpty() const
//     {
//         return m_scheme.isEmpty() && m_host.isEmpty() && m_path.isEmpty() && m_query.isEmpty() && m_fragment.isEmpty();
//     }

//     bool URI::isRelative() const
//     {
//         return m_scheme.isEmpty();
//     }

//     String URI::encode(const String & _str, const String & _reserved) const
//     {
//         String ret;
//         ret.reserve(64);
//         String::ConstIter it = _str.begin();
//         for (; it != _str.end(); ++it)
//         {
//             char c = *it;
//             if (isUnreservedChar(c))
//             {
//                 ret.append(c);
//             }
//             else if (_reserved.findIndex(c) != String::InvalidIndex || c <= 0x20 || c >= 0x7F)
//             {
//                 ret.append('%');
//                 //ret.append(toHexString((unsigned)(UInt8)c, 2));
//             }
//             else
//             {
//                 ret.append(c);
//             }
//         }
//         return ret;
//     }

//     String URI::decode(const String & _str, Error & _error) const
//     {
//         String ret;

//         String::ConstIter it = _str.begin();
//         for (; it != _str.end(); ++it)
//         {
//             char c = *it;
//             Int32 tmp;
//             if (c == '%')
//             {
//                 std::stringstream strstr;
//                 it++;
//                 if (it == _str.end())
//                 {
//                     _error = Error(ec::BadURI, "Two hex digits have to follow the percent sign!", STICK_FILE, STICK_LINE);
//                     return ret;
//                 }

//                 char hi = *it;
//                 it++;

//                 if (it == _str.end())
//                 {
//                     _error = Error(ec::BadURI, "Two hex digits have to follow the percent sign!", STICK_FILE, STICK_LINE);
//                     return ret;
//                 }

//                 char lo = *it;

//                 if (!(strstr << std::hex << hi << lo))
//                 {
//                     _error = Error(ec::BadURI, "The high or low digit is not a valid hex digit!", STICK_FILE, STICK_LINE);
//                     return ret;
//                 }

//                 if (!(strstr >> tmp))
//                 {
//                     _error = Error(ec::BadURI, "Error extracting char from hex digits!", STICK_FILE, STICK_LINE);
//                     return ret;
//                 }

//                 ret.append((char) tmp);
//             }
//             else
//             {
//                 ret.append(*it);
//             }
//         }

//         return ret;
//     }

//     UInt16 URI::defaultPortForScheme(const String & _scheme)
//     {
//         //see http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
//         if (_scheme == "ftp")
//             return 21;
//         else if (_scheme == "ssh")
//             return 22;
//         else if (_scheme == "telnet")
//             return 23;
//         else if (_scheme == "http")
//             return 80;
//         else if (_scheme == "nntp")
//             return 119;
//         else if (_scheme == "ldap")
//             return 389;
//         else if (_scheme == "https")
//             return 443;
//         else if (_scheme == "rtsp")
//             return 554;
//         else if (_scheme == "sip")
//             return 5060;
//         else if (_scheme == "sips")
//             return 5061;
//         else if (_scheme == "xmpp")
//             return 5222;
//         else
//             return 0;
//     }

//     String URI::authority() const
//     {
//         String ret;
//         if (!m_userInfo.isEmpty())
//         {
//             ret.append(m_userInfo, '@');
//         }
//         //Ipv6
//         if (m_host.findIndex(':') != String::InvalidIndex)
//         {
//             ret.append('[', m_host, ']');
//         }
//         else
//         {
//             ret.append(m_host);
//         }

//         if (m_port && defaultPortForScheme(m_scheme) == 0)
//         {
//             ret.append(':', toString(m_port));
//         }
//         return ret;
//     }

//     const String & URI::scheme() const
//     {
//         return m_scheme;
//     }

//     const String & URI::userInfo() const
//     {
//         return m_userInfo;
//     }

//     const String & URI::host() const
//     {
//         return m_host;
//     }

//     UInt16 URI::port() const
//     {
//         return m_port;
//     }

//     const String & URI::path() const
//     {
//         return m_path;
//     }

//     const String & URI::query() const
//     {
//         return m_query;
//     }

//     const String & URI::fragment() const
//     {
//         return m_fragment;
//     }

//     String URI::encodedPath() const
//     {
//         return encode(m_path, s_reservedPath);
//     }

//     String URI::encodedQuery() const
//     {
//         return encode(m_query, s_reservedQuery);
//     }

//     String URI::encodedFragment() const
//     {
//         return encode(m_fragment, s_reservedFragment);
//     }

//     String URI::encodedResource() const
//     {
//         String ret = encodedPath();
//         if (m_query.length())
//             ret.append("?", encodedQuery());
//         if (m_fragment.length())
//             ret.append("#", encodedFragment());
//         return ret;
//     }

//     bool URI::isUnreservedChar(char _c)
//     {
//         //see 2.3.  Unreserved Characters
//         if ((_c >= 'a' && _c <= 'z') ||
//                 (_c >= 'A' && _c <= 'Z') ||
//                 (_c >= '0' && _c <= '9') ||
//                 _c == '-' || _c == '_'   ||
//                 _c == '.' || _c == '~')
//             return true;
//         else
//             return false;
//     }

//     String toString(const URI & _uri)
//     {
//         String ret(64);
//         String auth = _uri.authority();
//         if (!_uri.scheme().isEmpty())
//             ret.append(_uri.scheme(), ":");
//         if (!auth.isEmpty() || _uri.scheme() == "file")
//         {
//             ret.append("//", auth);
//         }
//         if (!_uri.path().isEmpty())
//         {
//             if (!auth.isEmpty() && _uri.path()[0] != '/')
//                 ret.append('/');
//             ret.append(_uri.encodedPath());
//         }
//         else if (!_uri.query().isEmpty() || !_uri.fragment().isEmpty())
//         {
//             ret.append('/');
//         }
//         if (!_uri.query().isEmpty())
//         {
//             ret.append('?', _uri.encodedQuery());
//         }
//         if (!_uri.fragment().isEmpty())
//         {
//             ret.append('#', _uri.encodedFragment());
//         }
//         return ret;
//     }

// }
