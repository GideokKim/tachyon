#include "absl/strings/substitute.h"

#include "circomlib/circuit/quadratic_arithmetic_program.h"
#include "circomlib/json/groth16_proof.h"
#include "circomlib/json/json.h"
#include "circomlib/json/prime_field.h"
#include "circomlib/wtns/wtns.h"
#include "circomlib/zkey/zkey.h"
#include "tachyon/base/console/iostream.h"
#include "tachyon/base/files/file_path_flag.h"
#include "tachyon/base/flag/flag_parser.h"
#include "tachyon/base/time/time.h"
#include "tachyon/math/elliptic_curves/bls12/bls12_381/bls12_381.h"
#include "tachyon/math/elliptic_curves/bn/bn254/bn254.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain_factory.h"
#include "tachyon/zk/r1cs/groth16/prove.h"
#include "tachyon/zk/r1cs/groth16/verify.h"

#if TACHYON_CUDA
#include "tachyon/math/polynomials/univariate/icicle/icicle_ntt_holder.h"
#endif

namespace tachyon {

enum class Curve {
  kBN254,
  kBLS12_381,
};

namespace base {

template <>
class FlagValueTraits<Curve> {
 public:
  static bool ParseValue(std::string_view input, Curve* value,
                         std::string* reason) {
    if (input == "bn254") {
      *value = Curve::kBN254;
    } else if (input == "bls12_381") {
      *value = Curve::kBLS12_381;
    } else {
      *reason = absl::Substitute("Unknown curve: $0", input);
      return false;
    }
    return true;
  }
};

}  // namespace base

namespace circom {

struct ProverTime {
  base::TimeDelta total;
  base::TimeDelta max;
  size_t num_runs = 0;

  void Add(base::TimeDelta delta) {
    ++num_runs;
    total += delta;
    max = std::max(delta, max);
  }

  base::TimeDelta GetAvg() const {
    CHECK_NE(num_runs, size_t{0});
    return total / num_runs;
  }
};

template <typename Curve>
void CreateProof(const base::FilePath& zkey_path,
                 const base::FilePath& witness_path,
                 const base::FilePath& proof_path,
                 const base::FilePath& public_path, bool no_zk, size_t num_runs,
                 bool verify) {
  using F = typename Curve::G1Curve::ScalarField;
  using Domain = math::UnivariateEvaluationDomain<F, SIZE_MAX>;

  Curve::Init();

  base::TimeTicks start = base::TimeTicks::Now();
  zk::r1cs::groth16::ProvingKey<Curve> proving_key;
  zk::r1cs::ConstraintMatrices<F> constraint_matrices;
  std::cout << "Start parsing zkey" << std::endl;
  {
    std::unique_ptr<ZKey<Curve>> zkey = ParseZKey<Curve>(zkey_path);
    CHECK(zkey);

    proving_key = std::move(*zkey).TakeProvingKey().ToNativeProvingKey();
    constraint_matrices = std::move(*zkey).TakeConstraintMatrices().ToNative();
  }

  base::TimeTicks end = base::TimeTicks::Now();
  std::cout << "Time taken for parsing zkey: " << end - start << std::endl;
  start = end;

  std::cout << "Start parsing witness" << std::endl;
  std::unique_ptr<Wtns<F>> wtns = ParseWtns<F>(witness_path);
  CHECK(wtns);

  end = base::TimeTicks::Now();
  std::cout << "Time taken for parsing witness: " << end - start << std::endl;
  start = end;

  zk::r1cs::groth16::Proof<Curve> proof;
  absl::Span<const F> full_assignments = wtns->GetWitnesses();
  std::unique_ptr<Domain> domain =
      Domain::Create(constraint_matrices.num_constraints +
                     constraint_matrices.num_instance_variables);
#if TACHYON_CUDA
  std::cout << "Start initializing Icicle NTT domain" << std::endl;
  math::IcicleNTTHolder<F> icicle_ntt_holder =
      math::IcicleNTTHolder<F>::Create();
  // NOTE(chokobole): For |domain->size()| less than 8, it's very slow to
  // initialize the domain of |IcicleNTT|.
  if (domain->size() >= 8) {
    CHECK(icicle_ntt_holder->Init(domain->group_gen()));
  }
  domain->set_icicle(&icicle_ntt_holder);

  end = base::TimeTicks::Now();
  std::cout << "Time taken for initializing Icicle NTT domain: " << end - start
            << std::endl;
  start = end;
#endif
  std::cout << "Start proving" << std::endl;
  ProverTime prover_time;
  for (size_t i = 0; i < num_runs; ++i) {
    std::vector<F> h_evals =
        QuadraticArithmeticProgram<F>::WitnessMapFromMatrices(
            domain.get(), constraint_matrices, full_assignments);
    if (no_zk) {
      proof = zk::r1cs::groth16::CreateProofWithAssignmentNoZK(
          proving_key, absl::MakeConstSpan(h_evals),
          full_assignments.subspan(
              1, constraint_matrices.num_instance_variables - 1),
          full_assignments.subspan(constraint_matrices.num_instance_variables),
          full_assignments.subspan(1));
    } else {
      proof = zk::r1cs::groth16::CreateProofWithAssignmentZK(
          proving_key, absl::MakeConstSpan(h_evals),
          full_assignments.subspan(
              1, constraint_matrices.num_instance_variables - 1),
          full_assignments.subspan(constraint_matrices.num_instance_variables),
          full_assignments.subspan(1));
    }

    end = base::TimeTicks::Now();
    base::TimeDelta delta = end - start;
    std::cout << "Time taken for proving #" << i << ": " << delta << std::endl;
    prover_time.Add(delta);
    start = end;
  }
  std::cout << "Avg time taken for proving: " << prover_time.GetAvg()
            << std::endl;
  std::cout << "Max time taken for proving: " << prover_time.max << std::endl;

  absl::Span<const F> public_inputs = full_assignments.subspan(
      1, constraint_matrices.num_instance_variables - 1);
  if (verify) {
    std::cout << "Start verifying" << std::endl;
    zk::r1cs::groth16::PreparedVerifyingKey<Curve> prepared_verifying_key =
        std::move(proving_key).TakeVerifyingKey().ToPreparedVerifyingKey();
    CHECK(zk::r1cs::groth16::VerifyProof(prepared_verifying_key, proof,
                                         public_inputs));
    end = base::TimeTicks::Now();
    std::cout << "Time taken for verifying: " << end - start << std::endl;
  }

  CHECK(WriteToJson(proof, proof_path));
  std::cout << "Proof is saved to \"" << proof_path << "\"" << std::endl;
  CHECK(WriteToJson(public_inputs, public_path));
  std::cout << "Public input is saved to \"" << public_path << "\""
            << std::endl;
}

}  // namespace circom

int RealMain(int argc, char** argv) {
  base::FlagParser parser;
  base::FilePath zkey_path;
  base::FilePath witness_path;
  base::FilePath proof_path;
  base::FilePath public_path;
  Curve curve;
  bool no_zk;
  bool verify;
  size_t num_runs;
  parser.AddFlag<base::FilePathFlag>(&zkey_path)
      .set_name("zkey")
      .set_help("The path to zkey file");
  parser.AddFlag<base::FilePathFlag>(&witness_path)
      .set_name("wtns")
      .set_help("The path to wtns file");
  parser.AddFlag<base::FilePathFlag>(&proof_path)
      .set_name("proof")
      .set_help("The path to proof json");
  parser.AddFlag<base::FilePathFlag>(&public_path)
      .set_name("public")
      .set_help("The path to public json");
  parser.AddFlag<base::Flag<Curve>>(&curve)
      .set_long_name("--curve")
      .set_default_value(Curve::kBN254)
      .set_help(
          "The curve type among ('bn254', bls12_381'), by default 'bn254'");
  parser.AddFlag<base::BoolFlag>(&no_zk)
      .set_long_name("--no_zk")
      .set_default_value(false)
      .set_help(
          "Create proof without zk. By default zk is enabled. Use this flag to "
          "compare the proof with rapidsnark.");
  parser.AddFlag<base::BoolFlag>(&verify)
      .set_long_name("--verify")
      .set_default_value(false)
      .set_help(
          "Verify the proof. By default verify is disabled. Use this flag "
          "to verify the proof with the public inputs.");
  parser.AddFlag<base::Flag<size_t>>(&num_runs)
      .set_short_name("-n")
      .set_long_name("--num_runs")
      .set_default_value(1)
      .set_help("The number of times to run the proof generation");

  std::string error;
  if (!parser.Parse(argc, argv, &error)) {
    tachyon_cerr << error << std::endl;
    return 1;
  }
#if TACHYON_CUDA
  if (curve == Curve::kBLS12_381) {
    tachyon_cerr << "BLS12_381 curve is not supported on CUDA" << std::endl;
    return 1;
  }
#endif
  if (num_runs == 0) {
    tachyon_cerr << "num_runs should be positive" << std::endl;
    return 1;
  }
  switch (curve) {
    case Curve::kBN254:
      circom::CreateProof<math::bn254::BN254Curve>(zkey_path, witness_path,
                                                   proof_path, public_path,
                                                   no_zk, num_runs, verify);
      break;
    case Curve::kBLS12_381:
#if !TACHYON_CUDA
      circom::CreateProof<math::bls12_381::BLS12_381Curve>(
          zkey_path, witness_path, proof_path, public_path, no_zk, num_runs,
          verify);
#endif
      break;
  }

  return 0;
}

}  // namespace tachyon

int main(int argc, char** argv) { return tachyon::RealMain(argc, argv); }
