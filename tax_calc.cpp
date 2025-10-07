#include <algorithm>
#include <stdexcept>

struct TaxInput {
    long long grossIncome;
    int       dependents;
    long long insurancePaid;
};

static inline long long standardDeduction() {
    return 10'000'000;
}

static inline long long dependentDeduction(int dependents) {
    int validDeps = std::clamp(dependents, 0, 2);
    return 4'000'000LL * validDeps;
}

static inline long long insuranceDeduction(long long gross, long long paid) {
    if (gross <= 0) return 0;
    long long maxAllowed = std::min(gross / 10, 5'000'000LL);
    return std::clamp<long long>(paid, 0, maxAllowed);
}

static long long progressiveTax(long long taxable) {
    if (taxable <= 0) return 0;
    long long tax = 0;
    long long first = std::min<long long>(taxable, 5'000'000);
    tax += (first * 5 + 50) / 100;
    if (taxable <= 5'000'000) return tax;
    long long remaining = taxable - 5'000'000;
    long long second = std::min<long long>(remaining, 10'000'000);
    tax += (second * 10 + 50) / 100;
    if (remaining <= 10'000'000) return tax;
    long long third = remaining - 10'000'000;
    tax += (third * 20 + 50) / 100;
    return tax;
}

long long computeTax(const TaxInput& in) {
    if (in.grossIncome < 0 || in.insurancePaid < 0 || in.dependents < 0) {
        throw std::invalid_argument("Negative values not allowed");
    }
    long long deductions = standardDeduction()
                         + dependentDeduction(in.dependents)
                         + insuranceDeduction(in.grossIncome, in.insurancePaid);
    long long taxableIncome = std::max<long long>(0, in.grossIncome - deductions);
    return progressiveTax(taxableIncome);
}
