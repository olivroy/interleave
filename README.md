
[![R build
status](https://github.com/dcooley/interleave/workflows/R-CMD-check/badge.svg)](https://github.com/dcooley/interleave/actions)
[![Codecov test
coverage](https://codecov.io/gh/dcooley/interleave/branch/master/graph/badge.svg)](https://codecov.io/gh/dcooley/interleave?branch=master)

<!-- README.md is generated from README.Rmd. Please edit that file -->

<img src="./man/figures/interleaving.gif" width="256px" height="256px" />

# interleave

I’m building this library to be the workhorse for converting matrices
(and lists of matrices) into single, interleaved vectors, ready for
WebGL applications.

``` r

( mat1 <- matrix(1:20, ncol = 2, byrow = T) )
#       [,1] [,2]
#  [1,]    1    2
#  [2,]    3    4
#  [3,]    5    6
#  [4,]    7    8
#  [5,]    9   10
#  [6,]   11   12
#  [7,]   13   14
#  [8,]   15   16
#  [9,]   17   18
# [10,]   19   20

interleave( mat1 )
#  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20

( mat2 <- matrix(20:1, ncol = 5, byrow = T) )
#      [,1] [,2] [,3] [,4] [,5]
# [1,]   20   19   18   17   16
# [2,]   15   14   13   12   11
# [3,]   10    9    8    7    6
# [4,]    5    4    3    2    1

interleave( mat2 )
#  [1] 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1

lst <- list( mat1, mat2 )

interleave( lst )
#  [1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 20 19 18 17 16
# [26] 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1
```

## Ear-cutting / geometries / polygon structures

Isn’t this what `decido` is for?

Yes and no. I’m using this repo to experiment. Dunno if it’s going to
stay here or not. I’ve deviated away from `mapbox::earcut` slightly by
returning interleaved coordinates directly (rather than indicies), and I
accept any dimension (XY(Z(M(…n))), and so I can’t make it nicely fit in
`decido` (yet).

``` r
x <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
y <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
mat <- cbind(x, y)
lst <- list( mat )
interleave:::rcpp_earcut( lst )
# $coordinates
#  [1] 0.00 1.00 0.00 0.00 0.69 0.00 0.69 0.00 0.80 0.60 0.50 0.70 0.50 0.70 1.00
# [16] 0.80 0.75 1.00 0.00 1.00 0.69 0.00 0.50 0.70 0.50 0.70 0.75 1.00 0.00 1.00
# 
# $stride
# [1] 2
```

With a hole

``` r

x1 <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
x2 <- c(0.2, 0.5, 0.5, 0.3, 0.2)
y1 <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
y2 <- c(0.2, 0.2, 0.4, 0.6, 0.4)

mat1 <- cbind(x1, y1)
mat2 <- cbind(x2, y2)

lst <- list( mat1, mat2 )

interleave:::rcpp_earcut( lst )
# $coordinates
#  [1] 0.00 0.00 0.20 0.20 0.20 0.40 0.50 0.20 0.20 0.20 0.00 0.00 0.69 0.00 0.80
# [16] 0.60 0.50 0.70 0.50 0.70 1.00 0.80 0.75 1.00 0.00 1.00 0.00 0.00 0.20 0.40
# [31] 0.50 0.20 0.00 0.00 0.69 0.00 0.50 0.70 0.75 1.00 0.00 1.00 0.00 1.00 0.20
# [46] 0.40 0.30 0.60 0.50 0.40 0.50 0.20 0.69 0.00 0.50 0.70 0.00 1.00 0.30 0.60
# [61] 0.50 0.40 0.69 0.00 0.50 0.70 0.50 0.70 0.30 0.60 0.50 0.40
# 
# $stride
# [1] 2
```

**But Dave, why not just transpose a matrix and remove the dimension?**

Yeah, of course you can do that if you want. But my goal is to make
these functions callable through C++ directly, by other packages. And it
benchmarks pretty well.

``` r

mat <- matrix(rnorm(5e6), ncol = 5, byrow = T)

microbenchmark::microbenchmark(
  leave = { interleave( mat ) },
  baset = {
    mat2 <- t( mat )
    dim( mat2 ) <- NULL
    },
  times = 25
)
# Unit: milliseconds
#   expr       min       lq     mean   median       uq      max neval
#  leave 11.277468 13.32364 25.82973 16.74018 49.91673 64.96434    25
#  baset  9.901499 11.93108 20.68320 15.32377 16.84644 55.94543    25



mat <- matrix(1e6, ncol = 2)
lst <- list( mat, mat, mat, mat, mat )

microbenchmark::microbenchmark(
  leave = { interleave( lst ) },
  baset = {
    unlist(
      lapply(lst, function(x) {
        y <- t(x)
        dim( y ) <- NULL
        y
      })
    )
  },
  times = 25
)
# Unit: microseconds
#   expr    min     lq      mean median     uq       max neval
#  leave  4.208  4.977   5.96672  5.259  6.193    18.895    25
#  baset 15.456 16.204 719.45748 17.162 17.702 17567.680    25
```

## Interleaved object

  - A **coordinate** is a set of values describing a position, typically
    denoted by `(x, y)`, although any number of values are allowed.
  - The number of values in a **coordinate** is the **stride**. Often
    referred to as the *dimension* (XY)

Within the interleaved object

  - **coordinates** - all the coordinates as a single, interleaved
    vector
  - **stride** - the number of values in each coordinate.
  - **n\_coordinates** - the number of coordinates in each geometry
  - **start\_indices** - the index in the **coordinates** vector where
    each geometry starts, divided by the stride.

Wait, what? Can you explain the **start\_indices** again?

Ok, the **start\_indices** is **not** the index in the interleaved
vector where each coordinate starts.

If you imagine two lines represented by two matrices, one with seven
rows and the other with 4.

``` r
x1 <- c(1:7)
x2 <- c(8:11)
y1 <- c(1:7)
y2 <- c(8:11)

mat1 <- cbind(x1, y1)
mat2 <- cbind(x2, y2)
```

Then the interleaved object of these lines is

``` r
( lst <- list( mat1, mat2 ) )
# [[1]]
#      x1 y1
# [1,]  1  1
# [2,]  2  2
# [3,]  3  3
# [4,]  4  4
# [5,]  5  5
# [6,]  6  6
# [7,]  7  7
# 
# [[2]]
#      x2 y2
# [1,]  8  8
# [2,]  9  9
# [3,] 10 10
# [4,] 11 11

interleave:::rcpp_interleave_line( lst, 2 )
# $coordinates
#  [1]  1  1  2  2  3  3  4  4  5  5  6  6  7  7  8  8  9  9 10 10 11 11
# 
# $start_indices
# [1] 0 6
# 
# $stride
# [1] 2
```

The first geometry obviously starts at index 0. The second geometry
starts at index 7 multiplied by the stride; 7 x 2 = 14. So you can see
in the **coordinates** object the (8, 8) pair of coordinates starts at
index 14 (using 0-based indexing)

Finally, you can get the total number of coordinates for all the
geometries by `length( coordinates ) / stride`

## Interleave Point

``` r

## Assume a 'linestring' object (i.e., a matrix)
( mat <- matrix(1:10, ncol = 2) )
#      [,1] [,2]
# [1,]    1    6
# [2,]    2    7
# [3,]    3    8
# [4,]    4    9
# [5,]    5   10

interleave:::rcpp_interleave_point( mat, 2 )
# $coordinates
#  [1]  1  6  2  7  3  8  4  9  5 10
# 
# $start_indices
# [1] 0 1 2 3 4
# 
# $stride
# [1] 2
```

## Interleave Triangle

``` r

x1 <- c(0, 0, 0.75, 1, 0.5, 0.8, 0.69)
x2 <- c(0.2, 0.5, 0.5, 0.3, 0.2)
y1 <- c(0, 1, 1, 0.8, 0.7, 0.6, 0)
y2 <- c(0.2, 0.2, 0.4, 0.6, 0.4)

mat1 <- cbind(x1, y1)
mat2 <- cbind(x2, y2)

lst <- list( mat1, mat2 )

interleave:::rcpp_interleave_triangle( lst )
# $coordinates
#  [1] 0.00 0.00 0.20 0.20 0.20 0.40 0.50 0.20 0.20 0.20 0.00 0.00 0.69 0.00 0.80
# [16] 0.60 0.50 0.70 0.50 0.70 1.00 0.80 0.75 1.00 0.00 1.00 0.00 0.00 0.20 0.40
# [31] 0.50 0.20 0.00 0.00 0.69 0.00 0.50 0.70 0.75 1.00 0.00 1.00 0.00 1.00 0.20
# [46] 0.40 0.30 0.60 0.50 0.40 0.50 0.20 0.69 0.00 0.50 0.70 0.00 1.00 0.30 0.60
# [61] 0.50 0.40 0.69 0.00 0.50 0.70 0.50 0.70 0.30 0.60 0.50 0.40
# 
# $start_indices
#  [1]  0  3  6  9 12 15 18 21 24 27 30 33
# 
# $stride
# [1] 2
```
