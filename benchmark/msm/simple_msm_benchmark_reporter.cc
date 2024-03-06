#include "benchmark/msm/simple_msm_benchmark_reporter.h"

#include <string>
#include <vector>

#include "tachyon/base/strings/string_number_conversions.h"

namespace tachyon {

SimpleMSMBenchmarkReporter::SimpleMSMBenchmarkReporter(
    std::string_view title, const std::vector<uint64_t>& exponents)
    : exponents_(exponents) {
  title_ = std::string(title);
  for (uint64_t exponent : exponents) {
    targets_.push_back(base::NumberToString(exponent));
  }
  results_.resize(exponents.size());
  AddVendor("tachyon");
}

void SimpleMSMBenchmarkReporter::AddVendor(std::string_view name) {
  column_headers_.push_back(std::string(name));
}

}  // namespace tachyon
