#include <benchmark/benchmark.h>

#include "strategy.h"
#include <random>

namespace
{
    constexpr Sint16 operator ""_si16(unsigned long long __value) noexcept
        { return static_cast<Sint16>(__value); }

    Sint16 random()
    {
        std::random_device device;
        std::mt19937 generator(device());
        std::bernoulli_distribution is_empty(.8);
        std::bernoulli_distribution player;

        return
            is_empty(generator) ? -1_si16 :
            static_cast<Sint16>(player(generator));
    }

    class BM_Strategy : public benchmark::Fixture
    {
    public:
        void SetUp(benchmark::State& __state) override
        {
            bidiarray<bool> holes;

            for (std::size_t i = 0; i != 8; ++i) {
                for (std::size_t j = 0; j != 8; ++j) {
                    bool is_hole = (i % 2) == (j % 2);

                    blobs.set(i, j, is_hole ? -1 : random());
                    holes.set(i, j, is_hole);
                }
            }

            strategy = Strategy(blobs, holes, 0, [] (movement&) {});
        }
    public:
        bidiarray<Sint16> blobs;
        Strategy strategy;
    };

    BENCHMARK_DEFINE_F(BM_Strategy, BM_min_max_seq)(benchmark::State& __state)
    {
        strategy._max_level = static_cast<Uint16>(__state.range(0));

        for (auto _ : __state) {
            strategy.min_max_seq(blobs, 1, 0);
        }

        __state.SetComplexityN(__state.range(0));
    }

    BENCHMARK_DEFINE_F(BM_Strategy, BM_min_max_para)(benchmark::State& __state)
    {
        strategy._max_level = static_cast<Uint16>(__state.range(0));

        for (auto _ : __state) {
            strategy.min_max_para(blobs, 1, 0);
        }

        __state.SetComplexityN(__state.range(0));
    }

    BENCHMARK_DEFINE_F(BM_Strategy, BM_alpha_beta_seq)(
        benchmark::State& __state
    )
    {
        strategy._max_level = static_cast<Uint16>(__state.range(0));

        for (auto _ : __state) {
            strategy.alpha_beta_seq(
                blobs,
                1,
                0,
                -std::numeric_limits<Sint32>::max(),
                std::numeric_limits<Sint32>::max()
            );
        }

        __state.SetComplexityN(__state.range(0));
    }
}

BENCHMARK_REGISTER_F(BM_Strategy, BM_min_max_seq)
    ->DenseRange(1, 6)
    ->Complexity();

BENCHMARK_REGISTER_F(BM_Strategy, BM_min_max_para)
    ->DenseRange(1, 6)
    ->Complexity();

BENCHMARK_REGISTER_F(BM_Strategy, BM_alpha_beta_seq)
    ->DenseRange(1, 6)
    ->Complexity();

BENCHMARK_MAIN();
