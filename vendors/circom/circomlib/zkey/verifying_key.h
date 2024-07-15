#ifndef VENDORS_CIRCOM_CIRCOMLIB_ZKEY_VERIFYING_KEY_H_
#define VENDORS_CIRCOM_CIRCOMLIB_ZKEY_VERIFYING_KEY_H_

#include <string>

#include "absl/strings/substitute.h"

#include "tachyon/base/buffer/read_only_buffer.h"

namespace tachyon::circom {

template <typename Curve>
struct VerifyingKey {
  using G1AffinePoint = typename Curve::G1Curve::AffinePoint;
  using G2AffinePoint = typename Curve::G2Curve::AffinePoint;

  G1AffinePoint* alpha_g1 = nullptr;
  G1AffinePoint* beta_g1 = nullptr;
  G2AffinePoint* beta_g2 = nullptr;
  G2AffinePoint* gamma_g2 = nullptr;
  G1AffinePoint* delta_g1 = nullptr;
  G2AffinePoint* delta_g2 = nullptr;

  bool operator==(const VerifyingKey& other) const {
    return *alpha_g1 == *other.alpha_g1 && *beta_g1 == *other.beta_g1 &&
           *beta_g2 == *other.beta_g2 && *gamma_g2 == *other.gamma_g2 &&
           *delta_g1 == *other.delta_g1 && *delta_g2 == *other.delta_g2;
  }
  bool operator!=(const VerifyingKey& other) const {
    return !operator==(other);
  }

  bool Read(const base::ReadOnlyBuffer& buffer) {
    return buffer.ReadPtr(&alpha_g1, 1) && buffer.ReadPtr(&beta_g1, 1) &&
           buffer.ReadPtr(&beta_g2, 1) && buffer.ReadPtr(&gamma_g2, 1) &&
           buffer.ReadPtr(&delta_g1, 1) && buffer.ReadPtr(&delta_g2, 1);
  }

  // NOTE(chokobole): the fields are represented in montgomery form.
  std::string ToString() const {
    return absl::Substitute(
        "{alpha_g1: $0, beta_g1: $1, beta_g2: $2, gamma_g2: $3, delta_g1: $4, "
        "delta_g2: $5}",
        alpha_g1->ToString(), beta_g1->ToString(), beta_g2->ToString(),
        gamma_g2->ToString(), delta_g1->ToString(), delta_g2->ToString());
  }
};

}  // namespace tachyon::circom

#endif  // VENDORS_CIRCOM_CIRCOMLIB_ZKEY_VERIFYING_KEY_H_
