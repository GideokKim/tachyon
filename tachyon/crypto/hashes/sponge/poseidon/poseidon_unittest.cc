// Copyright 2022 arkworks contributors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.arkworks and the LICENCE-APACHE.arkworks
// file.

#include "tachyon/crypto/hashes/sponge/poseidon/poseidon.h"

#include <vector>

#include "gtest/gtest.h"

#include "tachyon/base/buffer/vector_buffer.h"
#include "tachyon/crypto/hashes/sponge/poseidon/poseidon_params.h"
#include "tachyon/math/elliptic_curves/bls12/bls12_381/fr.h"
#include "tachyon/math/finite_fields/baby_bear/baby_bear.h"
#include "tachyon/math/finite_fields/test/finite_field_test.h"

namespace tachyon::crypto {

namespace {

class PoseidonTest : public math::FiniteFieldTest<math::bls12_381::Fr> {};

}  // namespace

TEST_F(PoseidonTest, AbsorbSqueeze) {
  using Fr = math::bls12_381::Fr;
  using Params = PoseidonParams<Fr, 2, 17, 8, 31>;

  auto config = PoseidonConfig<Params>::Create(0);
  PoseidonSponge<Params> sponge(std::move(config));
  SpongeState<Params> state;
  std::vector<Fr> inputs = {Fr(0), Fr(1), Fr(2)};
  ASSERT_TRUE(sponge.Absorb(state, inputs));
  std::vector<Fr> result = sponge.SqueezeNativeFieldElements(state, 3);
  std::vector<Fr> expected = {
      *Fr::FromDecString(
          "404427934635713040283377530022421867103101638970489622"
          "78675457993207843616876"),
      *Fr::FromDecString(
          "266437446169989800029115314522409928771122402171620296"
          "0480903840045233645301"),
      *Fr::FromDecString(
          "501910788280669236620702282565306929518015040434228440"
          "38937334196346054068797"),
  };
  EXPECT_EQ(result, expected);
}

TEST_F(PoseidonTest, Copyable) {
  using Fr = math::bls12_381::Fr;
  using Params = PoseidonParams<Fr, 2, 17, 8, 31>;

  auto config = PoseidonConfig<Params>::Create(0);
  PoseidonSponge<Params> expected(std::move(config));

  base::Uint8VectorBuffer write_buf;
  ASSERT_TRUE(write_buf.Grow(base::EstimateSize(expected)));
  ASSERT_TRUE(write_buf.Write(expected));
  ASSERT_TRUE(write_buf.Done());

  write_buf.set_buffer_offset(0);

  PoseidonSponge<Params> value;
  ASSERT_TRUE(write_buf.Read(&value));

  EXPECT_EQ(value, expected);
}

namespace {

class PackedPoseidonTest : public math::FiniteFieldTest<math::PackedBabyBear> {
};

}  // namespace

TEST_F(PackedPoseidonTest, AbsorbSqueeze) {
  using F = math::BabyBear;
  using PackedF = math::PackedBabyBear;
  using Params = PoseidonParams<F, 2, 17, 8, 31>;
  using PackedParams = PoseidonParams<PackedF, 2, 17, 8, 31>;

  auto packed_config = PoseidonConfig<PackedParams>::Create(0);
  PoseidonSponge<PackedParams> packed_sponge(std::move(packed_config));
  SpongeState<PackedParams> packed_state;
  std::vector<PackedF> packed_inputs = {PackedF(0), PackedF(1), PackedF(2)};
  ASSERT_TRUE(packed_sponge.Absorb(packed_state, packed_inputs));
  std::vector<PackedF> packed_result =
      packed_sponge.SqueezeNativeFieldElements(packed_state, 1);

  auto config = PoseidonConfig<Params>::Create(0);
  PoseidonSponge<Params> sponge(std::move(config));
  SpongeState<Params> state;
  std::vector<F> inputs = {F(0), F(1), F(2)};
  ASSERT_TRUE(sponge.Absorb(state, inputs));
  std::vector<F> result = sponge.SqueezeNativeFieldElements(state, 1);

  EXPECT_EQ(packed_result[0], PackedF::Broadcast(result[0]));
}

}  // namespace tachyon::crypto
