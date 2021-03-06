/*
 Copyright © 2015 Teemu Lindborg
 SDAGE 1st year 2nd PPP Assignment
 Initial code © by Jonathan Macey
*/

#ifndef MAT4_H
#define MAT4_H

// ---------------------------------------------------------------------------------------
/// @file Mat4.h
/// @brief 4x4 Matrix class
/// @version 1.0
/// @author Jonathan Macey, minor modifications made by Teemu Lindborg
/// @date 03/05/15
/// Revision History:
///   -
/// @todo Maybe not necessary to have this stuff as an object but just have
///       'em in a namespace
// ---------------------------------------------------------------------------------------

class Mat4
{
  public:
    // ---------------------------------------------------------------------------------------
    /// @brief Construct Mat4 as scale matrix _s
    /// @param[in] _s Scale value, defaults to 1.0
    // ---------------------------------------------------------------------------------------
    Mat4(
         const float _s = 1.0f
        );

    // ---------------------------------------------------------------------------------------
    /// @brief Mat4
    /// @param[in] _rhs
    // ---------------------------------------------------------------------------------------
    Mat4(
        const Mat4 &_rhs
        );

    // ---------------------------------------------------------------------------------------
    /// @brief *= operator, multiplies the current matrix with _rhs matrix
    /// @param[in] _rhs matrix to multiply the lhs matrix with
    // ---------------------------------------------------------------------------------------
    void operator *=(
                     const Mat4 &_rhs
                    );

    // ---------------------------------------------------------------------------------------
    /// @brief * operator, multiply to matrices together
    /// @param[in] _rhs Right hand side matrix
    /// @return Resulting matrix
    // ---------------------------------------------------------------------------------------
    Mat4 operator *(
                    const double &_rhs
                   );

    // ---------------------------------------------------------------------------------------
    /// @brief Function that normalizes the matrix
    // ---------------------------------------------------------------------------------------
    void normalize();

    // ---------------------------------------------------------------------------------------
    /// @brief Set the matrix to an identity matrix
    // ---------------------------------------------------------------------------------------
    void identity();

    // ---------------------------------------------------------------------------------------
    /// @brief Load gl modelview
    // ---------------------------------------------------------------------------------------
    void loadModelView() const;

    // ---------------------------------------------------------------------------------------
    /// @brief Load gl projection
    // ---------------------------------------------------------------------------------------
    void loadProjection() const;

    // ---------------------------------------------------------------------------------------
    /// @brief Transpose matrix, switch from col major to row major and vice versa
    /// @return Transposed matrix
    // ---------------------------------------------------------------------------------------
    const Mat4& transpose();

    union
    {
      // ---------------------------------------------------------------------------------------
      /// @brief matrix element m_m as a 4x4 array mapped by union to m_nn elements and m_openGL
      // ---------------------------------------------------------------------------------------
      float m_m[4][4];

      // ---------------------------------------------------------------------------------------
      /// @brief the matrix in m_openGL 16 Real array format useful for OpenGL fv formats
      /// mapped to m_m[][] elements and m_xx elements
      // ---------------------------------------------------------------------------------------
      float m_openGL[16];
      struct
      {
        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[0][0] and m_openGL[0]
        // ---------------------------------------------------------------------------------------
        float m_00;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[0][1] and m_openGL[1]
        // ---------------------------------------------------------------------------------------
        float m_01;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[0][2] and m_openGL[2]
        // ---------------------------------------------------------------------------------------
        float m_02;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[0][3] and m_openGL[3]
        // ---------------------------------------------------------------------------------------
        float m_03;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[1][0] and m_openGL[4]
        // ---------------------------------------------------------------------------------------
        float m_10;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[1][1] and m_openGL[5]
        // ---------------------------------------------------------------------------------------
        float m_11;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[1][2] and m_openGL[6]
        // ---------------------------------------------------------------------------------------
        float m_12;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[1][3] and m_openGL[7]
        // ---------------------------------------------------------------------------------------
        float m_13;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[2][0] and m_openGL[8]
        // ---------------------------------------------------------------------------------------
        float m_20;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[2][1] and m_openGL[9]
        // ---------------------------------------------------------------------------------------
        float m_21;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[2][2] and m_openGL[10]
        // ---------------------------------------------------------------------------------------
        float m_22;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[2][3] and m_openGL[11]
        // ---------------------------------------------------------------------------------------
        float m_23;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[3][0] and m_openGL[12]
        // ---------------------------------------------------------------------------------------
        float m_30;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[3][1] and m_openGL[13]
        // ---------------------------------------------------------------------------------------
        float m_31;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[3][2] and m_openGL[14]
        // ---------------------------------------------------------------------------------------
        float m_32;

        // ---------------------------------------------------------------------------------------
        /// @brief matrix element, maps to m[3][3] and m_openGL[15]
        // ---------------------------------------------------------------------------------------
        float m_33;
      }; // end of struct
    }; // end of union
}; // end of class

#endif // end of MAT4_H
