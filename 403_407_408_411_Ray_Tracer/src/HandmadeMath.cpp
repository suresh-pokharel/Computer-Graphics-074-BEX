#include "HandmadeMath.h"


COVERAGE(HMM_Power, 2)
float HMM_Power(float Base, int Exponent)
{
    ASSERT_COVERED(HMM_Power);

    float Result = 1.0f;
    float Mul = Exponent < 0 ? 1.f / Base : Base;
    int X = Exponent < 0 ? -Exponent : Exponent;
    while (X)
    {
        if (X & 1)
        {
            ASSERT_COVERED(HMM_Power);

            Result *= Mul;
        }

        Mul *= Mul;
        X >>= 1;
    }

    return (Result);
}

#ifndef HANDMADE_MATH__USE_SSE
COVERAGE(HMM_Transpose, 1)
hmm_mat4 HMM_Transpose(hmm_mat4 Matrix)
{
    ASSERT_COVERED(HMM_Transpose);

    hmm_mat4 Result;

    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Rows][Columns] = Matrix.Elements[Columns][Rows];
        }
    }

    return (Result);
}
#endif

#ifndef HANDMADE_MATH__USE_SSE
COVERAGE(HMM_AddMat4, 1)
hmm_mat4 HMM_AddMat4(hmm_mat4 Left, hmm_mat4 Right)
{
    ASSERT_COVERED(HMM_AddMat4);

    hmm_mat4 Result;

    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Left.Elements[Columns][Rows] + Right.Elements[Columns][Rows];
        }
    }

    return (Result);
}
#endif

#ifndef HANDMADE_MATH__USE_SSE
COVERAGE(HMM_SubtractMat4, 1)
hmm_mat4 HMM_SubtractMat4(hmm_mat4 Left, hmm_mat4 Right)
{
    ASSERT_COVERED(HMM_SubtractMat4);

    hmm_mat4 Result;

    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Left.Elements[Columns][Rows] - Right.Elements[Columns][Rows];
        }
    }

    return (Result);
}
#endif

COVERAGE(HMM_MultiplyMat4, 1)
hmm_mat4 HMM_MultiplyMat4(hmm_mat4 Left, hmm_mat4 Right)
{
    ASSERT_COVERED(HMM_MultiplyMat4);

    hmm_mat4 Result;

#ifdef HANDMADE_MATH__USE_SSE
    Result.Columns[0] = HMM_LinearCombineSSE(Right.Columns[0], Left);
    Result.Columns[1] = HMM_LinearCombineSSE(Right.Columns[1], Left);
    Result.Columns[2] = HMM_LinearCombineSSE(Right.Columns[2], Left);
    Result.Columns[3] = HMM_LinearCombineSSE(Right.Columns[3], Left);
#else
    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            float Sum = 0;
            int CurrentMatrice;
            for(CurrentMatrice = 0; CurrentMatrice < 4; ++CurrentMatrice)
            {
                Sum += Left.Elements[CurrentMatrice][Rows] * Right.Elements[Columns][CurrentMatrice];
            }

            Result.Elements[Columns][Rows] = Sum;
        }
    }
#endif

    return (Result);
}

#ifndef HANDMADE_MATH__USE_SSE
COVERAGE(HMM_MultiplyMat4f, 1)
hmm_mat4 HMM_MultiplyMat4f(hmm_mat4 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_MultiplyMat4f);

    hmm_mat4 Result;

    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Matrix.Elements[Columns][Rows] * Scalar;
        }
    }

    return (Result);
}
#endif

COVERAGE(HMM_MultiplyMat4ByVec4, 1)
hmm_vec4 HMM_MultiplyMat4ByVec4(hmm_mat4 Matrix, hmm_vec4 Vector)
{
    ASSERT_COVERED(HMM_MultiplyMat4ByVec4);

    hmm_vec4 Result;

#ifdef HANDMADE_MATH__USE_SSE
	Result.InternalElementsSSE = HMM_LinearCombineSSE(Vector.InternalElementsSSE, Matrix);
#else
    int Columns, Rows;
    for(Rows = 0; Rows < 4; ++Rows)
    {
        float Sum = 0;
        for(Columns = 0; Columns < 4; ++Columns)
        {
            Sum += Matrix.Elements[Columns][Rows] * Vector.Elements[Columns];
        }

        Result.Elements[Rows] = Sum;
    }
#endif

    return (Result);
}

#ifndef HANDMADE_MATH__USE_SSE
COVERAGE(HMM_DivideMat4f, 1);
hmm_mat4 HMM_DivideMat4f(hmm_mat4 Matrix, float Scalar)
{
    ASSERT_COVERED(HMM_DivideMat4f);

    hmm_mat4 Result;

    int Columns;
    for(Columns = 0; Columns < 4; ++Columns)
    {
        int Rows;
        for(Rows = 0; Rows < 4; ++Rows)
        {
            Result.Elements[Columns][Rows] = Matrix.Elements[Columns][Rows] / Scalar;
        }
    }

    return (Result);
}
#endif

COVERAGE(HMM_Rotate, 1)
hmm_mat4 HMM_Rotate(float Angle,hmm_vec3 Axis)
{
    ASSERT_COVERED(HMM_Rotate);

    hmm_mat4 Result = HMM_Mat4d(1.0f);

    Axis = HMM_NormalizeVec3(Axis);

    float SinTheta = HMM_SinF(HMM_ToRadians(Angle));
    float CosTheta = HMM_CosF(HMM_ToRadians(Angle));
    float CosValue = 1.0f - CosTheta;

    Result.Elements[0][0] = (Axis.X * Axis.X * CosValue) + CosTheta;
    Result.Elements[0][1] = (Axis.X * Axis.Y * CosValue) + (Axis.Z * SinTheta);
    Result.Elements[0][2] = (Axis.X * Axis.Z * CosValue) - (Axis.Y * SinTheta);

    Result.Elements[1][0] = (Axis.Y * Axis.X * CosValue) - (Axis.Z * SinTheta);
    Result.Elements[1][1] = (Axis.Y * Axis.Y * CosValue) + CosTheta;
    Result.Elements[1][2] = (Axis.Y * Axis.Z * CosValue) + (Axis.X * SinTheta);

    Result.Elements[2][0] = (Axis.Z * Axis.X * CosValue) + (Axis.Y * SinTheta);
    Result.Elements[2][1] = (Axis.Z * Axis.Y * CosValue) - (Axis.X * SinTheta);
    Result.Elements[2][2] = (Axis.Z * Axis.Z * CosValue) + CosTheta;

    return (Result);
}

COVERAGE(HMM_LookAt, 1)
hmm_mat4 HMM_LookAt(const hmm_vec3& Eye,const hmm_vec3& Center,const hmm_vec3& Up)
{
    ASSERT_COVERED(HMM_LookAt);

    hmm_mat4 Result;

    hmm_vec3 F = HMM_NormalizeVec3(HMM_SubtractVec3(Center, Eye));
    hmm_vec3 S = HMM_NormalizeVec3(HMM_Cross(F, Up));
    hmm_vec3 U = HMM_Cross(S, F);

    Result.Elements[0][0] = S.X;
    Result.Elements[0][1] = U.X;
    Result.Elements[0][2] = -F.X;
    Result.Elements[0][3] = 0.0f;

    Result.Elements[1][0] = S.Y;
    Result.Elements[1][1] = U.Y;
    Result.Elements[1][2] = -F.Y;
    Result.Elements[1][3] = 0.0f;

    Result.Elements[2][0] = S.Z;
    Result.Elements[2][1] = U.Z;
    Result.Elements[2][2] = -F.Z;
    Result.Elements[2][3] = 0.0f;

    Result.Elements[3][0] = -HMM_DotVec3(S, Eye);
    Result.Elements[3][1] = -HMM_DotVec3(U, Eye);
    Result.Elements[3][2] = HMM_DotVec3(F, Eye);
    Result.Elements[3][3] = 1.0f;

    return (Result);
}

COVERAGE(HMM_InverseQuaternion, 1)
hmm_quaternion HMM_InverseQuaternion(hmm_quaternion Left)
{
    ASSERT_COVERED(HMM_InverseQuaternion);

    hmm_quaternion Conjugate;
    hmm_quaternion Result;
    float Norm = 0;
    float NormSquared = 0;

    Conjugate.X = -Left.X;
    Conjugate.Y = -Left.Y;
    Conjugate.Z = -Left.Z;
    Conjugate.W = Left.W;

    Norm = HMM_SquareRootF(HMM_DotQuaternion(Left, Left));
    NormSquared = Norm * Norm;

    Result = HMM_DivideQuaternionF(Conjugate, NormSquared);

    return (Result);
}

COVERAGE(HMM_Slerp, 1)
hmm_quaternion HMM_Slerp(hmm_quaternion Left, float Time, hmm_quaternion Right)
{
    ASSERT_COVERED(HMM_Slerp);

    hmm_quaternion Result;
    hmm_quaternion QuaternionLeft;
    hmm_quaternion QuaternionRight;

    float Cos_Theta = HMM_DotQuaternion(Left, Right);
    float Angle = HMM_ACosF(Cos_Theta);

    float S1 = HMM_SinF((1.0f - Time) * Angle);
    float S2 = HMM_SinF(Time * Angle);
    float Is = 1.0f / HMM_SinF(Angle);

    QuaternionLeft = HMM_MultiplyQuaternionF(Left, S1);
    QuaternionRight = HMM_MultiplyQuaternionF(Right, S2);

    Result = HMM_AddQuaternion(QuaternionLeft, QuaternionRight);
    Result = HMM_MultiplyQuaternionF(Result, Is);

    return (Result);
}

COVERAGE(HMM_QuaternionToMat4, 1)
hmm_mat4 HMM_QuaternionToMat4(hmm_quaternion Left)
{
    ASSERT_COVERED(HMM_QuaternionToMat4);

    hmm_mat4 Result;

    hmm_quaternion NormalizedQuaternion = HMM_NormalizeQuaternion(Left);

    float XX, YY, ZZ,
          XY, XZ, YZ,
          WX, WY, WZ;

    XX = NormalizedQuaternion.X * NormalizedQuaternion.X;
    YY = NormalizedQuaternion.Y * NormalizedQuaternion.Y;
    ZZ = NormalizedQuaternion.Z * NormalizedQuaternion.Z;
    XY = NormalizedQuaternion.X * NormalizedQuaternion.Y;
    XZ = NormalizedQuaternion.X * NormalizedQuaternion.Z;
    YZ = NormalizedQuaternion.Y * NormalizedQuaternion.Z;
    WX = NormalizedQuaternion.W * NormalizedQuaternion.X;
    WY = NormalizedQuaternion.W * NormalizedQuaternion.Y;
    WZ = NormalizedQuaternion.W * NormalizedQuaternion.Z;

    Result.Elements[0][0] = 1.0f - 2.0f * (YY + ZZ);
    Result.Elements[0][1] = 2.0f * (XY + WZ);
    Result.Elements[0][2] = 2.0f * (XZ - WY);
    Result.Elements[0][3] = 0.0f;

    Result.Elements[1][0] = 2.0f * (XY - WZ);
    Result.Elements[1][1] = 1.0f - 2.0f * (XX + ZZ);
    Result.Elements[1][2] = 2.0f * (YZ + WX);
    Result.Elements[1][3] = 0.0f;

    Result.Elements[2][0] = 2.0f * (XZ + WY);
    Result.Elements[2][1] = 2.0f * (YZ - WX);
    Result.Elements[2][2] = 1.0f - 2.0f * (XX + YY);
    Result.Elements[2][3] = 0.0f;

    Result.Elements[3][0] = 0.0f;
    Result.Elements[3][1] = 0.0f;
    Result.Elements[3][2] = 0.0f;
    Result.Elements[3][3] = 1.0f;

    return (Result);
}

// This method taken from Mike Day at Insomniac Games.
// https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
//
// Note that as mentioned at the top of the paper, the paper assumes the matrix
// would be *post*-multiplied to a vector to rotate it, meaning the matrix is
// the transpose of what we're dealing with. But, because our matrices are
// stored in column-major order, the indices *appear* to match the paper.
//
// For example, m12 in the paper is row 1, column 2. We need to transpose it to
// row 2, column 1. But, because the column comes first when referencing
// elements, it looks like M.Elements[1][2].
//
// Don't be confused! Or if you must be confused, at least trust this
// comment. :)
COVERAGE(HMM_Mat4ToQuaternion, 4)
hmm_quaternion HMM_Mat4ToQuaternion(hmm_mat4 M)
{
    float T;
    hmm_quaternion Q;

    if (M.Elements[2][2] < 0.0f) {
        if (M.Elements[0][0] > M.Elements[1][1]) {
            ASSERT_COVERED(HMM_Mat4ToQuaternion);

            T = 1 + M.Elements[0][0] - M.Elements[1][1] - M.Elements[2][2];
            Q = HMM_Quaternion(
                T,
                M.Elements[0][1] + M.Elements[1][0],
                M.Elements[2][0] + M.Elements[0][2],
                M.Elements[1][2] - M.Elements[2][1]
            );
        } else {
            ASSERT_COVERED(HMM_Mat4ToQuaternion);

            T = 1 - M.Elements[0][0] + M.Elements[1][1] - M.Elements[2][2];
            Q = HMM_Quaternion(
                M.Elements[0][1] + M.Elements[1][0],
                T,
                M.Elements[1][2] + M.Elements[2][1],
                M.Elements[2][0] - M.Elements[0][2]
            );
        }
    } else {
        if (M.Elements[0][0] < -M.Elements[1][1]) {
            ASSERT_COVERED(HMM_Mat4ToQuaternion);

            T = 1 - M.Elements[0][0] - M.Elements[1][1] + M.Elements[2][2];
            Q = HMM_Quaternion(
                M.Elements[2][0] + M.Elements[0][2],
                M.Elements[1][2] + M.Elements[2][1],
                T,
                M.Elements[0][1] - M.Elements[1][0]
            );
        } else {
            ASSERT_COVERED(HMM_Mat4ToQuaternion);

            T = 1 + M.Elements[0][0] + M.Elements[1][1] + M.Elements[2][2];
            Q = HMM_Quaternion(
                M.Elements[1][2] - M.Elements[2][1],
                M.Elements[2][0] - M.Elements[0][2],
                M.Elements[0][1] - M.Elements[1][0],
                T
            );
        }
    }

    Q = HMM_MultiplyQuaternionF(Q, 0.5f / HMM_SquareRootF(T));

    return Q;
}

COVERAGE(HMM_QuaternionFromAxisAngle, 1)
hmm_quaternion HMM_QuaternionFromAxisAngle(const hmm_vec3& Axis, float AngleOfRotation)
{
    ASSERT_COVERED(HMM_QuaternionFromAxisAngle);

    hmm_quaternion Result;

    hmm_vec3 AxisNormalized = HMM_NormalizeVec3(Axis);
    float SineOfRotation = HMM_SinF(AngleOfRotation / 2.0f);

    Result.XYZ = HMM_MultiplyVec3f(AxisNormalized, SineOfRotation);
    Result.W = HMM_CosF(AngleOfRotation / 2.0f);

    return (Result);
}



// Matrix inversion code taken from
// https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html



#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)



// for column major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A2 |
//                                              | A1  A3 |
// 2x2 column major Matrix multiply A*B
inline __m128 Mat2Mul(__m128 vec1, __m128 vec2)
{
	return
		_mm_add_ps(_mm_mul_ps(                     vec1, VecSwizzle(vec2, 0,0,3,3)),
		           _mm_mul_ps(VecSwizzle(vec1, 2,3,0,1), VecSwizzle(vec2, 1,1,2,2)));
}
// 2x2 column major Matrix adjugate multiply (A#)*B
__m128 Mat2AdjMul(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3,0,3,0), vec2),
		           _mm_mul_ps(VecSwizzle(vec1, 2,1,2,1), VecSwizzle(vec2, 1,0,3,2)));

}
// 2x2 column major Matrix multiply adjugate A*(B#)
__m128 Mat2MulAdj(__m128 vec1, __m128 vec2)
{
	return
		_mm_sub_ps(_mm_mul_ps(                     vec1, VecSwizzle(vec2, 3,3,0,0)),
		           _mm_mul_ps(VecSwizzle(vec1, 2,3,0,1), VecSwizzle(vec2, 1,1,2,2)));
}

// Inverse function is the same no matter column major or row major
// this version treats it as column major
inline hmm_mat4 GetInverse(const hmm_mat4 & inM)
{
	// use block matrix method
	// A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace

	// sub matrices
	__m128 A = VecShuffle_0101(inM.Columns[0], inM.Columns[1]);
	__m128 C = VecShuffle_2323(inM.Columns[0], inM.Columns[1]);
	__m128 B = VecShuffle_0101(inM.Columns[2], inM.Columns[3]);
	__m128 D = VecShuffle_2323(inM.Columns[2], inM.Columns[3]);

#if 0
	__m128 detA = _mm_set1_ps(inM.m[0][0] * inM.m[1][1] - inM.m[0][1] * inM.m[1][0]);
	__m128 detC = _mm_set1_ps(inM.m[0][2] * inM.m[1][3] - inM.m[0][3] * inM.m[1][2]);
	__m128 detB = _mm_set1_ps(inM.m[2][0] * inM.m[3][1] - inM.m[2][1] * inM.m[3][0]);
	__m128 detD = _mm_set1_ps(inM.m[2][2] * inM.m[3][3] - inM.m[2][3] * inM.m[3][2]);
#else
	// determinant as (|A| |C| |B| |D|)
	__m128 detSub = _mm_sub_ps(
		_mm_mul_ps(VecShuffle(inM.Columns[0], inM.Columns[2], 0,2,0,2), VecShuffle(inM.Columns[1], inM.Columns[3], 1,3,1,3)),
		_mm_mul_ps(VecShuffle(inM.Columns[0], inM.Columns[2], 1,3,1,3), VecShuffle(inM.Columns[1], inM.Columns[3], 0,2,0,2))
		);
	__m128 detA = VecSwizzle1(detSub, 0);
	__m128 detC = VecSwizzle1(detSub, 1);
	__m128 detB = VecSwizzle1(detSub, 2);
	__m128 detD = VecSwizzle1(detSub, 3);
#endif

	// let iM = 1/|M| * | X  Y |
	//                  | Z  W |

	// D#C
	__m128 D_C = Mat2AdjMul(D, C);
	// A#B
	__m128 A_B = Mat2AdjMul(A, B);
	// X# = |D|A - B(D#C)
	__m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
	// W# = |A|D - C(A#B)
	__m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

	// |M| = |A|*|D| + ... (continue later)
	__m128 detM = _mm_mul_ps(detA, detD);

	// Y# = |B|C - D(A#B)#
	__m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
	// Z# = |C|B - A(D#C)#
	__m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

	// |M| = |A|*|D| + |B|*|C| ... (continue later)
	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	// tr((A#B)(D#C))
	__m128 tr = _mm_mul_ps(A_B, VecSwizzle(D_C, 0,2,1,3));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);
	// |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C))
	detM = _mm_sub_ps(detM, tr);

	const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
	// (1/|M|, -1/|M|, -1/|M|, 1/|M|)
	__m128 rDetM = _mm_div_ps(adjSignMask, detM);

	X_ = _mm_mul_ps(X_, rDetM);
	Y_ = _mm_mul_ps(Y_, rDetM);
	Z_ = _mm_mul_ps(Z_, rDetM);
	W_ = _mm_mul_ps(W_, rDetM);

	hmm_mat4 r;

	// apply adjugate and store, here we combine adjugate shuffle and store shuffle
	r.Columns[0]= VecShuffle(X_, Z_, 3,1,3,1);
	r.Columns[1]= VecShuffle(X_, Z_, 2,0,2,0);
	r.Columns[2]= VecShuffle(Y_, W_, 3,1,3,1);
	r.Columns[3]= VecShuffle(Y_, W_, 2,0,2,0);

	return r;
}

hmm_vec3 HMM_UnProject(
    const hmm_vec3 &pos,
    const hmm_mat4 &vp, 
    int width, int height )
{
  hmm_mat4 inv = GetInverse( vp );
  hmm_vec4 point = { -1 + 2.0f * pos.X/width,
                     1 -2.0f * pos.Y/ height,
                     0.0f,
                     1.0f };
  hmm_vec4 x = inv * point;
  x /= x[3];
  return x.XYZ;
}

