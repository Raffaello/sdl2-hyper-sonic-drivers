f <- 100
xs <- seq(0, f-1) 
amin <- -128
amax <- 127
umax <- amax - amin

gen_square <- function(t, f) {
  if (t < f/2) {
    amax
  } else {
    amin
  }
}

gen_saw <- function(t, f) {
  if (f == 0) {
    0
  } else {
    t * umax / f + amin
  }
}

gen_tri <- function(t, f) {
  if (f==0) {
    0
  } else {
    f2 <- f/2
    if (t < f2) {
      t * umax / f2 + amin
    } else {
      amax - ((t-f2)*umax/f2)
      
      
    }
  }
}

gen_sin <- function(t, f) {
  if (f==0) {
    0
  } else {
    amax * sin(2*pi * t / f)
  }
}

square <- sapply(xs, FUN=gen_square, f)

saw <- sapply(xs, FUN=gen_saw, f)

tri <- sapply(xs, FUN=gen_tri, f)

s <- sapply(xs, FUN=gen_sin, f)

plot(xs, square, type="o")
plot(xs, saw, type="o")
plot(xs, tri, type="o")
plot(xs, s, type="o")
