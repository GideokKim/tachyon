// Copyright 2022 arkworks contributors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.arkworks and the LICENCE-APACHE.arkworks
// file.

// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#include "tachyon/crypto/hashes/sponge/poseidon/poseidon_config.h"

#include <string>

#include "gtest/gtest.h"

#include "tachyon/crypto/hashes/sponge/poseidon/poseidon_params.h"
#include "tachyon/math/elliptic_curves/bls12/bls12_381/fr.h"
#include "tachyon/math/elliptic_curves/bn/bn254/fr.h"

namespace tachyon::crypto {

template <typename F, size_t rate, uint64_t alpha, size_t full_rounds,
          size_t partial_rounds>
void RunTest(size_t skip_matrices, std::string ark00_str, std::string mds00_str,
             std::string pre_sparse_mds11_str,
             std::string sparse_matrices0_row0_str,
             std::string sparse_matrices0_col_hat0_str) {
  using Params = PoseidonParams<F, rate, alpha, full_rounds, partial_rounds>;
  PoseidonConfig<Params> config = PoseidonConfig<Params>::Create(skip_matrices);
  ASSERT_TRUE(config.IsValid());
  EXPECT_EQ(config.ark(0, 0), *F::FromDecString(ark00_str));
  EXPECT_EQ(config.mds(0, 0), *F::FromDecString(mds00_str));
  EXPECT_EQ(config.pre_sparse_mds(1, 1),
            *F::FromDecString(pre_sparse_mds11_str));
  EXPECT_EQ(config.sparse_mds_matrices[0].row()[0],
            *F::FromDecString(sparse_matrices0_row0_str));
  EXPECT_EQ(config.sparse_mds_matrices[0].col_hat()[0],
            *F::FromDecString(sparse_matrices0_col_hat0_str));
}

TEST(PoseidonConfigTest, Create) {
  using F = math::bn254::Fr;
  F::Init();

  // clang-format off
  RunTest<F, 2, 5, 8, 57>(0, "6745197990210204598374042828761989596302876299545964402857411729872131034734", "7511745149465107256748700652201246547602992235352608707588321460060273774987",  "20498480049173041451757161739353136932402063966867101132544382489060457121690", "7511745149465107256748700652201246547602992235352608707588321460060273774987", "8364259238812534287689210722577399963878179320345509803468849104367466297989");
  RunTest<F, 3, 5, 8, 31>(0, "13403165100170528731188983416904733323082063763924490907636477756087868446385", "4843064272860702558353681805605581092414485968533095609154162537440763859608", "18904002742018845139579665761306904915446670558152364574366010338917470570969", "4843064272860702558353681805605581092414485968533095609154162537440763859608", "17098920592029650201655138442655581967768635903769554042626458334412341558169");
  RunTest<F, 4, 5, 8, 57>(0, "19113776568595304904649638772185171763805916665512167111307269381068492077003", "9390358363320792447057897590391227342305356869000968376798315031785873376651", "10430399957950918663081154404041014339007209286885623491793224536133959093342", "9390358363320792447057897590391227342305356869000968376798315031785873376651", "4258476155330118372762506614067358362596124649116737498715627157925163661");
  RunTest<F, 5, 5, 4, 31>(0, "12911143707477888215006316149665736087938931277132002862716953775656297280505", "10942845101262402626166273431356787436787991939175819278065571096963239049995", "5243571363387738625655552142589814423245410823902064800321501427822383158829", "10942845101262402626166273431356787436787991939175819278065571096963239049995", "10233199471798935861979427851690416803067121439733943143275749690073539672323");
  RunTest<F, 6, 5, 2, 12>(0, "6891333877554147985347986116215540686446242805416768740851383136400218469817", "9837954178279989965317992196165220609182866932765981962230951853077616895744", "2342265246701838568043436561372198242203879164391121705534106671605750623757", "9837954178279989965317992196165220609182866932765981962230951853077616895744", "9504868060103463837598804825403938487759722464496492662464197532903433072082");
  RunTest<F, 7, 5, 6, 57>(0, "3674224614566270991667957870461358488780972465005715233452288530417333318480", "20096031343166107597256293287487285680099080393005092613845214101013675429510", "10312826920506585789288853057573225788967547772875440745847746432386053340298", "20096031343166107597256293287487285680099080393005092613845214101013675429510", "21032704002095809084962846202624190836816781835379410962327180744509481109422");
  RunTest<F, 8, 5, 8, 63>(0, "14715728137766105031387583973733149375806784983272780095398485311648630967927", "708458300293891745856425423607721463509413916954480913172999113933455141974", "1739241146470184391733348241765630726223090738057028604336245542891367839190", "708458300293891745856425423607721463509413916954480913172999113933455141974", "9912718281831214028971669318551955931380391283853329273384463785744511549623");
  // clang-format on
}

}  // namespace tachyon::crypto
