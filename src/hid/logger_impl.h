#pragma once
#ifndef __DSY_LOGGER_IMPL_H
#define __DSY_LOGGER_IMPL_H
#include <cassert>

#if defined(STM32H750xx)
#include "hid/usb.h"
#include "usbd_def.h"
#include "sys/system.h"
#include <unistd.h>
#elif defined(_WIN32)
#include <io.h>
#define STDOUT_FILENO _fileno(stdout)

inline void write(int __fd, const void* __buf, size_t __nbyte) 
{
    _write(__fd, __buf, (unsigned int)__nbyte);
}
#endif



namespace daisy
{
/** Enumeration of destination ports for debug logging
 */
enum LoggerDestination
{
    LOGGER_NONE,     /**< mute logging */
    LOGGER_SEMIHOST, /**< stdout */
#ifdef STM32H750xx
    LOGGER_INTERNAL, /**< internal USB port */
    LOGGER_EXTERNAL, /**< external USB port */
#else
    LOGGER_INTERNAL = LOGGER_SEMIHOST, /**< imitate internal USB port */
    LOGGER_EXTERNAL = LOGGER_NONE,     /**< disable external USB port */
#endif
};

/** @brief Logging I/O underlying implementation
 *  @author Alexander Petrov-Savchenko (axp@soft-amp.com)
 *  @date November 2020
 */
template <LoggerDestination dest>
class LoggerImpl
{
  public:
    /** Initialize logging destination
     */
    static void Init() {}

    /** Transmit a block of data
     */
    static bool Transmit(const void* buffer, size_t bytes) { return true; }
};

#ifdef STM32H750xx
/**  @brief Specialization for internal USB port
 */
template <>
class LoggerImpl<LOGGER_INTERNAL>
{
  public:
    /** Initialize logging destination
    */
    static void Init()
    {
        /** this implementation relies on the fact that UsbHandle class has no member variables and can be shared
         * assert this statement:
         */
        static_assert(1u == sizeof(usb_handle_));
        usb_handle_.Init(UsbHandle::FS_INTERNAL);
    }

    /** Transmit a block of data
     */
    static bool Transmit(const void* buffer, size_t bytes)
    {
        return USBD_OK == usb_handle_.TransmitInternal((uint8_t*)buffer, bytes);
    }

  protected:
    /** USB Handle for CDC transfers 
     */
    static UsbHandle usb_handle_;
};


/**  @brief Specialization for external USB port
 */
template <>
class LoggerImpl<LOGGER_EXTERNAL>
{
  public:
    /** Initialize logging destination
     */
    static void Init()
    {
        /** this implementation relies on the fact that UsbHandle class has no member variables and can be shared.
         * assert this statement:
         */
        static_assert(1u == sizeof(usb_handle_));
        usb_handle_.Init(UsbHandle::FS_EXTERNAL);
    }

    /** Transmit a block of data
     */
    static bool Transmit(const void* buffer, size_t bytes)
    {
        return USBD_OK == usb_handle_.TransmitExternal((uint8_t*)buffer, bytes);
    }

  protected:
    /** USB Handle for CDC transfers 
     */
    static UsbHandle usb_handle_;
};

#endif
/**  @brief Specialization for semihosting (stdout)
 */
template <>
class LoggerImpl<LOGGER_SEMIHOST>
{
  public:
    /** Initialize logging destination
     */
    static void Init() {}

    /** Transmit a block of data
     */
    static bool Transmit(const void* buffer, size_t bytes)
    {
        write(STDOUT_FILENO, buffer, bytes);
        return true;
    }
};


} /* namespace daisy */

#endif //__DSY_LOGGER_IMPL_H
