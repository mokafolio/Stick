#ifndef STICK_URI_HPP
#define STICK_URI_HPP

#include <Stick/String.hpp>
#include <Stick/Error.hpp>

namespace stick
{
    /**
     * @brief A Uniform Resource Identifier implementation: http://de.wikipedia.org/wiki/Uniform_Resource_Identifier
     *
     * Conforms to the RFC 3986 specification: http://tools.ietf.org/html/rfc3986
     */
    class URI
    {
    public:

        /**
         * @brief Default Constructor, constructs an emtpy URI.
         */
        URI();

        /**
         * @brief TODO
         */
        URI(const String & _str);

        URI(const char * _cStr);

        ~URI();

        /**
         * @brief Parse the URI from a percent encoded UTF-8 string.
         */
        Error parse(const String & _str);

        /**
         * @brief Returns true if both URIs are equal.
         */
        bool operator == (const URI & _other) const;

        /**
         * @brief Returns true if both URIs are not equal.
         */
        bool operator != (const URI & _other) const;


        void resolve(const URI & _relURI);

        /**
         * @brief Removes all ambigous elements (i.e. dots & double dots from the path part) from the URI
         * @see path::normalize
         */
        void normalize();

        /**
         * @brief Returns the Scheme of the URI.
         *
         * The Scheme of a URI is the top level of a uri (i.e. http): http://en.wikipedia.org/wiki/URI_scheme
         */
        const String & scheme() const;

        /**
         * @brief Returns the user info part of the URI.
         */
        const String & userInfo() const;

        /**
         * @brief Returns the host part of the URI.
         */
        const String & host() const;

        /**
         * @brief Returns true if the URI is empty.
         */
        bool isEmpty() const;

        /**
         * @brief Returns true if the URI is relative.
         *
         * A URI is relative if the scheme part is empty.
         */
        bool isRelative() const;

        /**
         * @brief Returns the port of the URI.
         *
         * If no explicit port is specified (i.e. through an IP), it returns the default port for a specific
         * Scheme (i.e. 80 for http). 0 is returned if the port can't be deduced.
         */
        UInt16 port() const;

        /**
         * @brief Returns the authority part of the URI.
         */
        String authority() const;

        /**
         * @brief Returns the path part of the URI in utf-8
         */
        const String & path() const;

        /**
         * @brief Returns the query part of the URI in utf-8
         */
        const String & query() const;

        /**
         * @brief Returns the fragment part of the URI in utf-8
         */
        const String & fragment() const;

        /**
         * @brief Returns the path in encoded ascii form
         *
         * http://en.wikipedia.org/wiki/Percent-encoding
         */
        String encodedPath() const;

        /**
         * @brief Returns the query in encoded ascii form
         *
         * http://en.wikipedia.org/wiki/Percent-encoding
         */
        String encodedQuery() const;

        /**
         * @brief returns the fragment in encoded ascii form
         *
         * http://en.wikipedia.org/wiki/Percent-encoding
         */
        String encodedFragment() const;

        /**
         * @brief encoded path + encoded query + encoded fragment
         * @see encodedPath
         * @see encodedQuery
         * @see encodedFragment
         */
        String encodedResource() const;


    protected:

        void parse(const String & _uri, Error & _error);

        void parseAuthority(String::ConstIter _begin, String::ConstIter _end, Error & _error);

        static bool isAuthorityEnd(char _c);

        static bool isPathEnd(char _c);

        static UInt16 defaultPortForScheme(const String & _scheme);

        //encoding/decoding helpers
        static String s_reservedPath;

        static String s_reservedQuery;

        static String s_reservedFragment;


        static bool isUnreservedChar(char _c);

        String encode(const String & _str, const String & _reserved) const;

        String decode(const String & _str, Error & _error) const;

    private:

        String m_scheme;
        String m_userInfo;
        String m_host;
        UInt16 m_port;
        String m_path;
        String m_query;
        String m_fragment;
    };

    //to string conversion (will percent encode the URI)
    //@TODO: add optional allocator argument
    String toString(const URI & _uri);

    template<>
    struct DefaultHash<URI>
    {
        Size operator()(const URI & _uri) const
        {
            //@TODO: this is slow
            //@TODO: Take allocator into account
            String str = toString(_uri);
            return detail::murmur2(str.cString(), str.length(), 0);
        }
    };
}

#endif //STICK_URI_HPP
