/*
 * Copyright 2002 - 2016, The libsigc++ Development Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifndef SIGC_TRACKABLE_HPP
#define SIGC_TRACKABLE_HPP
#include <list>
#include <sigc++config.h>

namespace sigc
{

struct notifiable;

namespace internal
{

using func_destroy_notify = void (*)(notifiable* data);

/** Destroy notification callback.
 * A destroy notification callback consists of a data pointer and a
 * function pointer. The function is executed from the owning callback
 * list (of type sigc::internal::trackable_callback_list) when its parent
 * object (of type sigc::trackable) is destroyed or overwritten.
 */
struct SIGC_API trackable_callback
{
  notifiable* data_;
  func_destroy_notify func_;
  trackable_callback(notifiable* data, func_destroy_notify func) noexcept : data_(data), func_(func)
  {
  }
};

/** Callback list.
 * A callback list holds an STL list of callbacks of type
 * trackable_callback. Callbacks are added and removed with
 * add_callback(), remove_callback() and clear(). The callbacks
 * are invoked from clear() and from the destructor.
 */
struct SIGC_API trackable_callback_list
{
  /** Add a callback function.
   * @param data Data that will be sent as a parameter to teh callback function.
   * @param func The callback function.
   *
   */
  void add_callback(notifiable* data, func_destroy_notify func);

  /** Remove the callback which has this data associated with it.
   * @param data The data that was given as a parameter to add_callback().
   */
  void remove_callback(notifiable* data);

  /** This invokes all of the callback functions.
   */
  void clear();

  trackable_callback_list() : clearing_(false) {}

  trackable_callback_list(const trackable_callback_list& src) = delete;
  trackable_callback_list& operator=(const trackable_callback_list& src) = delete;
  trackable_callback_list(trackable_callback_list&& src) = delete;
  trackable_callback_list& operator=(trackable_callback_list&& src) = delete;

  /** This invokes all of the callback functions.
   */
  ~trackable_callback_list();

private:
  using callback_list = std::list<trackable_callback>;
  callback_list callbacks_;
  bool clearing_;
};

} /* namespace internal */

struct SIGC_API notifiable
{
  using func_destroy_notify = internal::func_destroy_notify;
};

/** Base class for objects with auto-disconnection.
 * trackable must be inherited when objects shall automatically
 * invalidate slots referring to them on destruction.
 * A slot built from a member function of a trackable derived
 * type installs a callback that is invoked when the trackable object
 * is destroyed or overwritten.
 *
 * add_destroy_notify_callback() and remove_destroy_notify_callback()
 * can be used to manually install and remove callbacks when
 * notification of the object dying is needed.
 *
 * notify_callbacks() invokes and removes all previously installed
 * callbacks and can therefore be used to disconnect from all signals.
 *
 * Note that there is no virtual destructor. Don't use @p trackable*
 * as pointer type for managing your data or the destructors of
 * your derived types won't be called when deleting your objects.
 *
 * @ingroup signal
 */
struct SIGC_API trackable : public notifiable
{
  // Concerning noexcept specifications:
  // libsigc++ does not have complete control of what happens when notify_callbacks()
  // is called. It may throw an exception. A method that calls notify_callbacks()
  // shall not be declared noexcept.

  trackable() noexcept;

  trackable(const trackable& src) noexcept;

  trackable(trackable&& src) noexcept;

  trackable& operator=(const trackable& src);

  trackable& operator=(trackable&& src) noexcept;

  ~trackable();

  /*virtual ~trackable() {} */ /* we would need a virtual dtor for users
                                  who insist on using "trackable*" as
                                  pointer type for their own derived objects */

  using func_destroy_notify = internal::func_destroy_notify;

  /** Add a callback that is executed (notified) when the trackable object is detroyed.
   * @param data Passed into func upon notification.
   * @param func Callback executed upon destruction of the object.
   */
  void add_destroy_notify_callback(notifiable* data, func_destroy_notify func) const;

  /** Remove a callback previously installed with add_destroy_notify_callback().
   * The callback is not executed.
   * @param data Parameter passed into previous call to add_destroy_notify_callback().
   */
  void remove_destroy_notify_callback(notifiable* data) const;

  /// Execute and remove all previously installed callbacks.
  void notify_callbacks();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
  /* The callbacks are held in a list of type trackable_callback_list.
   * This list is allocated dynamically when the first callback is added.
   */
  internal::trackable_callback_list* callback_list() const;
  mutable internal::trackable_callback_list* callback_list_;
#endif
};

} /* namespace sigc */

#endif /* SIGC_TRACKABLE_HPP */
