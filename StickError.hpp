#ifndef STICK_STICKERROR_HPP
#define STICK_STICKERROR_HPP

#include <Stick/StickString.hpp>

namespace stick
{
    class ErrorCategory;

    /**
     * @brief An Error describes an error condition: http://en.wikipedia.org/wiki/Error_code
     *
     * This implementation holds an integer code that describes the error condition and a pointer to a
     * ErrorCategory which is the group of errors that the Error is part of.
     */
     class Error
     {
     public:

        /**
         * @brief Default Constructor.
         *
         * This will initialize the error category to NoErrorCategory and the code to 0,
         * which is the default no error indicator.
         */
         Error();

        /**
         * @brief Constructs an Error from a code and a category.
         *
         * You rarely use this constructor directly. Usually you would just provide the error code enum value,
         * and let the magic behind the scenes deduce the ErrorCategory automatically.
         *
         * @see detail::isErrorEnum
         */
         Error(Int32 _code, const ErrorCategory & _category);

        /**
         * @brief Constructs an Error from an ErrorEnum value.
         *
         * The error category is automatically deducted at compile time.
         */
        //template<class ErrorEnum>
        //Error(ErrorEnum _code, typename std::enable_if<detail::isErrorEnum<ErrorEnum>::value>::type* = 0);

        /**
         * @brief Assigns an ErrorEnum value to this.
         *
         * The error category is automatically deducted at compile time.
         */
        /*template<class ErrorEnum>
        typename std::enable_if<detail::isErrorEnum<ErrorEnum>::value, Error>::type &
        operator = (ErrorEnum _code);*/
        
        /**
         * @brief Implicit conversion to bool.
         */
         explicit operator bool() const;

        /**
         * @brief Implicit conversion to bool.
         */
         bool operator!() const;

        /**
         * @brief Returns true if the codes and categories of both Errors are equal.
         */
         bool operator == (const Error & _other) const;

        /**
         * @brief Returns true if either the codes and/or categories of the Errors are not equal.
         */
         bool operator != (const Error & _other) const;

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
         * @see ErrorCategory::resolveError
         */
         Error resolvedCode() const;

        /**
         * @brief Returns the ErrorCategory that this Error belongs to.
         */
         const ErrorCategory & category() const;


     protected:

        Int32 m_code;
        const ErrorCategory * m_category;
    };
}

#endif //STICK_STICKERROR_HPP
