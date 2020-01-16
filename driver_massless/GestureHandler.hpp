/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <vector>
#include <functional>

#include <openvr_driver.h>

#include <MasslessCallbackEvents.h>
#include <PenEvent.hpp>

/// <summary>
/// A class for handling gesture routes and actions
/// </summary>
struct GestureHandler
{
    /// <summary>
    /// Constructs a new GestureHandler with the supplied route and action to perform when that route is matched
    /// </summary>
    /// <param name="event_route">The route that should be matched to "activate" this gesture</param>
    /// <param name="event_action">The action that should be performed when the route is matched</param>
    GestureHandler(std::vector<Massless::Events::EventType> gesture_route, std::function<bool(std::vector<MasslessInterface::PenEvent>, vr::IVRDriverInput*, std::shared_ptr<MasslessManager>)> gesture_action) :
        m_gestureRoute(gesture_route), 
        m_gestureAction(gesture_action)
    {}

    /// <summary>
    /// Matches the stored event route with the events at the end of the stack (the most recent)
    /// </summary>
    /// <param name="event_stack">The event stack</param>
    /// <returns>True if all events at the end of the stack match events in the gesture route</returns>
    bool match(const std::vector<MasslessInterface::PenEvent>& event_stack) {
        if (event_stack.size() < m_gestureRoute.size())
            return false;
        std::size_t stackOffset = event_stack.size() - m_gestureRoute.size();
        for (std::size_t i = 0; i < m_gestureRoute.size(); i++) {
            auto requiredEvent = m_gestureRoute.at(i); // We need this event to appear in the stack
            auto stackEvent = event_stack.at(stackOffset + i);
            if (requiredEvent != stackEvent.m_eventType) {
                return false;
            }
        }
        return true;
    }

    /// <summary>
    /// Performs this gesture route's action
    /// </summary>
    /// <param name="events">The event stack, with events at the end of the stack in the order they are needed defined by m_gestureRoute</param>
    /// <returns>True if the event should trigger a clearing of the event stack (Is considered final), false otherwise</returns>
    bool performAction(std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> pen_manager) {
        std::size_t stackOffset = event_stack.size() - m_gestureRoute.size();
        std::vector<MasslessInterface::PenEvent> events_filtered(event_stack.begin() + stackOffset, event_stack.end());
        return m_gestureAction(event_stack, driver_input, pen_manager);
    }

    /// <summary>
    /// The action that should be performed when the route is matched
    /// </summary>
    std::function<bool(std::vector<MasslessInterface::PenEvent>,vr::IVRDriverInput*,std::shared_ptr<MasslessManager>)> m_gestureAction;

    /// <summary>
    /// The route that should be matched to "activate" this gesture
    /// </summary>
    std::vector<Massless::Events::EventType> m_gestureRoute;

};
