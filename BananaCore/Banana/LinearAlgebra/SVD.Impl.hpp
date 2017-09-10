//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class RealType>
void SVDDecomposition::svd(const Mat2x2<RealType>& mat, Mat2x2<RealType>& w, Vec2<RealType>& e, Mat2x2<RealType>& v)
{
    /* Probably not the fastest, but I can't find any simple algorithms
       Got most of the derivation from:
       http://www.ualberta.ca/~mlipsett/ENGM541/Readings/svd_ellis.pdf
       www.imm.dtu.dk/pubdb/views/edoc_download.php/3274/pdf/imm3274.pdf
       https://github.com/victorliu/Cgeom/blob/master/geom_la.c (geom_matsvd2d method)
     */
    //If it is diagonal, SVD is trivial
    if(fabs(mat[0][1] - mat[1][0]) < Tiny && fabs(mat[0][1]) < Tiny)
    {
        w = Mat2x2<RealType>(mat[0][0] < 0 ? -1 : 1, 0, 0, mat[1][1] < 0 ? -1 : 1);
        e = Vec2<RealType>(fabs(mat[0][0]), fabs(mat[1][1]));
        v = Mat2x2<RealType>(1.0);
    }
    //Otherwise, we need to compute A^T*A
    else
    {
        float j   = mat[0][0] * mat[0][0] + mat[0][1] * mat[0][1],
              k   = mat[1][0] * mat[1][0] + mat[1][1] * mat[1][1],
              v_c = mat[0][0] * mat[1][0] + mat[0][1] * mat[1][1];
        //Check to see if A^T*A is diagonal
        if(fabs(v_c) < Tiny)
        {
            float s1 = sqrt(j),
                  s2 = fabs(j - k) < Tiny ? s1 : sqrt(k);
            e = Vec2<RealType>(s1, s2);
            v = Mat2x2<RealType>(1.0);
            w = Mat2x2<RealType>(
                mat[0][0] / s1, mat[1][0] / s2,
                mat[0][1] / s1, mat[1][1] / s2
                );
        }
        //Otherwise, solve quadratic for eigenvalues
        else
        {
            float jmk  = j - k,
                  jpk  = j + k,
                  root = sqrt(jmk * jmk + 4 * v_c * v_c),
                  eig  = (jpk + root) / 2,
                  s1   = sqrt(eig),
                  s2   = fabs(root) < Tiny ? s1 : sqrt((jpk - root) / 2);
            e = Vec2<RealType>(s1, s2);
            //Use eigenvectors of A^T*A as V
            float v_s = eig - j,
                  len = sqrt(v_s * v_s + v_c * v_c);
            v_c /= len;
            v_s /= len;
            v    = Mat2x2<RealType>(v_c, -v_s, v_s, v_c);
            //Compute w matrix as Av/s
            w = Mat2x2<RealType>(
                (mat[0][0] * v_c + mat[1][0] * v_s) / s1,
                (mat[1][0] * v_c - mat[0][0] * v_s) / s2,
                (mat[0][1] * v_c + mat[1][1] * v_s) / s1,
                (mat[1][1] * v_c - mat[0][1] * v_s) / s2
                );
        }
    }
}
