// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_VANISHING_EVALUATION_INPUT_H_
#define TACHYON_ZK_PLONK_VANISHING_EVALUATION_INPUT_H_

#include <stddef.h>

#include <utility>
#include <vector>

#include "tachyon/zk/plonk/base/multi_phase_ref_table.h"

namespace tachyon::zk::plonk {

template <typename Evals>
class EvaluationInput {
 public:
  using F = typename Evals::Field;

  EvaluationInput(std::vector<F>&& intermediates,
                  std::vector<int32_t>&& rotations,
                  const MultiPhaseRefTable<Evals>& table, const F& theta,
                  const F& beta, const F& gamma, const F& y, int32_t n)
      : intermediates_(std::move(intermediates)),
        rotations_(std::move(rotations)),
        table_(table),
        theta_(theta),
        beta_(beta),
        gamma_(gamma),
        y_(y),
        n_(n) {}

  const std::vector<F>& intermediates() const { return intermediates_; }
  std::vector<F>& intermediates() { return intermediates_; }
  const std::vector<int32_t>& rotations() const { return rotations_; }
  std::vector<int32_t>& rotations() { return rotations_; }
  const MultiPhaseRefTable<Evals>& table() const { return table_; }
  const F& theta() const { return theta_; }
  const F& beta() const { return beta_; }
  const F& gamma() const { return gamma_; }
  const F& y() const { return y_; }
  int32_t n() const { return n_; }

 private:
  std::vector<F> intermediates_;
  std::vector<int32_t> rotations_;
  const MultiPhaseRefTable<Evals>& table_;
  const F& theta_;
  const F& beta_;
  const F& gamma_;
  const F& y_;
  int32_t n_ = 0;
};

}  // namespace tachyon::zk::plonk

#endif  // TACHYON_ZK_PLONK_VANISHING_EVALUATION_INPUT_H_
