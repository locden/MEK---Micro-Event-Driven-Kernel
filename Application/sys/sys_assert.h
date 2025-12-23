#ifndef LTK_SYS_ASSERT_H_
#define LTK_SYS_ASSERT_H_

/*! @file
* @brief Memory-efficient Design by Contract (SYS) for embedded C and C++.
*
* @note
* The runtime checking of the SYS assertions can be disabled by defining
* the macro #SYS_DISABLE. However, it is generally **not** advisable to
* disable assertions, *especially* in the production code. Instead, the
* assertion fault handler SYS_fault_handler() should be very carefully
* designed and tested under all fault conditions.
*/

/* Active SYS macros -------------------------------------------------------*/
#ifndef SYS_DISABLE

/*! Define the user-specified module name for assertions in this file.
*
* @details
* Macro to be placed at the top of each C/C++ module to define the
* single instance of the module name string to be used in reporting
* assertions in this module. This macro takes the user-supplied parameter
* `name_`.
*
* @param[in] name_ string constant representing the module name
*
* @note
* This macro should **not** be terminated by a semicolon.
*/
#define SYS_MODULE_NAME(name_) \
    static char const SYS_module_name_[] = name_;

/*! General purpose assertion with user-specified ID number.
*
* @details
* Makes sure the `expr_` parameter is TRUE. Calls the SYS_fault_handler()
* callback if the `expr_` evaluates to FALSE. This assertion takes the
* user-supplied parameter `label_` to identify the location of this
* assertion within the module. This avoids the volatility of using line
* numbers, which change whenever a line of code is added or removed
* upstream from the assertion.
*
* @param[in] label_ numeric label of the assertion (unique within the module)
* @param[in] expr_  Boolean expression to check
*
* @note
* The `expr_` expression is **not** evaluated if assertions are
* disabled with the ::SYS_DISABLE switch.
*/
#define SYS_ASSERT(label_, expr_) ((expr_) \
    ? ((void)0) : SYS_fault_handler(&SYS_module_name_[0], (label_)))

/*! General purpose assertion with user-specified ID number that
* evaluates the `expr_` expression even when assertions are disabled.
*
* @details
* Like the SYS_ASSERT() macro, except it **always** evaluates the
* `expr_` expression even when SYS assertions are disabled with the
* #SYS_DISABLE macro.
*
* @param[in] label_ numeric label of the assertion (unique within the module)
* @param[in] expr_  Boolean expression to check
*/
#define SYS_ALLEGE(label_, expr_) SYS_ASSERT(label_, expr_)

/*! Assertion for a wrong path through the code
*
* @details
* Calls the SYS_fault_handler() callback if ever executed. This assertion
* takes the user-supplied parameter `id_` to identify the location of
* this assertion within the file. This avoids the volatility of using
* line numbers, which change whenever a line of code is added or removed
* upstream from the assertion.
*
* @param[in] label_ numeric label of the assertion (unique within the module)
*/
#define SYS_ERROR(label_) SYS_fault_handler(&SYS_module_name_[0], (label_))

/*! Assertion for checking preconditions.
*
* @details
* Equivalent to SYS_ASSERT(), except the name provides a better
* documentation of the intention of this assertion.
*
* @param[in] label_ numeric label of the assertion (unique within the module)
* @param[in] expr_  Boolean expression to check
*
* @note
* The `expr_` expression is **not** evaluated if assertions are
* disabled with the ::SYS_DISABLE switch.
*/
#define SYS_REQUIRE(label_, expr_)  SYS_ASSERT((label_), (expr_))

/*! Assertion for checking postconditions.
*
* @details
* Equivalent to SYS_ASSERT(), except the name provides a better
* documentation of the intention of this assertion.
*
* @param[in] label_ numeric label of the assertion (unique within the module)
* @param[in] expr_  Boolean expression to check
*
* @note
* The `expr_` expression is **not** evaluated if assertions are
* disabled with the ::SYS_DISABLE switch.
*/
#define SYS_ENSURE(label_, expr_) SYS_ASSERT((label_), (expr_))

/*! Assertion for checking invariants.
*
* @details
* Equivalent to SYS_ASSERT(), except the name provides a better
* documentation of the intention of this assertion.
*
* @param[in] label_ numeric label of the assertion (unique within the module)
* @param[in] expr_  Boolean expression to check
*
* @note
* The `expr_` expression is **not** evaluated if assertions are
* disabled with the ::SYS_DISABLE switch.
*/
#define SYS_INVARIANT(label_, expr_)  SYS_ASSERT((label_), (expr_))

#ifndef SYS_NORETURN
#define SYS_NORETURN
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! SYS assertion fault handler.
*
* @details
* This is an application-specific callback function needs to be defined in
* the application to perform the clean system shutdown and perhaps a reset.
* The SYS_fault_handler() function is the last line of defense after the
* system failure and its implementation should be very **carefully**
* designed and **tested** under various fault conditions, including but
* not limited to: stack overflow, stack corruption, or calling
* SYS_fault_handler() from ISRs.

* @param[in] module name of the file/module in which the assertion failed
*                   (constant, zero-terminated C string)
* @param[in] label  unique label of the assertion within the module.
*                   This could be a line number or a user-defined label.
*
* @returns
* This callback function should **not return** (see #NORETURN),
* as continuation after an assertion failure does not make sense.
*
* @note
* It is typically a **bad idea** to implement SYS_fault_handler() as an
* endless loop that ties up the CPU. During debugging, SYS_fault_handler()
* is an ideal place to put a breakpoint.
*/
SYS_NORETURN void SYS_fault_handler(char const * module, int label);

#ifdef __cplusplus
}
#endif

/* Inactive SYS macros -----------------------------------------------------*/
#else

#define SYS_MODULE_NAME(dummy_)
#define SYS_ASSERT(label_, expr_)      ((void)0)
#define SYS_ERROR(label_)              ((void)0)
#define SYS_REQUIRE(label_, expr_)  ((void)0)
#define SYS_ENSURE(label_, expr_) ((void)0)
#define SYS_INVARIANT(label_, expr_)  ((void)0)
#define SYS_ALLEGE(label_, expr_)      ((void)(expr_))

#endif /* Inactive SYS macros */

#endif /* LTK_SYS_ASSERT_H_ */
