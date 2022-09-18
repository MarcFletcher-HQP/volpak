#include <Rcpp.h>



class A {

public:

  A(double a){
    this->a = a;
  }

  virtual double times(){return a;};

  virtual ~A(){};


public:

  double a = 0.0;

};




class B : public A {

public:

  B(double a, double b) : A(a){
    this->b = b;
  }

  virtual double times(){ return a * b; }

  virtual ~B(){};


public:

  double b = 0.0;

};



bool ptr_is_B(A * ptr){

  return (dynamic_cast<B*>(ptr) != NULL);

}




// [[Rcpp::export]]
Rcpp::NumericVector times(Rcpp::NumericVector x, Rcpp::NumericVector y) {


  // B b(x[0], y[0]);
  A b(x[0]);

  if(ptr_is_B(&b)){

    Rprintf("B: %.1f  %.1f\n", x[0], y[0]);

  } else {

    Rprintf("A: %.1f\n", x[0]);

  }


  return b.times();
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
times(5, 8)
*/
