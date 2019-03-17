#ifndef FINANCE_H_
#define FINANCE_H_

/**
 * Computes the future value.
 *
 * @param pv   a present value
 * @param rate an interest 'rate' compounded once per period
 * @param n    the number of periods (e.g., 10 years)
 *
 * @return the value at the end of 'n' periods
 */
double future_value(double pv, double rate, double n);

/**
 * Computes the present value.
 *
 * @param fv   a future value
 * @param rate an interest 'rate' compounded once per period
 * @param n    the number of periods (e.g., 10 years).
 */
double present_value(double fv, double rate, double n);

#endif  /* FINANCE_H_ */
