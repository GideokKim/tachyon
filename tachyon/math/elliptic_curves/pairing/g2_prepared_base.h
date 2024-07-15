// Copyright 2022 arkworks contributors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.arkworks and the LICENCE-APACHE.arkworks
// file.

#ifndef TACHYON_MATH_ELLIPTIC_CURVES_PAIRING_G2_PREPARED_BASE_H_
#define TACHYON_MATH_ELLIPTIC_CURVES_PAIRING_G2_PREPARED_BASE_H_

#include <string>
#include <utility>

#include "tachyon/base/strings/string_util.h"
#include "tachyon/math/elliptic_curves/pairing/ell_coeff.h"

namespace tachyon::math {

template <typename PairingFriendlyCurveConfig>
class G2PreparedBase {
 public:
  using Config = PairingFriendlyCurveConfig;
  using G2Curve = typename Config::G2Curve;
  using Fp2 = typename G2Curve::BaseField;

  G2PreparedBase() = default;
  explicit G2PreparedBase(const EllCoeffs<Fp2>& ell_coeffs)
      : ell_coeffs_(ell_coeffs), infinity_(false) {}
  explicit G2PreparedBase(EllCoeffs<Fp2>&& ell_coeffs)
      : ell_coeffs_(std::move(ell_coeffs)), infinity_(false) {}

  const EllCoeffs<Fp2>& ell_coeffs() const { return ell_coeffs_; }
  bool infinity() const { return infinity_; }

  std::string ToString() const {
    return absl::Substitute("{ell_coeffs: $0, infinity: $1}",
                            base::ContainerToString(ell_coeffs_), infinity_);
  }

  std::string ToHexString(bool pad_zero = false) const {
    return absl::Substitute("{ell_coeffs: $0, infinity: $1}",
                            base::ContainerToString(ell_coeffs_, pad_zero),
                            infinity_);
  }

 protected:
  // Stores the coefficients of the line evaluations as calculated in
  // https://eprint.iacr.org/2013/722.pdf
  EllCoeffs<Fp2> ell_coeffs_;
  bool infinity_ = true;
};

}  // namespace tachyon::math

#endif  // TACHYON_MATH_ELLIPTIC_CURVES_PAIRING_G2_PREPARED_BASE_H_
