/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <cstdint>
namespace MasslessInterface {
	/// <summary>
	/// Structure for storing a Massless Pen state data 
	/// </summary>
    struct PenState
	{
		PenState(float surface_proximity = 0, uint8_t capsense_value = 0) :
			m_surfaceProximity(surface_proximity),
			m_capsenseValue(capsense_value),
            m_surfaceFound(false),
            m_isTapped(false)
		{}

		/// <summary>
		/// Distance from a surface (in metres)
		/// </summary>
		float m_surfaceProximity;
		
		/// <summary>
		/// Capacitive sensor value
		/// </summary>
		uint8_t m_capsenseValue;

        /// <summary>
        /// True if the pen is considered touching a surface, false otherwise
        /// </summary>
        bool m_surfaceFound;

        /// <summary>
        /// True if the capacitive slider was just tapped
        /// </summary>
        bool m_isTapped;

        /// <summary>
        /// Value for capacitive sensor when untouched
        /// </summary>
        static constexpr uint8_t CAPSENSE_NOT_TOUCHED = 0xFF;

        /// <summary>
        /// Maximum value for a touched capacitive sensor
        /// </summary>
        static constexpr uint8_t CAPSENSE_MAX = 0xFE;

        /// <summary>
        /// Value marking the centre of the capacitive sensor
        /// </summary>
        static constexpr uint8_t CAPSENSE_CENTRE = 0x7F;

        /// <summary>
        /// Value for distance sensor when a surface is not found
        /// </summary>
        static constexpr float SURFACE_NOT_FOUND = 0;

        /// <summary>
        /// Threshold for distance sensor when a surface is "Touched"
        /// </summary>
        static constexpr float SURFACE_TOUCHED = 0.5f;
	};
}

