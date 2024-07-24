// clang-format off
#include <stddef.h>
#include <stdint.h>

#include "tachyon/export.h"
%{if NeedsBigInt}
#include "tachyon/math/base/big_int.h"
%{endif NeedsBigInt}

namespace %{namespace} {

class TACHYON_EXPORT %{class}Config {
 public:
  constexpr static const char* kName = "%{namespace}::%{class}";

  constexpr static size_t kModulusBits = %{modulus_bits};
  constexpr static %{modulus_type} kModulus = %{modulus};

  constexpr static %{value_type} kOne = %{value};

  static void Init() {}
};

}  // namespace %{namespace}
// clang-format on
