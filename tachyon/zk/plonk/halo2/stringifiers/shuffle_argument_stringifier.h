// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_HALO2_STRINGIFIERS_SHUFFLE_ARGUMENT_STRINGIFIER_H_
#define TACHYON_ZK_PLONK_HALO2_STRINGIFIERS_SHUFFLE_ARGUMENT_STRINGIFIER_H_

#include <ostream>

#include "tachyon/base/strings/rust_stringifier.h"
#include "tachyon/zk/plonk/halo2/stringifiers/expression_stringifier.h"
#include "tachyon/zk/shuffle/argument.h"

namespace tachyon::base::internal {

template <typename F>
class RustDebugStringifier<zk::shuffle::Argument<F>> {
 public:
  static std::ostream& AppendToStream(
      std::ostream& os, RustFormatter& fmt,
      const zk::shuffle::Argument<F>& argument) {
    return os << fmt.DebugStruct("Argument")
                     .Field("input_expressions", argument.input_expressions())
                     .Field("shuffle_expressions",
                            argument.shuffle_expressions())
                     .Finish();
  }
};

}  // namespace tachyon::base::internal

#endif  // TACHYON_ZK_PLONK_HALO2_STRINGIFIERS_SHUFFLE_ARGUMENT_STRINGIFIER_H_
