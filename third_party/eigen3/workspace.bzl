"""Provides the repository macro to import Eigen."""

load("//third_party:repo.bzl", "tachyon_http_archive", "tf_mirror_urls")

def repo():
    """Imports Eigen."""

    # Attention: tools parse and update these lines.
    # LINT.IfChange
    EIGEN_COMMIT = "0b51f763cbbd0ed08168f88972724329f0375498"
    EIGEN_SHA256 = "70a3b0e357fc037740002f5097a15dba1ea0dde28d37f5d9c86f76a06626f4fc"
    # LINT.ThenChange(//tensorflow/lite/tools/cmake/modules/eigen.cmake)

    tachyon_http_archive(
        name = "eigen_archive",
        build_file = "//third_party/eigen3:eigen_archive.BUILD",
        sha256 = EIGEN_SHA256,
        strip_prefix = "eigen-{commit}".format(commit = EIGEN_COMMIT),
        urls = tf_mirror_urls("https://gitlab.com/libeigen/eigen/-/archive/{commit}/eigen-{commit}.tar.gz".format(commit = EIGEN_COMMIT)),
        patch_file = [
            "@kroma_network_tachyon//third_party/eigen3:add_sfinae_to_scalar_random_op.patch",
            "@kroma_network_tachyon//third_party/eigen3:add_sfinae_to_num_traits.patch",
            "@kroma_network_tachyon//third_party/eigen3:use_faster_constant_method.patch",
            "@kroma_network_tachyon//third_party/eigen3:allow_field_inverse.patch",
        ],
    )
