// Copyright (c) 2022 The Plonky3 Authors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.plonky3 and the LICENCE-APACHE.plonky3
// file.

#ifndef TACHYON_CRYPTO_HASHES_COMPRESSOR_H_
#define TACHYON_CRYPTO_HASHES_COMPRESSOR_H_

namespace tachyon::crypto {

template <typename _Derived>
class Compressor {
 public:
  using Derived = _Derived;

  // TODO(chokobole): Make this accept iterator as an argument.
  template <typename T>
  auto Compress(const T& input) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    auto state = derived.CreateEmptyState();
    return derived.DoCompress(state, input);
  }
};

}  // namespace tachyon::crypto

#endif  // TACHYON_CRYPTO_HASHES_COMPRESSOR_H_
