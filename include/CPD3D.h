// cpd - Coherent Point Drift
// Copyright (C) 2017 Pete Gadomski <pete.gadomski@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef CPD_3D_H
#define CPD_3D_H

#include <omp.h>

#include <Eigen/Dense>
#include <chrono>
#include <cmath>
#include <limits>

namespace CPD {

using Matrix = Eigen::MatrixXd;
using Vector = Eigen::VectorXd;
using IndexVector = Eigen::Matrix<Matrix::Index, Eigen::Dynamic, 1>;

struct Normalization {
    Vector fixed_mean;
    Matrix fixed;
    double fixed_scale;
    Vector moving_mean;
    Matrix moving;
    double moving_scale;

    Normalization(const Matrix& f, const Matrix& m, bool linked = true);
};

struct Probabilities {
    Vector p1;
    Vector pt1;
    Matrix px;
    double l;
    IndexVector correspondence;
};

struct NonrigidResult {
    Matrix points;
    double sigma2;
    IndexVector correspondence;
    std::chrono::microseconds runtime;
    size_t iterations;

    void denormalize(const Normalization& normalization);
};

class Nonrigid {
   public:
    Nonrigid();

    Nonrigid& beta(double beta);
    Nonrigid& lambda(double lambda);
    Nonrigid& linked(bool linked);
    Nonrigid& max_iterations(size_t maxit);
    Nonrigid& normalize(bool n);
    Nonrigid& outliers(double o);
    Nonrigid& sigma2(double s2);
    Nonrigid& tolerance(double tol);
    Nonrigid& correspondence(bool corr);

    NonrigidResult run(Matrix fixed, Matrix moving);

   private:
    double m_lambda, m_beta;
    bool m_linked;
    size_t m_max_iterations;
    bool m_normalize;
    double m_outliers, m_sigma2, m_tolerance;
    bool m_correspondence;

    Matrix m_g, m_w;

    void init(const Matrix& moving);
    double default_sigma2(const Matrix& fixed, const Matrix& moving) const;
    Matrix affinity(const Matrix& x, const Matrix& y, double beta) const;
    Probabilities compute_probabilities(const Matrix& fixed,
                                        const Matrix& moving,
                                        double sigma2) const;
    NonrigidResult compute_one(const Matrix& fixed, const Matrix& moving,
                               const Probabilities& probs, double sigma2) const;
};

NonrigidResult nonrigid(const Matrix& fixed, const Matrix& moving);

}  // namespace CPD

#endif  // CPD_3D_H
