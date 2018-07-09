#include <stdio.h>

#include "stats.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

int main(void)
{
  double a[] = {1.6, 2.7, 3.8, 4.9};
  double sum = 0.0;
  double mean = 0.0;
  double variance = 0.0;
  double stdev = 0.0;

  sum = stats_sum(a, ARRAY_SIZE(a));
  mean = stats_mean(a, ARRAY_SIZE(a));
  variance = stats_var(a, ARRAY_SIZE(a));
  stdev = stats_stdev(a, ARRAY_SIZE(a));

  printf("sum %f\n", sum);
  printf("mean %f\n", mean);
  printf("variance %f\n", variance);
  printf("standard deviation %f\n", stdev);

  return 0;
}
