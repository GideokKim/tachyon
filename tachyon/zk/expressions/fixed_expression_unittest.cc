#include "tachyon/zk/expressions/fixed_expression.h"

#include "gtest/gtest.h"

#include "tachyon/math/finite_fields/test/finite_field_test.h"
#include "tachyon/math/finite_fields/test/gf7.h"

namespace tachyon::zk {

using F = math::GF7;

class FixedExpressionTest : public math::FiniteFieldTest<F> {};

TEST_F(FixedExpressionTest, DegreeComplexity) {
  std::unique_ptr<FixedExpression<F>> expr =
      FixedExpression<F>::CreateForTesting(
          plonk::FixedQuery(1, Rotation(1), plonk::FixedColumnKey(1)));
  EXPECT_EQ(expr->Degree(), size_t{1});
  EXPECT_EQ(expr->Complexity(), uint64_t{1});
}

}  // namespace tachyon::zk
