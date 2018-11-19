#ifndef STICK_ERRORCATEGORY_HPP
#define STICK_ERRORCATEGORY_HPP

#include <Stick/ErrorCodes.hpp>
#include <Stick/String.hpp>

namespace stick
{
class Error;

/**
 * @brief An ErrorCategory allows you to easily group error codes.
 *
 * This is essentially very similar to how std::error_code / std::error_condition (or the boost
 * counterparts) works. Some additional functionality for debugging and exception emitting has been
 * added in comparison to those implementations. Also, this implementation does not differ between
 * error_code and error_condition. It only has one Error class, that holds the error code in
 * question (which usually is the platform specific error), which then can be resolved in order to
 * get a plarform independent error code.
 *
 * @see Error
 */
class STICK_API ErrorCategory
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
    virtual ~ErrorCategory()
    {
    }

    /**
     * @brief Returns true if the other ErrorCategory is this.
     */
    bool operator==(const ErrorCategory & _other) const;

    /**
     * @brief Returns true if the other ErrorCategory is not this.
     */
    bool operator!=(const ErrorCategory & _other) const;

    /**
     * @brief Returns the name of this category.
     */
    const String & name() const;

    /**
     * @brief Resolve can potentially change an error to something more meaningful/portable inside
     * of this category.
     *
     * For example, _code might hold a platform specific error, that is changed to a platform
     * independent error inside of the resolveError function of an ErrorCategory. The default
     * implementation just returns the input Error.
     *
     * @param _code The code to resolve.
     */
    virtual Error resolveError(const Error & _code) const;

    /**
     * @brief This needs to be overwritten to return descriptions for the error code in this
     * category.
     * @param _code The code to get the description for.
     */
    virtual String description(const Error & _code) const = 0;

    /**
     * @brief This allows you to specify conditions that don't necessarily indicate an error. (i.e.
     * status codes)
     */
    virtual bool indicatesError(const Error & _code) const;

  private:
    String m_name;
};

/**
 * @brief This is the default error category, indicating no error.
 */
class NoErrorCategory : public ErrorCategory
{
  public:
    /**
     * @brief Default Constructor.
     */
    NoErrorCategory();

    /**
     * @brief Overwritten from ErrorCategory.
     */
    String description(const Error & _code) const;
};

/**
 * @brief The error category for the default errors.
 * @see ErrorCategory
 */
class MiscErrorCategory : public ErrorCategory
{
  public:
    MiscErrorCategory();

    String description(const Error & _code) const;
};

/**
 * @brief The error category for the platform independent system errors.
 * @see ErrorCategory
 */
class SystemErrorCategory : public ErrorCategory
{
  public:
    SystemErrorCategory();

    String description(const Error & _code) const;
};

namespace detail
{
STICK_API const NoErrorCategory & noErrorCategory();

STICK_API const SystemErrorCategory & systemErrorCategory();

STICK_API const MiscErrorCategory & miscErrorCategory();

inline const ErrorCategory & errorCategory(ec::SystemErrorCode)
{
    return detail::systemErrorCategory();
}

inline const ErrorCategory & errorCategory(ec::MiscErrorCode)
{
    return detail::miscErrorCategory();
}
} // namespace detail
} // namespace stick

#endif // STICK_ERRORCATEGORY_HPP
