#ifndef TACHYON_C_ZK_PLONK_HALO2_BN254_SHPLONK_PROVER_TYPE_TRAITS_H_
#define TACHYON_C_ZK_PLONK_HALO2_BN254_SHPLONK_PROVER_TYPE_TRAITS_H_

#include "tachyon/c/base/type_traits_forward.h"
#include "tachyon/c/zk/plonk/halo2/bn254_halo2_ls.h"
#include "tachyon/c/zk/plonk/halo2/bn254_shplonk_pcs.h"
#include "tachyon/c/zk/plonk/halo2/bn254_shplonk_prover.h"
#include "tachyon/c/zk/plonk/halo2/kzg_family_prover_impl.h"

namespace tachyon::c::base {

template <>
struct TypeTraits<zk::plonk::halo2::KZGFamilyProverImpl<
    zk::plonk::halo2::bn254::SHPlonkPCS, zk::plonk::halo2::bn254::Halo2LS>> {
  using CType = tachyon_halo2_bn254_shplonk_prover;
};

template <>
struct TypeTraits<tachyon_halo2_bn254_shplonk_prover> {
  using NativeType =
      zk::plonk::halo2::KZGFamilyProverImpl<zk::plonk::halo2::bn254::SHPlonkPCS,
                                            zk::plonk::halo2::bn254::Halo2LS>;
};

}  // namespace tachyon::c::base

#endif  // TACHYON_C_ZK_PLONK_HALO2_BN254_SHPLONK_PROVER_TYPE_TRAITS_H_
