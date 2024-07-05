#ifndef TACHYON_ZK_PLONK_HALO2_STRINGIFIERS_EXPRESSION_STRINGIFIER_H_
#define TACHYON_ZK_PLONK_HALO2_STRINGIFIERS_EXPRESSION_STRINGIFIER_H_

#include <memory>
#include <string>
#include <vector>

#include "tachyon/base/logging.h"
#include "tachyon/base/strings/rust_stringifier.h"
#include "tachyon/zk/expressions/constant_expression.h"
#include "tachyon/zk/expressions/negated_expression.h"
#include "tachyon/zk/expressions/product_expression.h"
#include "tachyon/zk/expressions/scaled_expression.h"
#include "tachyon/zk/expressions/sum_expression.h"
#include "tachyon/zk/plonk/expressions/advice_expression.h"
#include "tachyon/zk/plonk/expressions/challenge_expression.h"
#include "tachyon/zk/plonk/expressions/fixed_expression.h"
#include "tachyon/zk/plonk/expressions/instance_expression.h"
#include "tachyon/zk/plonk/expressions/selector_expression.h"
#include "tachyon/zk/plonk/halo2/stringifiers/challenge_stringifier.h"
#include "tachyon/zk/plonk/halo2/stringifiers/field_stringifier.h"
#include "tachyon/zk/plonk/halo2/stringifiers/phase_stringifier.h"
#include "tachyon/zk/plonk/halo2/stringifiers/rotation_stringifier.h"
#include "tachyon/zk/plonk/halo2/stringifiers/selector_stringifier.h"

namespace tachyon::base::internal {

template <typename F>
class RustDebugStringifier<zk::Expression<F>> {
 public:
  static std::ostream& AppendToStream(std::ostream& os, RustFormatter& fmt,
                                      const zk::Expression<F>& expression) {
    switch (expression.type()) {
      case zk::ExpressionType::kConstant: {
        return os << fmt.DebugTuple("Constant")
                         .Field(expression.ToConstant()->value())
                         .Finish();
      }
      case zk::ExpressionType::kSelector: {
        zk::plonk::Selector selector = expression.ToSelector()->selector();
        return os << fmt.DebugTuple("Selector").Field(selector).Finish();
      }
      case zk::ExpressionType::kFixed: {
        const zk::plonk::FixedQuery& query = expression.ToFixed()->query();
        return os << fmt.DebugStruct("Fixed")
                         .Field("query_index", query.index())
                         .Field("column_index", query.column().index())
                         .Field("rotation", query.rotation())
                         .Finish();
      }
      case zk::ExpressionType::kAdvice: {
        const zk::plonk::AdviceQuery& query = expression.ToAdvice()->query();
        base::internal::DebugStruct debug_struct = fmt.DebugStruct("Advice");
        debug_struct.Field("query_index", query.index())
            .Field("column_index", query.column().index())
            .Field("rotation", query.rotation());
        if (query.column().phase() != zk::plonk::kFirstPhase) {
          debug_struct.Field("phase", query.column().phase());
        }
        return os << debug_struct.Finish();
      }
      case zk::ExpressionType::kInstance: {
        const zk::plonk::InstanceQuery& query =
            expression.ToInstance()->query();
        return os << fmt.DebugStruct("Instance")
                         .Field("query_index", query.index())
                         .Field("column_index", query.column().index())
                         .Field("rotation", query.rotation())
                         .Finish();
      }
      case zk::ExpressionType::kChallenge: {
        zk::plonk::Challenge challenge = expression.ToChallenge()->challenge();
        return os << fmt.DebugTuple("Challenge").Field(challenge).Finish();
      }
      case zk::ExpressionType::kNegated: {
        return os << fmt.DebugTuple("Negated")
                         .Field(*expression.ToNegated()->expr())
                         .Finish();
      }
      case zk::ExpressionType::kSum: {
        const zk::SumExpression<F>* sum = expression.ToSum();
        return os << fmt.DebugTuple("Sum")
                         .Field(*sum->left())
                         .Field(*sum->right())
                         .Finish();
      }
      case zk::ExpressionType::kProduct: {
        const zk::ProductExpression<F>* product = expression.ToProduct();
        return os << fmt.DebugTuple("Product")
                         .Field(*product->left())
                         .Field(*product->right())
                         .Finish();
      }
      case zk::ExpressionType::kScaled: {
        const zk::ScaledExpression<F>* scaled = expression.ToScaled();
        return os << fmt.DebugTuple("Scaled")
                         .Field(*scaled->expr())
                         .Field(scaled->scale())
                         .Finish();
      }
      case zk::ExpressionType::kFirstRow:
      case zk::ExpressionType::kLastRow:
      case zk::ExpressionType::kTransition:
      case zk::ExpressionType::kVariable:
        NOTREACHED() << "AIR expression "
                     << ExpressionTypeToString(expression.type())
                     << " is not allowed in Halo2!";
    }
    NOTREACHED();
    return os;
  }
};

template <typename F>
class RustDebugStringifier<std::vector<std::unique_ptr<zk::Expression<F>>>> {
 public:
  static std::ostream& AppendToStream(
      std::ostream& os, RustFormatter& fmt,
      const std::vector<std::unique_ptr<zk::Expression<F>>>& expressions) {
    base::internal::DebugList list = fmt.DebugList();
    for (const std::unique_ptr<zk::Expression<F>>& expression : expressions) {
      list.Entry(*expression);
    }
    return os << list.Finish();
  }
};

}  // namespace tachyon::base::internal

#endif  // TACHYON_ZK_PLONK_HALO2_STRINGIFIERS_EXPRESSION_STRINGIFIER_H_
