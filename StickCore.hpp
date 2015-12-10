#ifndef STICK_CORE_HPP
#define STICK_CORE_HPP

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Stick/StickPlatform.hpp>

#include <iostream>

namespace stick
{
    namespace core
    {

        template<class T>
        struct RemoveReference
        {
            typedef T Type;
        };

        template<class T>
        struct RemoveReference<T&>  
        {
            typedef T Type;
        };

        template<class T>
        struct RemoveReference<T&&> 
        {
            typedef T Type;
        };

        template<class T>
        inline typename RemoveReference<T>::Type && move(T && _arg)
        {
            return static_cast<typename RemoveReference<T>::Type&&>(_arg);
        }

        template<class T>
        inline T min(const T & _a, const T & _b)
        {
            return _a < _b ? _a : _b;
        }

        template<class T>
        inline T max(const T & _a, const T & _b)
        {
            return _a > _b ? _a : _b;
        }

        struct Block
        {
            void * ptr;
            Size byteCount;
        };

        struct Allocator
        {
            virtual Block allocate(Size _byteCount) = 0;

            virtual Block reallocate(const Block & _block, Size _byteCount) = 0;

            virtual void deallocate(const Block & _block) = 0;
        };

        struct Mallocator : public Allocator
        {
            inline Block allocate(Size _byteCount) override
            {
                return {malloc(_byteCount), _byteCount};
            }

            inline Block reallocate(const Block & _block, Size _byteCount) override
            {
                return {realloc(_block.ptr, _byteCount), _byteCount};
            }

            inline void deallocate(const Block & _block) override
            {
                free(_block.ptr);
            }
        };

        inline Allocator & defaultAllocator()
        {
            static Mallocator s_mallocator;
            return s_mallocator;
        }

        template<class T>
        struct IteratorTraits;

        template<class T>
        struct IteratorTraits<T*>
        {
            typedef T ValueType;
            typedef T & ReferenceType;
            typedef T * PointerType;
        };

        template<class T>
        struct IteratorTraits<const T*>
        {
            typedef const T ValueType;
            typedef const T & ReferenceType;
            typedef const T * PointerType;
        };

        template<class T>
        struct ReverseIterator
        {
            typedef IteratorTraits<T> IterTraits;

            typedef T Iter;

            typedef typename IterTraits::ValueType ValueType;

            typedef typename IterTraits::ReferenceType ReferenceType;

            typedef typename IterTraits::PointerType PointerType;


            explicit ReverseIterator(const Iter & _it) :
            m_it(_it)
            {

            }

            inline ReverseIterator & operator--() 
            {
                m_it++;
                return *this;
            } 

            inline ReverseIterator & operator--(int) 
            {
                m_it++;
                return *this;
            }

            inline ReverseIterator & operator-=(Size _i) 
            {
                m_it += _i;
                return *this;
            }

            inline ReverseIterator & operator-(Size _i) 
            {
                m_it += _i;
                return *this;
            }

            inline ReverseIterator & operator++() 
            {
                m_it--;
                return *this;
            } 

            inline ReverseIterator & operator++(int) 
            {
                m_it--;
                return *this;
            }

            inline ReverseIterator & operator+=(Size _i) 
            {
                m_it -= _i;
                return *this;
            }

            inline ReferenceType operator * () const
            {
                return *m_it;
            }

            inline PointerType operator -> () const
            {
                return m_it;
            }

            inline bool operator == (const ReverseIterator & _other) const
            {
                return m_it == _other.m_it;
            }

            inline bool operator != (const ReverseIterator & _other) const
            {
                return m_it != _other.m_it;
            }

            Iter m_it;
        };

        class String
        {
        public:

            typedef char * Iter;

            typedef const char * ConstIter;

            typedef ReverseIterator<Iter> ReverseIter;

            typedef ReverseIterator<ConstIter> ReverseConstIter;


            String() :
            m_cStr(nullptr),
            m_length(0),
            m_allocator(&defaultAllocator())
            {

            }

            String(const char * _c, Allocator & _alloc = defaultAllocator()) :
            m_allocator(&_alloc)
            {
                m_length = strlen(_c);
                m_cStr = (char*)m_allocator->allocate(m_length + 1).ptr;
                strcpy(m_cStr, _c);
            }

            String(const String & _other) :
            m_length(_other.m_length),
            m_allocator(_other.m_allocator)
            {
                if(_other.m_cStr)
                {
                    m_cStr = (char*)m_allocator->allocate(m_length + 1).ptr;
                    strcpy(m_cStr, _other.m_cStr);
                }
            }

            String(String && _other) :
            m_cStr(move(_other.m_cStr)),
            m_length(move(_other.m_length)),
            m_allocator(move(_other.m_allocator))
            {

            }

            ~String()
            {
                if(m_cStr)
                {
                    STICK_ASSERT(m_allocator);
                    m_allocator->deallocate({m_cStr, m_length});
                }
            }

            inline String & operator = (const String & _other)
            {
                m_length = _other.m_length;
                m_allocator = _other.m_allocator;
                if(_other.m_cStr)
                {
                    m_cStr = (char*)m_allocator->allocate(_other.m_length + 1).ptr;
                    strcpy(m_cStr, _other.m_cStr);
                }

                return *this;
            }

            inline String & operator = (String && _other)
            {
                m_cStr = move(_other.m_cStr);
                m_allocator = move(_other.m_allocator);
                m_length = move(_other.m_length);

                return *this;
            }

            inline String & operator = (const char * _other)
            {   
                if(!m_allocator)
                    m_allocator = &defaultAllocator();

                m_length = strlen(_other);
                m_cStr = (char*)m_allocator->allocate(m_length + 1).ptr;
                strcpy(m_cStr, _other);

                return *this;
            }

            inline char operator [](Size _index) const
            {
                return *(begin() + _index);
            }

            inline char & operator [](Size _index)
            {
                return *(begin() + _index);
            }

            inline bool operator == (const String & _b) const
            {
                return strcmp(m_cStr, _b.m_cStr) == 0;
            }

            inline bool operator != (const String & _b) const
            {
                return !(*this == _b);
            }

            inline bool operator == (const char * _str) const
            {
                return strcmp(m_cStr, _str) == 0;
            }

            inline bool operator != (const char * _str) const
            {
                return !(*this == _str);
            }

            inline Size length() const
            {
                return m_length;
            }

            inline Iter begin()
            {
                return m_cStr;
            }

            inline ConstIter begin() const
            {
                return m_cStr;
            }

            inline Iter end()
            {
                return m_cStr + m_length;
            }

            inline ConstIter end() const
            {
                return m_cStr + m_length;
            }

            inline ReverseIter rbegin()
            {
                return ReverseIter(end() - 1);
            }

            inline ReverseConstIter rbegin() const
            {
                return ReverseConstIter(end() - 1);
            }

            inline ReverseIter rend()
            {
                return ReverseIter(begin());
            }

            inline ReverseConstIter rend() const
            {
                return ReverseConstIter(begin());
            }

        private:

            char * m_cStr;
            Size m_length;
            Allocator * m_allocator;
        };

        template<class T>
        class DynamicArray
        {
        public:

            typedef T ValueType;

            typedef T * Iter;

            typedef const T * ConstIter;

            typedef ReverseIterator<Iter> ReverseIter;

            typedef ReverseIterator<ConstIter> ReverseConstIter;


            DynamicArray(Allocator & _alloc = defaultAllocator()) :
            m_data({nullptr, 0}),
            m_elementCount(0),
            m_allocator(&_alloc)
            {

            }

            DynamicArray(Size _size, Allocator & _alloc = defaultAllocator()) :
            m_elementCount(_size),
            m_allocator(&_alloc)
            {
                m_data = (T*)m_allocator->allocate(m_elementCount * sizeof(T));
            }

            DynamicArray(const DynamicArray & _other) :
            m_elementCount(_other.m_elementCount),
            m_allocator(_other.m_allocator)
            {
                if(m_elementCount)
                {
                    resize(m_elementCount);
                    for(Size i = 0; i < m_elementCount; ++i)
                    {
                        *this[i] = _other[i];
                    }
                }
            }

            DynamicArray(DynamicArray && _other) :
            m_data(move(_other.m_data)),
            m_elementCount(move(_other.m_elementCount)),
            m_allocator(move(_other.m_allocator))
            {
            }

            inline DynamicArray & operator = (const DynamicArray & _other)
            {
                m_elementCount = _other.m_elementCount;
                m_allocator = _other.m_allocator;
                resize(m_elementCount);
                for(Size i = 0; i < m_elementCount; ++i)
                {
                    *this[i] = _other[i];
                }
                return *this;
            }

            inline DynamicArray & operator = (DynamicArray && _other)
            {
                m_data = move(_other.m_data);
                m_allocator = move(_other.m_allocator);
                m_elementCount = move(_other.m_elementCount);

                return *this;
            }

            inline void resize(Size _s)
            {
                reserve(_s);
                m_elementCount = _s;
            }

            inline void reserve(Size _s)
            {
                if(_s > capacity())
                {
                    auto blk = m_allocator->allocate(_s * sizeof(T));
                    if(blk.ptr != m_data.ptr)
                    {
                        for(Size i = 0; i < m_elementCount; ++i)
                        {
                            reinterpret_cast<T*>(blk.ptr)[i] = reinterpret_cast<T*>(m_data.ptr)[i];
                        }
                        m_allocator->deallocate(m_data);
                    }
                    m_data = blk;
                    //TODO: do this for POD types?
                    //m_data = m_allocator->reallocate(m_data, _s * sizeof(T));
                }
            }

            inline void append(const T & _element)
            {
                if(capacity() <= m_elementCount)
                {
                    reserve(max((Size)1, m_elementCount * 2));
                }
                (*this)[m_elementCount++] = _element;
            }

            template<class InputIter>
            inline Iter insert(ConstIter _it, InputIter _first, InputIter _last)
            {
                Size idiff = _last - _first;
                Size index = (_it - begin());
                Size diff = m_elementCount - index;
                
                if(capacity() < m_elementCount + idiff)
                {
                    reserve(max(idiff, m_elementCount * 2));
                }
                
                Size fidx = index + diff - 1;
                Size iidx = fidx + idiff; 
                for(Size i = 0; i < diff; ++i)
                {
                    (*this)[iidx - i] = (*this)[fidx - i];
                }

                for(Size i = 0; _first != _last; ++_first, ++i)
                {
                    (*this)[index + i] = *_first;
                }

                m_elementCount += idiff;
                return begin() + index;
            }

            inline Iter insert(ConstIter _it, const T & _val)
            {
                return insert(_it, &_val, &_val + 1);
            }

            inline Iter erase(ConstIter _first, ConstIter _last)
            {
                Size diff = end() - _last;
                Size idiff = _last - _first;
                Size index = (_first - begin());
                Size endIndex = m_elementCount - diff;

                for(Size i=0; i < diff; ++i)
                {
                    (*this)[index + i] = (*this)[endIndex + i];
                }
                m_elementCount -= idiff;
                return begin() + index;
            }

            inline void popBack()
            {
                (reinterpret_cast<T*>(m_data.ptr)[m_elementCount - 1]).~T();
                m_elementCount--;
            }

            inline void clear()
            {
                for(auto & el : *this)
                {
                    el.~T();
                }
                m_elementCount = 0;
            }

            inline const T & operator [](Size _index) const
            {
                return reinterpret_cast<T*>(m_data.ptr)[_index];
            }

            inline T & operator [](Size _index)
            {
                return reinterpret_cast<T*>(m_data.ptr)[_index];
            }

            inline Iter begin()
            {
                return (Iter)m_data.ptr;
            }

            inline ConstIter begin() const
            {
                return (ConstIter)m_data.ptr;
            }

            inline Iter end()
            {
                return (Iter)m_data.ptr + m_elementCount;
            }

            inline ConstIter end() const
            {
                return (ConstIter)m_data.ptr + m_elementCount;
            }

            inline ReverseIter rbegin()
            {
                return ReverseIter(end() - 1);
            }

            inline ReverseConstIter rbegin() const
            {
                return ReverseConstIter(end() - 1);
            }

            inline ReverseIter rend()
            {
                return ReverseIter(begin());
            }

            inline ReverseConstIter rend() const
            {
                return ReverseConstIter(begin());
            }

            inline Size elementCount() const
            {
                return m_elementCount;
            }

            inline Size byteCount() const
            {
                return m_elementCount * sizeof(T);
            }

            inline const T * ptr() const
            {
                return (const T*)m_data.ptr;
            }

            inline Size capacity() const
            {
                return m_data.byteCount / sizeof(T);
            }

            inline T & front()
            {
                return (*this)[0];
            }

            inline const T & front() const
            {
                return (*this)[0];
            }

            inline T & back()
            {
                return (*this)[m_elementCount-1];
            }

            inline const T & back() const
            {
                return (*this)[m_elementCount-1];
            }

        private:

            Block m_data;
            Size m_elementCount;
            Allocator * m_allocator;
        };

        template<class A, class B>
        struct KeyValuePair
        {
            typedef A KeyType;
            typedef B ValueType;

            KeyType key;
            ValueType value;
        };

        template<class A, class B>
        struct Pair
        {
            typedef A FirstType;
            typedef B SecondType;

            FirstType first;
            SecondType second;
        };

        /*
        template<class K, class V>
        class Dict
        {
        public:

            typedef K KeyType;

            typedef V ValueType;

            typedef KeyValuePair<K, V> KeyValueType;

            typedef DynamicArray<KeyValueType> ArrayType;

            typedef typename ArrayType::Iter Iter;

            typedef typename ArrayType::ConstIter ConstIter;


            Dict(Allocator & _allocator = defaultAllocator()) :
            m_array(_allocator)
            {

            }

            inline Pair<Iter, bool> insert(const ValueType & _val)
            {
                bool bFound = true;
                auto it == lowerBound(_val);
                m_array.pushBack(_val);
            }

            Iter lowerBound(const KeyType & _key)
            {

            }

        private:

            ArrayType m_array;
        };*/

        class ErrorCategory;

        /**
         * @brief An ErrorCode describes an error condition: http://en.wikipedia.org/wiki/Error_code
         *
         * This implementation holds an integer code that describes the error condition and a pointer to a
         * ErrorCategory which is the group of errors that the ErrorCode is part of.
         */
        class ErrorCode
        {
        public:
            
            /**
             * @brief Default Constructor.
             *
             * This will initialize the error category to NoErrorCategory and the code to 0,
             * which is the default no error indicator.
             */
            ErrorCode();
            
            /**
             * @brief Constructs an ErrorCode from a code and a category.
             *
             * You rarely use this constructor directly. Usually you would just provide the error code enum value,
             * and let the magic behind the scenes deduce the ErrorCategory automatically.
             *
             * @see detail::isErrorCodeEnum
             */
            ErrorCode(Int32 _code, const ErrorCategory & _category);
            
            /**
             * @brief Constructs an ErrorCode from an ErrorCodeEnum value.
             *
             * The error category is automatically deducted at compile time.
             */
            //template<class ErrorCodeEnum>
            //ErrorCode(ErrorCodeEnum _code, typename std::enable_if<detail::isErrorCodeEnum<ErrorCodeEnum>::value>::type* = 0);
            
            /**
             * @brief Assigns an ErrorCodeEnum value to this.
             *
             * The error category is automatically deducted at compile time.
             */
            /*template<class ErrorCodeEnum>
            typename std::enable_if<detail::isErrorCodeEnum<ErrorCodeEnum>::value, ErrorCode>::type &
            operator = (ErrorCodeEnum _code);*/
            
            /**
             * @brief Implicit conversion to bool.
             */
            explicit operator bool() const;
            
            /**
             * @brief Implicit conversion to bool.
             */
            bool operator!() const;
            
            /**
             * @brief Returns true if the codes and categories of both ErrorCodes are equal.
             */
            bool operator == (const ErrorCode & _other) const;
            
            /**
             * @brief Returns true if either the codes and/or categories of the ErrorCodes are not equal.
             */
            bool operator != (const ErrorCode & _other) const;
            
            /**
             * @brief Returns a String describing the error condition of this code.
             */
            String description() const;
            
            /**
             * @brief Returns the integer representation of the error condition.
             */
            Int32 code() const;
            
            /**
             * @brief Returns the resolved, platform independent error code.
             * @see ErrorCategory::resolveErrorCode
             */
            ErrorCode resolvedCode() const;
            
            /**
             * @brief Returns the ErrorCategory that this ErrorCode belongs to.
             */
            const ErrorCategory & category() const;
            
            
        protected:
            
            Int32 m_code;
            const ErrorCategory * m_category;
        };


        /**
         * @brief An ErrorCategory allows you to easily group error codes.
         *
         * This is essentially very similar to how std::error_code / std::error_condition (or the boost counterparts) works.
         * Some additional functionality for debugging and exception emitting has been added in comparison to those implementations.
         * Also, this implementation does not differ between error_code and error_condition. It only has one ErrorCode class,
         * that holds the error code in question (which usually is the platform specific error), which then can be resolved in
         * order to get a plarform independent error code.
         *
         * @see ErrorCode
         * @see ErrorReport
         */
        class ErrorCategory
        {
        public:
            
            /**
             * @brief Constructs an ErrorCategory with a name.
             *
             * Usually, error categories are lazy constructed statically.
             *
             * @see errorCategory
             */
            ErrorCategory(const String & _name);

            /**
             * @brief Virtual Destructor. You derive from this class to create a new ErrorCategory.
             */
            virtual ~ErrorCategory();
            
            /**
             * @brief Returns true if the other ErrorCategory is this.
             */
            bool operator == (const ErrorCategory & _other) const;
            
            /**
             * @brief Returns true if the other ErrorCategory is not this.
             */
            bool operator != (const ErrorCategory & _other) const;
            
            /**
             * @brief Returns the name of this category.
             */
            const String & name() const;
            
            /**
             * @brief Resolve can potentially change an error to something more meaningful/portable inside of this category.
             *
             * For example, _code might hold a platform specific error, that is changed to a platform independent error inside
             * of the resolveErrorCode function of an ErrorCategory. The default implementation just returns the input ErrorCode.
             *
             * @param _code The code to resolve.
             */
            virtual ErrorCode resolveErrorCode(const ErrorCode & _code) const;
            
            /**
             * @brief This needs to be overwritten to return descriptions for the error code in this category.
             * @param _code The code to get the description for.
             */
            virtual String description(const ErrorCode & _code) const = 0;

            /**
             * @brief This allows you to specify conditions that don't necessarily indicate an error. (i.e. status codes)
             */
            virtual bool indicatesError(const ErrorCode & _code) const;
            
            
        private:
            
            String m_name;
        };

        /**
         * @brief An ErrorReport adds some more meta information about an error condition on top of ErrorCode.
         *
         * In addition to the error code, it can hold a message that gives some more contextual information about
         * where and how the error occured. It can also store a file name and line number for debugging purposes
         * to guide you to the piece of code that emitted the error. Additionally, an ErrorReport knows how and when
         * to throw an exception that is appropriate for its condition.
         */
        class ErrorReport :
        public ErrorCode
        {
        public:
            
            /**
             * @brief Default Constructor.
             *
             * See the default constructor of ErrorCode.
             */
            ErrorReport();
            
            /**
             * @brief Constructs an ErrorReport from an ErrorCode and contextual information.
             * @param _code The ErrorCode that describes the error.
             * @param _message A contextual message that describes when and how the error occured.
             * @param _file You can optionally provide which file emitted the error for debugging.
             * @param _line You can optionally provide the line number that emitted the error for debugging.
             */
            ErrorReport(const ErrorCode & _code, const String & _message = "", const String & _file = "", UInt32 _line = 0);
            
            /**
             * @brief Returns the error message (contextual info) of this error.
             */
            const String & message() const;
            
            /**
             * @brief Returns the contextual message of this error with the general
             * error description.
             * @see message
             */
            const String & messageAndDescription() const;
            
            /**
             * @brief Returns the file where the error occured (if it was provided).
             */
            const String & file() const;
            
            /**
             * @brief Returns the line number where the error occured (if it was provided).
             * @see file
             */
            UInt32 line() const;
            
            /**
             * @brief Throws the exception for this error. This will throw no matter what!
             */
            void throwException() const;
            
            /**
             * @brief Throws the exception for this error if the condition indicates an error.
             */
            void throwOnError() const;
            
        private:
            
            mutable String m_messageAndDescription;
            String m_message;
            String m_file;
            UInt32 m_line;
        };




        /*
        namespace detail
        {
            inline Size strLen(const String & _str)
            {
                return _str.length;
            }

            inline Size strLen(const char * _str)
            {
                return strlen(_str);
            }

            inline int performCopy(String & _dest, Size & _off, const String & _src)
            {
                strcpy(_dest.cStr + _off, _src.cStr);
                _off += _src.length;
                return 0;
            }

            inline int performCopy(String & _dest, Size & _off, const char * _src)
            {
                strcpy(_dest.cStr + _off, _src);
                _off += strlen(_src);
                return 0;
            }
        }

        template<class Allocator, class ... Strings>
        inline String concatStringsWithAllocator(Allocator & _allocator, Strings ... _args)
        {
            Size len = 0;
            int unpack[]{0, (len += detail::strLen(_args), 0)...};
            String ret = createString(len, _allocator);
            Size off = 0;
            int unpack2[]{0, (detail::performCopy(ret, off, _args))...};
            return ret;
        }

        template<class ... Strings>
        inline String concatStrings(const String & _a, Strings ... _args)
        {
            return concatStringsWithAllocator(defaultAllocator(), _a, _args...);
        }

        template<class ... Strings>
        inline String concatStrings(const char * _a, Strings ... _args)
        {
            return concatStringsWithAllocator(defaultAllocator(), _a, _args...);
        }

        template<class Allocator = Mallocator>
        inline String copy(const String & _str, Allocator & _allocator = defaultAllocator())
        {
            return createString(_str.cStr, _allocator);
        }
        
        template<class Allocator = Mallocator>
        inline void destroy(const String & _str, Allocator & _allocator = defaultAllocator())
        {
            deallocate(_allocator, {_str.cStr, _str.length});
        }

        void toCString(Float32 _v, char * _str, Size _len)
        {
            snprintf (_str, _len, "%f", _v);
        }

        void toCString(Float64 _v, char * _str, Size _len)
        {
            snprintf (_str, _len, "%f", _v);
        }

        char * begin(String & _str)
        {
            return _str.cStr;
        }

        char * end(String & _str)
        {
            return _str.cStr + _str.length;
        }

        namespace detail
        {
            template<class T>
            struct FmtGenerator;

            template<>
            struct FmtGenerator<Float32>
            {
                static constexpr char fmt[] = "%f ";
            };

            constexpr char FmtGenerator<Float32>::fmt[4];

            template<>
            struct FmtGenerator<Float64>
            {
                static constexpr char fmt[] = "%f ";
            };

            constexpr char FmtGenerator<Float64>::fmt[4];

            template<>
            struct FmtGenerator<const char *>
            {
                static constexpr char fmt[] = "%s ";
            };

            constexpr char FmtGenerator<const char *>::fmt[4];

            template<class ... T>
            struct AccumulatedFmt
            {
               static char * fmtStr()
               {
                    static char s_c[sizeof...(T) * 4];
                    memset(s_c, 0, sizeof(s_c));
                    int unpack[]{0, (strcat(s_c, FmtGenerator<T>::fmt), 0)...};
                    return s_c;
               }
            };
        }

        template<class ... T>
        inline void print(T ... _args)
        {
            printf(detail::AccumulatedFmt<T...>::fmtStr(), _args...);
        }*/
    }
}

#ifdef STICK_CORE_TESTS

#include <iostream>
#include <chrono>

class Adder
{
public:
    Adder() :
    num(0)
    {
    }

    void add(int _num)
    {
        num += _num;
        std::cerr<<"fock"<<std::endl;
    }

    int num;
};

class BaseAdder
{
    public:
    virtual void add(int _num) = 0;
};

class VirtualAdder : public BaseAdder
{
public:

    VirtualAdder() :
    num(0)
    {
    }

    void add(int _num) override
    {
        num += _num;
        std::cerr<<"fock"<<std::endl;
    }

    int num;
};


int main(int _argc, const char * _args[])
{
    using namespace stick::core;

    SUITE("String Tests")
    {
        /*
        String a = createString("test");
        String b = createString("test");
        String c = createString("test!!! ");
        String d = concatStrings(a, b, c, "woot");

        TEST(a == b);
        TEST(a != c);
        TEST(c != b);
        TEST(a == "test");
        TEST(b == "test");
        TEST(c == "test!!! ");
        TEST(d.length == a.length + b.length + c.length + 4);
        TEST(d == "testtesttest!!! woot");

        destroy(c);
        destroy(b);
        destroy(a);

        print(43.2f);
        print(43.2f, 100.0, "test");*/

        String a("test");
        String b("test");
        String c(String("different"));
        TEST(a == b);
        TEST(a != c);
        TEST(b != c);

        char expectedResults[] = {'t', 'e', 's', 't'};

        Int32 i = 0;
        for(auto c : a)
        {
            TEST(c == expectedResults[i]);
            i++;
        }

        i = 3;
        for(auto it = a.rbegin(); it != a.rend(); it++)
        {
            TEST(*it == expectedResults[i]);
            i--;
        }

        String d;
        d = a;
        TEST(d == a);
        TEST(d == "test");

        d = "another one";
        TEST(d == "another one");

        /*
        Adder a;
        auto start = std::chrono::system_clock::now();
        for(int i=0; i < 100; i++)
        {
            a.add(i);
        }
        std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now()-start).count()<<std::endl;
        VirtualAdder b;
        start = std::chrono::system_clock::now();
        for(int i=0; i < 100; i++)
        {
            b.add(i);
        }
        std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now()-start).count()<<std::endl;
        */
    }

    SUITE("DynamicArray Tests")
    {
        DynamicArray<Int32> a;
        TEST(a.elementCount() == 0);
        TEST(a.byteCount() == 0);

        a.resize(5);
        TEST(a.elementCount() == 5);
        TEST(a.byteCount() == 20);

        for(Int32 i = 0; i < 5; ++i)
        {
            a[i] = i;
        }

        TEST(a[0] == 0);
        TEST(a[1] == 1);
        TEST(a[2] == 2);
        TEST(a[3] == 3);
        TEST(a[4] == 4);

        a.append(10);
        TEST(a.elementCount() == 6);

        TEST(a[5] == 10);
        TEST(a.back() == 10);
        TEST(a.front() == 0);

        a.popBack();
        TEST(a.back() == 4);
        TEST(a.elementCount() == 5);

        Int32 expectedResults[] = {0, 1, 2, 3, 4};
        Int32 i = 0;
        for(auto e : a)
        {
            TEST(e == expectedResults[i]);
            ++i;
        }

        i = a.elementCount() - 1;
        for(auto it = a.rbegin(); it != a.rend(); ++it)
        {
            TEST(*it == expectedResults[i]);
            --i;
        }

        a.clear();
        TEST(a.elementCount() == 0);

        
        DynamicArray<Int32> b;
        Int32 arr[] = {1, 2, 3, 4};
        b.append(10);
        b.append(11);
        b.append(12);
        b.append(13);
        auto it = b.insert(b.end(), arr, arr + 4);
        TEST(it == b.end() - 4);
        TEST(it == b.begin() + 4);
        TEST(*it == 1);
        auto it2 = b.insert(b.begin() + 2, 99);
        TEST(it2 == b.begin() + 2);
        TEST(*it2 == 99);

        Int32 expectedArr[] = {10, 11, 99, 12, 13, 1, 2, 3, 4};
        i = 0;
        for(auto v : b)
        {
            TEST(expectedArr[i] == v);
            i++;
        }
        TEST(b.elementCount() == 9);

        auto it3 = b.erase(b.begin() + 2, b.begin() + 6);
        TEST(it3 == b.begin() + 2);
        TEST(*it3 == 2);
        Int32 expectedArr2[] = {10, 11, 2, 3, 4};
        i=0;
        for(auto v : b)
        {
            TEST(expectedArr2[i] == v);
            i++;
        }
    }
}

#endif //STICK_CORE_TESTS

#endif //STICK_CORE_HPP
