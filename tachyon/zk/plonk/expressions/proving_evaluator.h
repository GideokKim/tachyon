// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_EXPRESSIONS_PROVING_EVALUATOR_H_
#define TACHYON_ZK_PLONK_EXPRESSIONS_PROVING_EVALUATOR_H_

#include <vector>

#include "absl/types/span.h"

#include "tachyon/zk/expressions/constant_expression.h"
#include "tachyon/zk/expressions/evaluator.h"
#include "tachyon/zk/expressions/negated_expression.h"
#include "tachyon/zk/expressions/product_expression.h"
#include "tachyon/zk/expressions/scaled_expression.h"
#include "tachyon/zk/expressions/sum_expression.h"
#include "tachyon/zk/plonk/base/multi_phase_ref_table.h"
#include "tachyon/zk/plonk/expressions/advice_expression.h"
#include "tachyon/zk/plonk/expressions/challenge_expression.h"
#include "tachyon/zk/plonk/expressions/fixed_expression.h"
#include "tachyon/zk/plonk/expressions/instance_expression.h"
#include "tachyon/zk/plonk/expressions/selector_expression.h"

namespace tachyon::zk::plonk {

template <typename Evals>
class ProvingEvaluator
    : public Evaluator<typename Evals::Field, typename Evals::Field> {
 public:
  using Field = typename Evals::Field;

  ProvingEvaluator() = default;
  ProvingEvaluator(int32_t idx, int32_t size, int32_t rot_scale,
                   const MultiPhaseRefTable<Evals>& table)
      : idx_(idx), size_(size), rot_scale_(rot_scale), table_(table) {}

  int32_t idx() const { return idx_; }
  void set_idx(int32_t idx) { idx_ = idx; }
  int32_t size() const { return size_; }
  int32_t rot_scale() const { return rot_scale_; }

  // Evaluator methods
  Field Evaluate(const Expression<Field>* input) override {
    switch (input->type()) {
      case ExpressionType::kConstant:
        return input->ToConstant()->value();
      case ExpressionType::kSelector:
        NOTREACHED() << "virtual selectors are removed during optimization";
        break;
      case ExpressionType::kFixed: {
        const FixedExpression<Field>* fixed_expr = input->ToFixed();
        const FixedQuery& query = fixed_expr->query();
        const Evals& evals = table_.GetFixedColumns()[query.column().index()];
        return evals[query.rotation().GetIndex(idx_, rot_scale_, size_)];
      }
      case ExpressionType::kAdvice: {
        const AdviceExpression<Field>* advice_expr = input->ToAdvice();
        const AdviceQuery& query = advice_expr->query();
        const Evals& evals = table_.GetAdviceColumns()[query.column().index()];
        return evals[query.rotation().GetIndex(idx_, rot_scale_, size_)];
      }
      case ExpressionType::kInstance: {
        const InstanceExpression<Field>* instance_expr = input->ToInstance();
        const InstanceQuery& query = instance_expr->query();
        const Evals& evals =
            table_.GetInstanceColumns()[query.column().index()];
        return evals[query.rotation().GetIndex(idx_, rot_scale_, size_)];
      }
      case ExpressionType::kChallenge:
        return table_.challenges()[input->ToChallenge()->challenge().index()];
      case ExpressionType::kNegated:
        return -Evaluate(input->ToNegated()->expr());
      case ExpressionType::kSum: {
        const SumExpression<Field>* sum = input->ToSum();
        return Evaluate(sum->left()) + Evaluate(sum->right());
      }
      case ExpressionType::kProduct: {
        const ProductExpression<Field>* product = input->ToProduct();
        return Evaluate(product->left()) * Evaluate(product->right());
      }
      case ExpressionType::kScaled: {
        const ScaledExpression<Field>* scaled = input->ToScaled();
        return Evaluate(scaled->expr()) * scaled->scale();
      }
      case ExpressionType::kFirstRow:
      case ExpressionType::kLastRow:
      case ExpressionType::kTransition:
      case ExpressionType::kVariable:
        NOTREACHED() << "AIR expression "
                     << ExpressionTypeToString(input->type())
                     << " is not allowed in plonk!";
    }
    NOTREACHED();
    return Field::Zero();
  }

 private:
  int32_t idx_ = 0;
  int32_t size_ = 0;
  int32_t rot_scale_ = 0;
  MultiPhaseRefTable<Evals> table_;
};

}  // namespace tachyon::zk::plonk

#endif  // TACHYON_ZK_PLONK_EXPRESSIONS_PROVING_EVALUATOR_H_
