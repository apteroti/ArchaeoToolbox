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

#include "CPD3D.h"

namespace CPD {

// Constructor: normalize fixed (f) and moving (m) point sets
Normalization::Normalization(const Matrix& f, const Matrix& m, bool linked)
    // Compute column-wise mean (centroid) of fixed points
    : fixed_mean(f.colwise().mean()),
      // Center fixed points by subtracting mean from each row
      fixed(f.rowwise() - fixed_mean.transpose()),
      // Compute scale (RMS of fixed points)
      fixed_scale(std::sqrt(f.array().square().sum() / f.rows())),
      // Compute column-wise mean of moving points
      moving_mean(m.colwise().mean()),
      // Center moving points by subtracting moving mean from each row
      moving(m.rowwise() - moving_mean.transpose()),
      // Compute scale (RMS of moving points)
      moving_scale(std::sqrt(m.array().square().sum() / m.rows())) {
    if (linked) {
        // If linked, unify scales to the larger one (assumes similar scale)
        double scale = std::max(fixed_scale, moving_scale);
        fixed_scale = scale;
        moving_scale = scale;
    }
    // Normalize fixed points by fixed_scale
    fixed /= fixed_scale;
    // Normalize moving points by moving_scale
    moving /= moving_scale;
}

// Undo normalization: rescale and translate points back to original space
void NonrigidResult::denormalize(const Normalization& normalization) {
    // Scale points back
    points = points * normalization.fixed_scale;
    // Translate points by adding fixed mean
    points.rowwise() += normalization.fixed_mean.transpose();
}

// Constructor: set default parameter values for Nonrigid registration
Nonrigid::Nonrigid()
    : m_lambda(3.0),              // Regularization weight (smoothness)
      m_beta(3.0),                // Gaussian kernel width (influence radius)
      m_linked(true),             // Whether to use linked normalization
      m_max_iterations(150),      // Maximum iterations for EM algorithm
      m_normalize(true),          // Whether to normalize input point sets
      m_outliers(0.1),            // Outlier weight (expected proportion)
      m_sigma2(0.0),              // Initial noise variance (0 means auto)
      m_tolerance(1e-5),          // Convergence tolerance threshold
      m_correspondence(false) {}  // Whether to compute hard correspondences

// Setter for beta (Gaussian kernel width)
Nonrigid& Nonrigid::beta(double beta) {
    m_beta = beta;
    return *this;
}

// Setter for lambda (regularization weight)
Nonrigid& Nonrigid::lambda(double lambda) {
    m_lambda = lambda;
    return *this;
}

// Setter for linked normalization flag
Nonrigid& Nonrigid::linked(bool linked) {
    m_linked = linked;
    return *this;
}

// Setter for max iterations
Nonrigid& Nonrigid::max_iterations(size_t maxit) {
    m_max_iterations = maxit;
    return *this;
}

// Setter for normalize flag
Nonrigid& Nonrigid::normalize(bool normalize) {
    m_normalize = normalize;
    return *this;
}

// Setter for outlier weight
Nonrigid& Nonrigid::outliers(double outliers) {
    m_outliers = outliers;
    return *this;
}

// Setter for initial sigma2 (noise variance)
Nonrigid& Nonrigid::sigma2(double sigma2) {
    m_sigma2 = sigma2;
    return *this;
}

// Setter for convergence tolerance
Nonrigid& Nonrigid::tolerance(double tolerance) {
    m_tolerance = tolerance;
    return *this;
}

// Setter for correspondence flag
Nonrigid& Nonrigid::correspondence(bool correspondence) {
    m_correspondence = correspondence;
    return *this;
}

// Compute Gaussian affinity matrix between points in x and y with width beta
Matrix Nonrigid::affinity(const Matrix& x, const Matrix& y, double beta) const {
    // Precompute denominator factor in Gaussian kernel exponent: -2 * beta^2
    const double k = -2.0 * beta * beta;
    // Initialize output matrix of size [x.rows() x y.rows()]
    Matrix g(x.rows(), y.rows());
#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(y.rows()); ++i) {
        // For each point in y, compute squared distances to all points in x,
        // then apply Gaussian kernel elementwise:
        // G_ij = exp(-||x_i - y_j||^2 / (2 * beta^2))
        g.col(i) =
            ((x.rowwise() - y.row(i)).array().square().rowwise().sum() / k)
                .exp();
    }
    return g;
}

// Compute default initial noise variance sigma² based on input points
double Nonrigid::default_sigma2(const Matrix& fixed,
                                const Matrix& moving) const {
    // Formula estimates initial sigma^2 from point sets using
    // trace and sums, corresponding to average squared distance between
    // points normalized by dimensionality and number of points
    return ((moving.rows() * (fixed.transpose() * fixed).trace()) +
            (fixed.rows() * (moving.transpose() * moving).trace()) -
            2 * fixed.colwise().sum() * moving.colwise().sum().transpose()) /
           (fixed.rows() * moving.rows() * fixed.cols());
}

// Initialize the Gaussian kernel matrix m_g and zero matrix m_w
void Nonrigid::init(const Matrix& moving) {
    // Compute kernel matrix m_g for moving points using affinity function
    m_g = affinity(moving, moving, m_beta);
    // Initialize m_w as zero matrix with size [moving.rows() x moving.cols()]
    m_w = Matrix::Zero(moving.rows(), moving.cols());
}

// Compute the E-step probabilities in EM: soft correspondence between points
Probabilities Nonrigid::compute_probabilities(const Matrix& fixed,
                                              const Matrix& moving,
                                              double sigma2) const {
    // Precompute constant in Gaussian exponent denominator (-2 * sigma2)
    double ksig = -2.0 * sigma2;
    size_t cols = fixed.cols();

    // Compute outlier weight factor for probability normalization
    double outlier_tmp =
        (m_outliers * moving.rows() * std::pow(-ksig * M_PI, 0.5 * cols)) /
        ((1 - m_outliers) * fixed.rows());

    // Initialize vectors and matrices for probability computations
    Vector p = Vector::Zero(moving.rows());
    Vector p1 = Vector::Zero(moving.rows());
    Vector p1_max = Vector::Zero(moving.rows());
    Vector pt1 = Vector::Zero(fixed.rows());
    Matrix px = Matrix::Zero(moving.rows(), cols);
    IndexVector correspondence = IndexVector::Zero(moving.rows());
    double l = 0.0;  // Log-likelihood accumulator

    // For each fixed point (row i)
    for (Matrix::Index i = 0; i < fixed.rows(); ++i) {
        double sp = 0;  // Sum of probabilities for normalization
        // For each moving point (row j)
        for (Matrix::Index j = 0; j < moving.rows(); ++j) {
            // Squared Euclidean distance between fixed[i] and moving[j]
            double razn = (fixed.row(i) - moving.row(j)).array().square().sum();
            // Compute Gaussian kernel value for this pair
            p(j) = std::exp(razn / ksig);
            // Accumulate sum for normalization
            sp += p(j);
        }
        // Add outlier weight to denominator
        sp += outlier_tmp;
        // Probability that fixed point i is not an outlier
        pt1(i) = 1 - outlier_tmp / sp;

        // Accumulate per moving point probabilities weighted by fixed points
        for (Matrix::Index j = 0; j < moving.rows(); ++j) {
            // Update marginal probability for moving point j
            p1(j) += p(j) / sp;
            // Update weighted sum of fixed points corresponding to moving j
            px.row(j) += fixed.row(i) * p(j) / sp;

            // Track maximum probability assignment for hard correspondence
            if (p(j) / sp > p1_max(j)) {
                correspondence(j) = i;
                p1_max(j) = p(j) / sp;
            }
        }
        // Accumulate negative log likelihood contribution for fixed point i
        l += -std::log(sp);
    }

    // Add penalty term proportional to model dimensionality and variance
    l += cols * fixed.rows() * std::log(sigma2) / 2;

    // Return all computed probability-related quantities
    return {p1, pt1, px, l, correspondence};
}

// Compute the M-step: update deformation parameters and estimate new points
NonrigidResult Nonrigid::compute_one(const Matrix& fixed, const Matrix& moving,
                                     const Probabilities& probabilities,
                                     double sigma2) const {
    size_t cols = fixed.cols();
    // Diagonal matrix of marginal probabilities for moving points
    auto dp = probabilities.p1.asDiagonal();

    // Solve linear system for deformation weights 'w':
    // (p1 * G + lambda * sigma2 * I) * w = px - p1 * moving
    Matrix w = (dp * m_g + m_lambda * sigma2 *
                               Matrix::Identity(moving.rows(), moving.rows()))
                   .colPivHouseholderQr()
                   .solve(probabilities.px - dp * moving);

    NonrigidResult result;

    // Compute updated points by applying deformation to moving points
    result.points = moving + m_g * w;

    // Compute normalization factor (sum of probabilities)
    double np = probabilities.p1.sum();

    // Update sigma2 (variance) based on weighted residual error between fixed
    // points and deformed points
    result.sigma2 = std::abs(
        ((fixed.array().square() * probabilities.pt1.replicate(1, cols).array())
             .sum() +
         (result.points.array().square() *
          probabilities.p1.replicate(1, cols).array())
             .sum() -
         2 * (probabilities.px.transpose() * result.points).trace()) /
        (np * cols));

    return result;
}

// Main registration loop: run EM optimization for nonrigid CPD
NonrigidResult Nonrigid::run(Matrix fixed, Matrix moving) {
    // Start timer for performance measurement
    auto tic = std::chrono::high_resolution_clock::now();

    // Normalize input point sets if requested
    Normalization normalization(fixed, moving, m_linked);
    if (m_normalize) {
        fixed = normalization.fixed;
        moving = normalization.moving;
    }

    // Initialize kernel matrix and weight matrix
    init(moving);

    NonrigidResult result;
    // Initialize points to moving points at start
    result.points = moving;

    // Initialize sigma2 (variance)
    if (m_sigma2 == 0.0) {
        // If sigma2 not provided, compute default from points
        result.sigma2 = default_sigma2(fixed, moving);
    } else if (m_normalize) {
        // If normalized, scale provided sigma2 accordingly
        result.sigma2 = m_sigma2 / normalization.fixed_scale;
    } else {
        // Use provided sigma2 as is
        result.sigma2 = m_sigma2;
    }

    size_t iter = 0;
    double ntol = m_tolerance + 10.0;  // Initialize tolerance measure
    double l = 0.;                     // Log-likelihood from previous iteration

    // EM iteration loop: stop on max iterations or convergence
    while (iter < m_max_iterations && ntol > m_tolerance &&
           result.sigma2 > 10 * std::numeric_limits<double>::epsilon()) {
        // E-step: compute soft correspondences given current estimate of points
        Probabilities probabilities =
            compute_probabilities(fixed, result.points, result.sigma2);

        // Compute normalized change in log-likelihood for convergence check
        ntol = std::abs((probabilities.l - l) / probabilities.l);
        l = probabilities.l;

        // M-step: update points given computed probabilities
        result = compute_one(fixed, moving, probabilities, result.sigma2);

        ++iter;  // Increase iteration counter
    }

    // Undo normalization if applied
    if (m_normalize) {
        result.denormalize(normalization);
    }

    // If hard correspondence requested, compute after convergence
    if (m_correspondence) {
        Probabilities probabilities =
            compute_probabilities(fixed, result.points, result.sigma2);
        result.correspondence = probabilities.correspondence;
    }

    // Stop timer and compute elapsed runtime
    auto toc = std::chrono::high_resolution_clock::now();
    result.runtime =
        std::chrono::duration_cast<std::chrono::microseconds>(toc - tic);
    result.iterations = iter;

    return result;
}

// Convenience function to run default nonrigid CPD
NonrigidResult nonrigid(const Matrix& fixed, const Matrix& moving) {
    Nonrigid nonrigid;
    return nonrigid.run(fixed, moving);
}

}  // namespace CPD
