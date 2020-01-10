/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <chrono>

namespace MasslessInterface {
    /// <summary>
    /// Structure for storing Massless Pen pose data
    /// </summary>
	struct Pose
	{
		Pose(float x = 0, float y = 0, float z = 0, float qr = 1, float qx = 0, float qy = 0, float qz = 0, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now()) :
			m_x(x), 
			m_y(y), 
			m_z(z), 
			m_qr(qr), 
			m_qx(qx), 
			m_qy(qy), 
			m_qz(qz),
            m_ex(0),
            m_ey(0),
            m_ez(0),
            m_gx(0),
            m_gy(0),
            m_gz(0),
            m_timestamp(timestamp)
		{}

        inline bool operator==(const Pose& other) {
            return
                this->m_x == other.m_x &&
                this->m_y == other.m_y &&
                this->m_z == other.m_z &&
                this->m_qr == other.m_qr &&
                this->m_qx == other.m_qx &&
                this->m_qy == other.m_qy &&
                this->m_qz == other.m_qz;
        }

        inline bool operator!=(const Pose& other) {
            return !((*this) == other);
        }

		/// <summary>
		/// Position of the pen in the current scale (default metres)
		/// </summary>
		float m_x, m_y, m_z;

		/// <summary>
		/// Quaternion rotation of the pen
		/// </summary>
		float m_qr, m_qx, m_qy, m_qz;

        /// <summary>
        /// Error in the position of the pen in the current scale (default metres)
        /// </summary>
        float m_ex, m_ey, m_ez;

        /// <summary>
        /// Gyroscope readings
        /// </summary>
        float m_gx, m_gy, m_gz;

        std::chrono::system_clock::time_point m_timestamp;
	};

    inline std::ostream& operator<< (std::ostream& out, const MasslessInterface::Pose pose) {
        return out << "{ x: " << pose.m_x << ", y: " << pose.m_y << ", z: " << pose.m_z << ", qr: " << pose.m_qr << ", qx: " << pose.m_qx << ", qy: " << pose.m_qy << ", qz: " << pose.m_qz << " }";
    }
};

